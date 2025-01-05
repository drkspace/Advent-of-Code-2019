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

bool test(const P& p)
{
    const auto s = split(R"(#.......................................
.#......................................
..##....................................
...###..................................
....###.................................
.....####...............................
......#####.............................
......######............................
.......#######..........................
........########........................
.........#########......................
..........#########.....................
...........##########...................
...........############.................
............############................
.............#############..............
..............##############............
...............###############..........
................###############.........
................#################.......
.................##################.....
..................##################....
...................###################..
....................####################
.....................###################
.....................###################
......................##################
.......................#################
........................################
.........................###############
..........................##############
..........................##############
...........................#############
............................############
.............................###########)", '\n');
    if (p.y >= static_cast<decltype(p.y)>(s.size()))
    {
        return false;
    }
    if (p.x >= static_cast<decltype(p.y)>(s[p.y].size()))
    {
        return false;
    }
    return s[p.y][p.x] == '#';
}

auto getInput(const std::string& fp)
{
    auto lines = read_file(fp);
    IntCode::Program out;

    for (const auto& l : lines)
    {
        if (l.empty())
        {
            break;
        }

        for (const auto& ele : split(l, ','))
        {
            out.program.emplace_back(std::stoll(ele));
        }
    }
    return out;
}

int main(const int argc, char* argv[])
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

        std::unordered_set<P> tb;
        if (!IntCode::next_is_inp(prog, ip) || !IntCode::next_is_out(prog, ip) || !IntCode::halted(prog, ip))
        {
            IntCode::run_till_io(prog, tmp, ip);
        }

        /*
         * if A is hole or B is hole or C is hole or D is hole:
         *      if D is not hole: # D == True
         *          Jump
         */

        for (const auto& c : R"(NOT T T
AND A T
AND B T
AND C T
AND D T
NOT T T
AND D T
OR T J
WALK
)")
        {
            tmp.emplace(c);
        }
        while (!IntCode::halted(prog, ip))
        {
            if (IntCode::next_is_inp(prog, ip))
            {
                IntCode::run_till_io(prog, tmp, ip);
            }
            else if (IntCode::next_is_out(prog, ip))
            {
                const auto ov = IntCode::run_till_io(prog, tmp, ip);
                if (ov == static_cast<char>(ov))
                {
                    std::print("{}", static_cast<char>(ov));
                }
                else
                {
                    std::println("{}", ov);
                }
            }
        }
    }

    {
        auto prog = getInput(argv[1]);
        IntCode::InputQueue tmp;
        IntCode::dt ip = 0;

        std::unordered_set<P> tb;
        if (!IntCode::next_is_inp(prog, ip) || !IntCode::next_is_out(prog, ip) || !IntCode::halted(prog, ip))
        {
            IntCode::run_till_io(prog, tmp, ip);
        }


        /*
         * if A is hole or B is hole or C is hole:
         *      if D is not hole:
         *          if !(H is Hole and E is Hole) <==> H is not Hole or E is not Hole
         *              Jump
         *
         */

        for (const std::string l : {
                 "NOT T T", // Set T to True
                 "AND A T", // Get that A-D are not holes
                 "AND B T",
                 "AND C T",
                 "NOT T T", // Invert T so T will be true if any of A-D are holes
                 "AND D T", // D is not a hole
                 "OR T J",
                 "NOT H T", // set T to H
                 "NOT T T",
                 "OR E T",
                 "AND T J",
                 "RUN"})
        {
            for (const auto& c : l)
            {
                tmp.emplace(c);
            }
            tmp.emplace('\n');
        }
        while (!IntCode::halted(prog, ip))
        {
            if (IntCode::next_is_inp(prog, ip))
            {
                IntCode::run_till_io(prog, tmp, ip);
            }
            else if (IntCode::next_is_out(prog, ip))
            {
                const auto ov = IntCode::run_till_io(prog, tmp, ip);
                if (ov == static_cast<char>(ov))
                {
                    std::print("{}", static_cast<char>(ov));
                }
                else
                {
                    std::println("{}", ov);
                }
            }
        }
    }
}
