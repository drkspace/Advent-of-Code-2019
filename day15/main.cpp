
#include <chrono>
#include <iostream>
#include <map>
#include <numeric>
#include <ostream>
#include <set>
#include <stack>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "../utils/utils.h"
#include "../day09/intcode.h"

using P = Point2d<IntCode::dt>;

auto getInput(const std::string& fp){
    auto lines = read_file(fp);
    IntCode::Program out;

    for (const auto& l: lines)
    {
        if (l.empty())
        {
            break;
        }

        for (const auto& ele: split(l, ','))
        {
            out.program.emplace_back(std::stoll(ele));
        }
    }
    return out;

}

void display(const std::unordered_map<P, IntCode::dt>& map, const P& bot_pos)
{
    std::vector<std::vector<char>> out;
    std::string out_str;
    IntCode::dt minx = 10000000;
    IntCode::dt miny = 10000000;
    for (const auto& k: map | std::ranges::views::keys)
    {
        if (k.x < minx)
        {
            minx = k.x;
        }
        if (k.y < miny)
        {
            miny = k.y;
        }
    }
    for (const auto& [k,v]: map)
    {
        const auto tmp = k - P(minx, miny);
        if (out.size() <= static_cast<size_t>(tmp.y))
        {
            out.resize(tmp.y+1);
        }
        if (out[tmp.y].size() <= static_cast<size_t>(tmp.x))
        {
            out[tmp.y].resize(tmp.x+1, ' ');
        }
        switch (v)
        {
        case 0:
            out[tmp.y].at(tmp.x) = '#';
            break;
        case 2:
            out[tmp.y][tmp.x] = '@';
            break;
        case 1:
        default:
            out[tmp.y][tmp.x] = '.';
            break;
        }
        if (k == bot_pos)
        {
            out[tmp.y][tmp.x] = 'D';
        }
    }
    for (const auto& r: out)
    {
        for (const auto& c: r)
        {
            out_str += c;
        }
        out_str += '\n';
    }
    std::print("{}", out_str);
}
enum class Orient: IntCode::dt{
    NORTH = 1,
    SOUTH = 2,
    WEST = 3,
    EAST = 4,
};
[[nodiscard]] P otop(const Orient o)
{
    switch (o)
    {
    case Orient::EAST:
        return {1,0};
    case Orient::SOUTH:
        return {0,1};
    case Orient::WEST:
        return {-1,0};
    case Orient::NORTH:
        return {0,-1};
    }
    return {};
}

[[nodiscard]] Orient left(const Orient o)
{
    switch (o)
    {
    case Orient::EAST:
        return Orient::SOUTH;
    case Orient::SOUTH:
        return Orient::WEST;
    case Orient::WEST:
        return Orient::NORTH;
    case Orient::NORTH:
        return Orient::EAST;
    }
    return {};
}
[[nodiscard]] Orient right(const Orient o)
{
    switch (o)
    {
    case Orient::EAST:
        return Orient::NORTH;
    case Orient::SOUTH:
        return Orient::EAST;
    case Orient::WEST:
        return Orient::SOUTH;
    case Orient::NORTH:
        return Orient::WEST;
    }
    return {};
}

[[nodiscard]] Orient best_orient(const std::unordered_map<P, IntCode::dt>& map, const P& bot_pos, const Orient& prev_dir)
{
    if (!map.contains(bot_pos+otop(Orient::EAST)))
    {
        return  Orient::EAST;
    }
    if (!map.contains(bot_pos+otop(Orient::WEST)))
    {
        return Orient::WEST;
    }
    if (!map.contains(bot_pos+otop(Orient::SOUTH)))
    {
        return Orient::SOUTH;
    }
    if (!map.contains(bot_pos+otop(Orient::NORTH)))
    {
        return Orient::NORTH;
    }
    if (map.find(bot_pos+otop(Orient::EAST))->second == 1 && prev_dir != Orient::WEST)
    {
        return  Orient::EAST;
    }
    if (map.find(bot_pos+otop(Orient::WEST))->second == 1 && prev_dir != Orient::EAST)
    {
        return Orient::WEST;
    }
    if (map.find(bot_pos+otop(Orient::SOUTH))->second == 1 && prev_dir != Orient::NORTH)
    {
        return Orient::SOUTH;
    }
    if (map.find(bot_pos+otop(Orient::NORTH))->second == 1 && prev_dir != Orient::SOUTH)
    {
        return Orient::NORTH;
    }

    // Find the nearest unknown space

    return Orient::NORTH;
}

std::optional<std::vector<Orient>> navigate_to_helper(const std::unordered_map<P, IntCode::dt>& map, const P& bot_pos, const P& target_pos, std::unordered_map<P, int>& visited, const int depth)
{
    std::vector<Orient> out;

    if (bot_pos == target_pos)
    {
        return out;
    }
    if (const auto f = visited.find(bot_pos); f!= visited.end() && f->second < depth)
    {
        return std::nullopt;
    }
    visited[bot_pos] = depth;
    if (const auto f = map.find(bot_pos+P(0,1)); f!=map.end() && f->second == 1)
    {
        auto tmp = navigate_to_helper(map, bot_pos+P(0,1), target_pos, visited, depth+1);
        if (tmp.has_value())
        {
            out.emplace_back(Orient::SOUTH);
            out.insert(out.end(), tmp.value().begin(), tmp.value().end());
            return out;
        }
    }
    if (const auto f = map.find(bot_pos+P(0,-1)); f!=map.end() && f->second == 1)
    {
        auto tmp = navigate_to_helper(map, bot_pos+P(0,-1), target_pos, visited, depth+1);
        if (tmp.has_value())
        {
            out.emplace_back(Orient::NORTH);
            out.insert(out.end(), tmp.value().begin(), tmp.value().end());
            return out;
        }
    }
    if (const auto f = map.find(bot_pos+P(1,0)); f!=map.end() && f->second == 1)
    {
        auto tmp = navigate_to_helper(map, bot_pos+P(1,0), target_pos, visited, depth+1);
        if (tmp.has_value())
        {
            out.emplace_back(Orient::EAST);
            out.insert(out.end(), tmp.value().begin(), tmp.value().end());
            return out;
        }
    }
    if (const auto f = map.find(bot_pos+P(-1,0)); f!=map.end() && f->second == 1)
    {
        auto tmp = navigate_to_helper(map, bot_pos+P(-1,0), target_pos, visited, depth+1);
        if (tmp.has_value())
        {
            out.emplace_back(Orient::WEST);
            out.insert(out.end(), tmp.value().begin(), tmp.value().end());
            return out;
        }
    }

    return std::nullopt;
}

void navigate_to(const std::unordered_map<P, IntCode::dt>& map, P& bot_pos, const P& target_pos, IntCode::Program& prog, IntCode::dt& ip)
{
    std::unordered_map<P, int> visited;
    auto res = navigate_to_helper(map, bot_pos, target_pos, visited, 0);
    if (!res.has_value())
    {
        std::println("AHHH");
        return;
    }
    auto path = res.value();
    IntCode::InputQueue tmp;
    for (const auto& o: path)
    {
        tmp.push(static_cast<IntCode::dt>(o));
        IntCode::run_till_io(prog, tmp, ip);
        IntCode::run_till_io(prog, tmp, ip);
    }

    bot_pos = target_pos;
}

bool best_path_helper(const std::unordered_map<P, IntCode::dt>& map, const P& bot_pos, const P& target_pos, std::unordered_map<P, int>& visited, const int depth)
{


    if (const auto f = visited.find(bot_pos); f!= visited.end() && f->second < depth)
    {
        return false;
    }
    visited[bot_pos] = depth;
    if (bot_pos == target_pos)
    {
        return true;
    }
    if (const auto f = map.find(bot_pos+P(0,1)); f!=map.end() && f->second >= 1)
    {
        auto tmp = best_path_helper(map, bot_pos+P(0,1), target_pos, visited, depth+1);
        if (tmp)
        {
            return true;
        }
    }
    if (const auto f = map.find(bot_pos+P(0,-1)); f!=map.end() && f->second >= 1)
    {
        auto tmp = best_path_helper(map, bot_pos+P(0,-1), target_pos, visited, depth+1);
        if (tmp)
        {
            return true;
        }
    }
    if (const auto f = map.find(bot_pos+P(1,0)); f!=map.end() && f->second >= 1)
    {
        auto tmp = best_path_helper(map, bot_pos+P(1,0), target_pos, visited, depth+1);
        if (tmp)
        {
            return true;
        }
    }
    if (const auto f = map.find(bot_pos+P(-1,0)); f!=map.end() && f->second >= 1)
    {
        auto tmp = best_path_helper(map, bot_pos+P(-1,0), target_pos, visited, depth+1);
        if (tmp)
        {
            return true;
        }
    }

    return false;
}

int find_best_path(const std::unordered_map<P, IntCode::dt>& map, const P& bot_pos)
{
    P target_pos{};
    for (const auto&[p,v]: map)
    {
        if (v==2)
        {
            target_pos = p;
            break;
        }
    }
    std::unordered_map<P, int> visited;
    auto res = best_path_helper(map, bot_pos, target_pos, visited, 0);
    if (!res)
    {
        std::println("AHHH");
        return 0;
    }

    return visited[target_pos];
}

int find_fill_time(const std::unordered_map<P, IntCode::dt>& map)
{
    P target_pos{};
    for (const auto&[p,v]: map)
    {
        if (v==2)
        {
            target_pos = p;
            break;
        }
    }
    std::unordered_map<P, int> visited;
    best_path_helper(map, target_pos, {-1000000,-1000000}, visited, 0);

    int max = 0;
    for (const auto& v: visited | std::ranges::views::values)
    {
        if (v > max)
        {
            max = v;
        }
    }
    return max;
}

[[nodiscard]] bool know_around(const std::unordered_map<P, IntCode::dt>& map, P& p)
{
    return map.contains(p) && map.contains(p+P(0,1)) && map.contains(p+P(0,-1)) && map.contains(p+P(1,0)) && map.contains(p+P(-1,0));
}

int main(const int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid args" << std::endl;
        return 1;
    }

    {
        auto prog = getInput(argv[1]);
        std::queue<IntCode::dt> tmp;
        IntCode::dt ip = 0;

        P bot_pos(0,0);
        auto o = Orient::NORTH;
        std::unordered_map<P, IntCode::dt> map;
        // std::optional<P> target = std::nullopt;
        std::stack<P> prev;
        prev.push(bot_pos);
        int i = 0;
        // int count = 0;
        // IntCode::run_till_io(prog, tmp, ip);
        while (!IntCode::halted(prog, ip) && i<100){

            tmp.push(static_cast<IntCode::dt>(o));
            IntCode::run_till_io(prog, tmp, ip);
            const auto res = IntCode::run_till_io(prog, tmp, ip);
            i++;
            if (!map.contains(bot_pos+otop(o)))
            {
                i = 0;
            }
            map[bot_pos+otop(o)] = res;
            switch (res)
            {
                //Hit a wall
                case 0:
                       if (!prev.empty())
                       {
                            P tmp2 = prev.top();
                           while (know_around(map, tmp2) && prev.size() >= 2)
                           {
                               prev.pop();
                               tmp2 = prev.top();
                           }
                            std::println("Trying to go back to {}", tmp2);
                            navigate_to(map, bot_pos, tmp2, prog, ip);

                        }
                        o = best_orient(map, bot_pos, o);
                    break;

                //Moved
                case 1:
                    bot_pos += otop(o);
                    o = best_orient(map, bot_pos, o);
                    if (!know_around(map, bot_pos))
                    {
                        prev.push(bot_pos);
                    }
                    break;

                // Moved and Found the system
                case 2:
                    bot_pos += otop(o);
                    std::println("{}", bot_pos);
                    break;

                default:
                    std::println("PANIC");
                    break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000/75));
            std::print("\033[2J\033[H");
            display(map, bot_pos);

        }

        auto best = find_best_path(map, {0,0});
        std::println("Part 1: {}", best);
        auto fill_time = find_fill_time(map);
        std::println("Part 2: {}", fill_time);
    }

}
