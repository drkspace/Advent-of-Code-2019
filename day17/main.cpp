
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

// void find_all_paths_helper(const std::vector<std::vector<char>>& map, std::unordered_set<std::string>& paths, const std::string& cur_path_s, std::unordered_set<P> visited, P p, const P& target, const Dir d)
// {
//     if (p == target)
//     {
//         paths.emplace(cur_path_s);
//     }
//     visited.emplace(p);
//
//     P next;
//     P side;
//     switch (d)
//     {
//     case Dir::UP:
//         next = {0,-1};
//         side = {1,0};
//         break;
//     case Dir::DOWN:
//         next = {0,1};
//         side = {1,0};
//         break;
//     case Dir::LEFT:
//         next = {-1,0};
//         side = {0,1};
//         break;
//     case Dir::RIGHT:
//         next = {1,0};
//         side = {0,1};
//         break;
//     }
//     auto next_pos = p+next;
//     auto s1 = next_pos+side;
//     auto s2 = next_pos-side;
//     while (map[next_pos.y][next_pos.x] == '#' && map[s1.y][s1.x] != '#' && map[s2.y][s2.x] != '#')
//     {
//         p = next_pos;
//         next_pos = p + next;
//         s1 = next_pos+side;
//         s2 = next_pos-side;
//     }
// }
//
// std::unordered_set<std::string> find_all_paths(const std::vector<std::vector<char>>& map)
// {
//     std::unordered_set<std::string> paths;
//     std::unordered_set<P> inter, corners;
//     for (const auto& [i, r]: enumerate(map))
//     {
//         for (const auto& [j,c]: enumerate(r))
//         {
//             std::print("{}", c);
//             if (c == '#')
//             {
//                 if (!(i == 0 || i == static_cast<long>(map.size()-1) || j == 0 || j == static_cast<long>(map[0].size()-1)) &&
//                     map[i+1][j] == '#' && map[i-1][j] == '#' && map[i][j+1] == '#' && map[i][j-1] == '#')
//                 {
//                     inter.emplace(j,i);
//                 }
//                 // I'm going to ignore T intersections
//                 // TL corner
//                 else if (i != static_cast<long>(map.size()-1) && )
//                 {
//                     corners.emplace(j,i);
//                 }
//             }
//         }
//         std::println("");
//     }
//     return  paths;
// }

int main(const int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid args" << std::endl;
        return 1;
    }


    {
        auto prog = getInput(argv[1]);
        IntCode::InputQueue tmp;
        IntCode::dt ip = 0;

        P bot_pos(0,0);

        std::vector<std::vector<char>> map(1);
        while (!IntCode::halted(prog, ip))
        {
            char c = IntCode::run_till_io(prog, tmp, ip);

            if (c == '\0')
            {
                continue;
            }
            if (c == '\n')
            {
                if (!map.back().empty())
                {
                    map.emplace_back();
                }
            }
            else
            {
                map.back().emplace_back(c);
            }
        }
        if (map.back().empty())
        {
            map.resize(map.size()-1);
        }
        std::vector<P> inter;
        for (const auto& [i, r]: enumerate(map))
        {
            for (const auto& [j,c]: enumerate(r))
            {
                std::print("{}", c);
                if (c == '#')
                {
                    if (i == 0 || i == static_cast<long>(map.size()-1) || j == 0 || j == static_cast<long>(map[0].size()-1))
                    {
                        continue;
                    }
                    if (map[i+1][j] == '#' && map[i-1][j] == '#' && map[i][j+1] == '#' && map[i][j-1] == '#')
                    {
                        inter.emplace_back(j,i);
                    }
                }
            }
            std::println("");
        }
        long sum = 0;
        for (const auto& p: inter)
        {
            sum += p.x*p.y;
        }
        std::println("Part 1: {}", sum);
    }

    {
        IntCode::InputQueue seq;

        for (const auto& ele: std::string("B,C,B,C,A,A,C,B,C,A"))
        {
            std::println("Adding {}", static_cast<IntCode::dt>(ele));
            seq.push(ele);
        }
        seq.push('\n');

        // A function
        for (const auto& ele: std::string("R,12,L,12,R,6"))
        {
            seq.push(ele);
        }
        seq.push('\n');

        //B function
        for (const auto& ele: std::string("L,10,R,10,L,10,L,10"))
        {
            seq.push(ele);
        }
        seq.push('\n');

        //C function
        for (const auto& ele: std::string("R,10,R,12,L,12"))
        {
            seq.push(ele);
        }
        seq.push('\n');
        seq.push('n');
        seq.push('\n');


        auto prog = getInput(argv[1]);
        prog.set(0, 2);

        IntCode::dt ip = 0;

        IntCode::dt out;
        std::vector<std::vector<char>> map(1);
        while (!IntCode::halted(prog, ip))
        {
            if (IntCode::next_is_inp(prog, ip))
            {
                std::println("Getting input. {} in queue. Next is {}", seq.size(), seq.front());
                IntCode::run_till_io(prog, seq, ip);
            }
            else
            {
                IntCode::dt c = IntCode::run_till_io(prog, seq, ip);
                if (static_cast<char>(c) == c)
                {
                    if (c == '\0')
                    {
                        continue;
                    }
                    if (c == '\n')
                    {
                        if (!map.back().empty())
                        {
                            map.emplace_back();
                        }
                    }
                    else
                    {
                        map.back().emplace_back(c);
                    }
                }
                else
                {
                    out = c;
                }
            }

        }

        if (map.back().empty())
        {
            map.resize(map.size()-1);
        }
        for (const auto& [i, r]: enumerate(map))
        {
            for (const auto& [j,c]: enumerate(r))
            {
                std::print("{}", c);
            }
            std::println("");
        }

        std::println("{}", out);
    }
}
