
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
using pd = std::pair<P, dt>;
using dk_locs = std::unordered_map<char, pd>;
constexpr auto INF = std::numeric_limits<dt>::max();

std::vector<std::vector<char>> getInput(const std::string& fp){
    auto lines = read_file(fp);
    std::vector<std::vector<char>> out;

    for (const auto& l: lines)
    {
        if (l.empty())
        {
            break;
        }
        out.emplace_back();
        out.back().insert(out.back().end(), l.begin(), l.end());

    }
    return out;

}

[[nodiscard]] inline bool is_upper(const char c)
{
    return c >= 'A' && c <= 'Z';
}
[[nodiscard]] inline bool is_lower(const char c)
{
    return c >= 'a' && c <= 'z';
}

[[nodiscard]] inline char to_upper(const char c)
{

    return c & ~static_cast<char>(0b00100000);
}
[[nodiscard]] inline char to_lower(const char c)
{
    return c | static_cast<char>(0b00100000);
}

void flood_fill(const std::vector<std::vector<char>>& map, dk_locs& doors, dk_locs& keys, std::unordered_set<P>& visited, const P pos, const dt depth)
{
    if (const auto c = map[pos.y][pos.x]; is_upper(c))
    {
        if (const auto f = doors.find(c); f == doors.end() || f->second.second > depth)
        {
            doors[c] = {pos, depth};
        }
        return;
    }
    else if (is_lower(c))
    {
        if (const auto f = keys.find(c); f == keys.end() || f->second.second > depth)
        {
            keys[c] = {pos, depth};
        }
        return;
    }
    else if (map[pos.y][pos.x] == '#')
    {
        return;
    }
    if (visited.contains(pos))
    {
        return;
    }
    visited.emplace(pos);
    for (const P& next: std::initializer_list<P>({{1,0}, {-1,0}, {0,1}, {0,-1}}))
    {
        flood_fill(map, doors, keys, visited, pos+next, depth+1);
    }
}

void flood_fill_haxs(const std::vector<std::vector<char>>& map, dk_locs& doors, dk_locs& keys, std::unordered_map<P, dt>& visited, const P pos, const dt depth)
{
    if (const auto c = map[pos.y][pos.x]; is_upper(c))
    {
        if (const auto f = doors.find(c); f == doors.end() || f->second.second > depth)
        {
            doors[c] = {pos, depth};
        }
    }
    else if (is_lower(c))
    {
        if (const auto f = keys.find(c); f == keys.end() || f->second.second > depth)
        {
            keys[c] = {pos, depth};
        }
    }
    else if (map[pos.y][pos.x] == '#')
    {
        return;
    }
    if (visited.contains(pos) && visited[pos] < depth)
    {
        return;
    }
    visited[pos] = depth;
    for (const P& next: std::initializer_list<P>({{1,0}, {-1,0}, {0,1}, {0,-1}}))
    {
        flood_fill_haxs(map, doors, keys, visited, pos+next, depth+1);
    }
}

using Cache = std::map<std::tuple<char, std::set<char>>, dt>;

dt find_avaliable_helper(std::vector<std::vector<char>>& map,
                        const std::vector<std::vector<dt>>& distances,
                        const std::vector<std::vector<char>>& unlocks,
                        std::vector<char>& needed_keys,
                        std::vector<char>& avail,
                        const char cur_c,
                        std::vector<char>& path,
                        const dt cur_path_len,
                        std::vector<char>& owned_keys,
                        dt& best_path_len,
                        Cache& cache)
{
    // print_arr(std::span(path));
    if (needed_keys.empty())
    {
        std::println("best = {}", cur_path_len);
        if (cur_path_len < best_path_len)
        {
            best_path_len = cur_path_len;
        }
        return cur_path_len;
    }

    const auto s = std::set(owned_keys.begin(), owned_keys.end());
    if (cache.contains({cur_c, s}))
    {
        const auto tmp = cache[{cur_c, s}];
        if (tmp+cur_path_len < best_path_len)
        {
            best_path_len = tmp+cur_path_len;
        }
        return tmp+cur_path_len;
    }

    dt this_best = INF/2;
    const auto avail_cpy = avail;
    for (const auto& c: avail_cpy)
    {
            const auto next_distance = cur_path_len+distances[cur_c-'A'][c-'A'];
            // std::println("keys left: {}", needed_keys.size());
            if (is_lower(c))
            {
                needed_keys.erase(std::ranges::find(needed_keys, c));
                avail.erase(std::ranges::find(avail, c));
                owned_keys.emplace_back(c);
                avail.insert(avail.end(), unlocks[c-'A'].begin(), unlocks[c-'A'].end());

                if (cur_path_len == 0)
                {
                    std::println("Trying to clear key {}, best={}", c, best_path_len);
                }
                path.emplace_back(c);

                const auto pl = find_avaliable_helper(map, distances, unlocks, needed_keys, avail, c, path, next_distance, owned_keys, best_path_len, cache);
                // reset the map and sets
                path.pop_back();
                needed_keys.emplace_back(c);
                avail.emplace_back(c);
                owned_keys.pop_back();
                for (const auto& unlk: unlocks[c-'A'])
                {
                    avail.erase(std::ranges::find(avail, unlk));
                }
                if (pl-next_distance+distances[cur_c-'A'][c-'A'] < this_best)
                {
                    this_best = pl-next_distance+distances[cur_c-'A'][c-'A'];
                }
            }
            else
            {
                if (const auto f = std::ranges::find(owned_keys, to_lower(c)); f != owned_keys.end())
                {
                    avail.erase(std::ranges::find(avail, c));
                    for (const auto& unlk: unlocks[c-'A'])
                    {
                        avail.emplace_back(unlk);
                    }

                    if (cur_path_len == 0)
                    {
                        std::println("Trying to clear door {}, best={}", c, best_path_len);
                    }
                    path.emplace_back(c);
                    const auto pl = find_avaliable_helper(map, distances, unlocks, needed_keys, avail, c, path, next_distance, owned_keys, best_path_len, cache);
                    path.pop_back();
                    avail.emplace_back(c);
                    for (const auto& unlk: unlocks[c-'A'])
                    {
                        avail.erase(std::ranges::find(avail, unlk));
                    }
                    if (pl-next_distance+distances[cur_c-'A'][c-'A'] < this_best)
                    {
                        this_best = pl-next_distance+distances[cur_c-'A'][c-'A'];
                    }
                }
            }
        }
    cache[{cur_c, s}] = this_best;
    return cur_path_len+this_best;
}

void findAvailable(std::vector<std::vector<char>> map)
{
    // vector might be faster, only upto 26 charaters per list
    std::vector<char> needed_keys;
    std::unordered_map<char, P> locs;
    P pos(0,0);
    {
        for(const auto& [i,r]: enumerate(map)){
            for(const auto& [j,c]: enumerate(r)){
                if (c == '@')
                {
                    pos = {j,i};

                    // Removing the robot from the map so I don't have to worry about it's charater
                    // I will just update its location
                    map[i][j] = '.';

                    // ^ is between the upper and lower case charaters
                    locs['^'] = {j,i};
                }
                else if (is_lower(c) || is_upper(c))
                {
                    if (is_lower(c))
                    {
                        needed_keys.emplace_back(c);
                    }
                    locs[c] = {j,i};
                }
            }
        }
    }

    std::vector<std::vector<dt>> distances('z'-'A'+1);
    for (auto& v: distances)
    {
        v.resize('z'-'A'+1);
    }
    for (const auto& [c,p]: locs)
    {
        dk_locs doors, keys;
        std::unordered_map<P, dt> visited;
        flood_fill_haxs(map, doors, keys, visited, p, 0);
        for (const auto& [dc, v]: doors)
        {
            distances[c-'A'][dc-'A'] = v.second;
        }
        for (const auto& [dc, v]: keys)
        {
            distances[c-'A'][dc-'A'] = v.second;
        }
    }
    std::vector<std::vector<char>> unlocks('z'-'A'+1);
    std::queue<std::pair<char, P>> todo;
    std::unordered_set<char> done;
    std::vector<char> initial_avail;
    {
        dk_locs doors, keys;
        std::unordered_set<P> visited;
        flood_fill(map, doors, keys, visited, pos, 0);

        for (const auto& [c, snd]: doors)
        {
            todo.emplace(c, snd.first);
            done.emplace(c);
            initial_avail.emplace_back(c);
        }
        for (const auto& [c, snd]: keys)
        {
            todo.emplace(c, snd.first);
            done.emplace(c);
            initial_avail.emplace_back(c);
        }
    }
    auto map_cpy = map;
    while (!todo.empty())
    {
        auto [c, p] = todo.front();
        todo.pop();
        map_cpy[p.y][p.x] = '.';
        dk_locs doors, keys;
        std::unordered_set<P> visited;
        flood_fill(map_cpy, doors, keys, visited, p, 0);
        for (const auto& [dc, v] : doors)
        {

            if (!done.contains(dc))
            {
                unlocks[c-'A'].emplace_back(dc);
                todo.emplace(dc, v.first);
                done.emplace(dc);
            }
        }
        for (const auto& [kc, v] : keys)
        {
            if (!done.contains(kc))
            {
                unlocks[c-'A'].emplace_back(kc);
                todo.emplace(kc, v.first);
                done.emplace(kc);
            }
        }
    }


    std::vector<char> owned_keys;
    dt best_path_len = INF;
    std::vector<char> path;
    Cache c = {{}};
    find_avaliable_helper(map, distances, unlocks, needed_keys, initial_avail, '^', path, 0, owned_keys, best_path_len, c);
    std::println("min path {}", best_path_len);

}


int main(const int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid args" << std::endl;
        return 1;
    }
    auto map = getInput(argv[1]);
    for(const auto& r: map){
     for(const auto& c: r){
           std::print("{}", c);
       }
        std::print("\n");
    }
    findAvailable(map);
}