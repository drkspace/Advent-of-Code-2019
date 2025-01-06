
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

bool test(const P &p)
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

        std::unordered_set<P> tb;
        if (!IntCode::next_is_inp(prog, ip) && !IntCode::next_is_out(prog, ip) && !IntCode::halted(prog, ip))
        {
            IntCode::run_till_io(prog, tmp, ip);
        }
        while (!IntCode::halted(prog, ip))
        {
            if (IntCode::next_is_inp(prog, ip))
            {
                std::string l;
                std::getline(std::cin, l);
                l += '\n';
                for (const auto& c: l)
                {
                    tmp.emplace(c);
                    IntCode::run_till_io(prog, tmp, ip);
                }

            }else
            {
                auto c = IntCode::run_till_io(prog, tmp, ip);
                if (c == static_cast<char>(c))
                {
                    std::print("{}", static_cast<char>(c));
                }
            }
        }

    }


}
