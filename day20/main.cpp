
#include <iostream>
#include <map>
#include <numeric>
#include <ostream>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <generator>
#include <omp.h>
#include <queue>

#include "../utils/utils.h"
#include "../easyTimer.h"

using dt = int64_t;
using P = Point2d<dt>;
using Cache = std::unordered_map<P, dt>;
using Cache2 = std::unordered_map<std::pair<P, dt>, dt, hash_tuple>;
constexpr auto INF = std::numeric_limits<dt>::max();
constexpr dt MAX_LEVEL = 50;
constexpr dt MAX_DEPTH = 12000;

std::pair<std::vector<std::vector<char>>, std::vector<std::tuple<std::string, P>>> getInput(const std::string& fp){
    auto lines = read_file(fp);
    std::vector<std::vector<char>> out;
    std::vector<std::tuple<std::string, P>> portals;

    for (const auto& [i, l]: enumerate(lines))
    {
        if (l.empty())
        {
            break;
        }
        out.emplace_back();
        for (const auto& [j,c]: enumerate(l))
        {
            if (c == '#' || c == '.' || c == ' ')
            {
                out.back().emplace_back(c);
            }
            else
            {
                out.back().emplace_back('@');
            }

            if (std::isalpha(c))
            {
                // The portal labels are read left to right or up to down

                // Check horizontal
                if (j < static_cast<long>(l.size()-1))
                {
                    if (std::isalpha(l[j+1]))
                    {
                        std::string tmp{c, l[j+1]};
                        //Is a portal
                        if (j >= 1 && l[j-1] == '.')
                        {
                            portals.emplace_back(tmp, P(j,i));
                        } else if(j < static_cast<long>(l.size()-2) && l[j+2] == '.'){
                            portals.emplace_back(tmp, P(j+1,i));
                        }
                    }
                }
                // Check vertical
                if (i < static_cast<long>(lines.size()-1))
                {
                    if (std::isalpha(lines[i+1][j]))
                    {
                        std::string tmp{c, lines[i+1][j]};
                        //Is a portal
                        if (i >= 1 && lines[i-1][j] == '.')
                        {
                            portals.emplace_back(tmp, P(j,i));
                        } else if(i < static_cast<long>(lines.size()-2) && lines[i+2][j] == '.'){
                            portals.emplace_back(tmp, P(j,i+1));
                        }
                    }
                }
            }
        }

    }
    return {out, portals};

}

void shortest_path_helper(const std::vector<std::vector<char>>& map,
    const std::unordered_map<std::string, std::pair<P, P>>& portal_locs,
    const std::unordered_map<P, std::string>& loc_to_p,
    const P& pos, const P& target,
    const dt depth,
    dt& best,
    Cache& cache)
{
    if (cache.contains(pos) && cache[pos] < depth)
    {
        return;
    }
    cache[pos] = depth;
    std::println("{}", pos);
    if (pos == target)
    {
        if (depth < best)
        {
            best = depth;
        }
        return;
    }

    for (const P& next: std::initializer_list<P>({{1,0}, {-1,0}, {0,1}, {0,-1}}))
    {
        auto next_pos = pos+next;

        // If it stepped into a portal
        bool portal = false;
        if (auto f = loc_to_p.find(next_pos); f != loc_to_p.end())
        {
            portal = true;
            const auto portal_name = f->second;
            const auto& [a,b] = portal_locs.find(portal_name)->second;
            if (a == next_pos)
            {
                next_pos = b;
            }else
            {
                next_pos = a;
            }
        }
        if (portal || map[next_pos.y][next_pos.x] == '.' || next_pos == target)
        {
            shortest_path_helper(map, portal_locs, loc_to_p, next_pos, target, depth+1-portal, best, cache);
        }
    }
}

dt shortest_path(const std::vector<std::vector<char>>& map,
    const std::unordered_map<std::string, std::pair<P, P>>& portal_locs,
    const std::unordered_map<P, std::string>& loc_to_p,
    const P& start, const P& end)
{

    Cache c;
    dt best = INF;
    shortest_path_helper(map, portal_locs, loc_to_p, start, end, 0, best, c);
    return best-2;
}

void shortest_path_helper2(const std::vector<std::vector<char>>& map,
    const std::unordered_map<std::string, std::pair<P, P>>& portal_locs,
    const std::unordered_map<P, std::string>& loc_to_p,
    const P& pos, const P& target,
    const dt depth,
    const dt level,
    dt& best,
    Cache2& cache)
{
    if (level+depth > best || level > MAX_LEVEL || depth > MAX_DEPTH)
    {
        return;
    }
    if (cache.contains({pos, level}) && cache[{pos, level}] < depth)
    {
        return;
    }
    cache[{pos, level}] = depth;

    if (pos == target)
    {
        if (depth < best && level == 0)
        {
            best = depth;
        }
        return;
    }

    for (const P& next: std::initializer_list<P>({{1,0}, {-1,0}, {0,1}, {0,-1}}))
    {
        auto next_pos = pos+next;
        auto next_level = level;
        // If it stepped into a portal
        bool portal = false;
        if (auto f = loc_to_p.find(next_pos); f != loc_to_p.end())
        {
            portal = true;
            const auto portal_name = f->second;
            const auto& [outter,inner] = portal_locs.find(portal_name)->second;
            if (outter == next_pos)
            {
                if (level > 0)
                {
                    next_pos = inner;
                    next_level--;
                }
                else
                {
                    portal = false;
                }
            }else
            {
                next_pos = outter;
                next_level++;
            }
        }
        if (portal || map[next_pos.y][next_pos.x] == '.' || (next_pos == target && next_level==0))
        {
            shortest_path_helper2(map, portal_locs, loc_to_p, next_pos, target, depth+1-portal, next_level, best, cache);
        }
    }
}

dt shortest_path2(const std::vector<std::vector<char>>& map,
    const std::unordered_map<std::string, std::pair<P, P>>& portal_locs,
    const std::unordered_map<P, std::string>& loc_to_p,
    const P& start, const P& end)
{

    Cache2 c;
    dt best = INF;
    shortest_path_helper2(map, portal_locs, loc_to_p, start, end, 0,0, best, c);
    return best-2;
}

int main(const int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid args" << std::endl;
        return 1;
    }
    auto [map, portals_pre] = getInput(argv[1]);
    for(const auto& r: map){
     for(const auto& c: r){
           std::print("{}", c);
       }
        std::print("\n");
    }

    std::unordered_map<std::string, std::pair<P, P>> portal_locs;
    std::unordered_map<P, std::string> loc_to_p;
    P start{}, end{};
    for (const auto&[l, p]: portals_pre)
    {
        if (l == "AA")
        {
            start = p;
            continue;
        }
        if (l == "ZZ")
        {
            end = p;
            continue;
        }

        // if (portal_locs.contains(l))
        if (p.x <=2 || p.x >= static_cast<dt>(map[map.size()/2].size()-2) || p.y <=2 || p.y >= static_cast<dt>(map.size()-2))
        {
            portal_locs[l].first = p;
        }else
        {
            portal_locs[l].second = p;
        }
        loc_to_p[p] = l;
    }

    auto best = shortest_path(map, portal_locs, loc_to_p, start, end);
    std::println("{}", best);
    auto best2 = shortest_path2(map, portal_locs, loc_to_p, start, end);
    std::println("{}", best2);
}