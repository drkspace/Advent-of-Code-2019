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
        constexpr size_t N_COMP = 50;
        auto main_prog = getInput(argv[1]);
        std::array<IntCode::Program, N_COMP> computers;
        std::array<IntCode::dt, N_COMP> ips{};
        std::array<IntCode::InputQueue, N_COMP> inputs;

        for (size_t i = 0; i<N_COMP; i++)
        {
            computers[i] = main_prog;
            ips[i] = 0;
            if (!IntCode::next_is_inp(computers[i], ips[i]) && !IntCode::next_is_out(computers[i], ips[i]) && !IntCode::halted(computers[i], ips[i]))
            {
                IntCode::run_till_io(computers[i], inputs[i], ips[i]);
            }
            inputs[i].emplace(i);
            // Should ask for network address first
            IntCode::run_till_io(computers[i], inputs[i], ips[i]);
        }

        bool all_halted = false;
        while (!all_halted)
        {
            all_halted = true;
            for (size_t i = 0; i<N_COMP; i++)
            {
                auto& comp = computers[i];
                auto& ip = ips[i];
                auto& input = inputs[i];
                if (!IntCode::halted(comp, ip))
                {
                    all_halted = false;
                    if (IntCode::next_is_inp(comp, ip))
                    {
                        if (input.empty())
                        {
                            input.emplace(-1);
                            IntCode::run_till_io(comp, input, ip);
                        }else
                        {
                            IntCode::run_till_io(comp, input, ip);
                            IntCode::run_till_io(comp, input, ip);
                        }
                    }else if (IntCode::next_is_out(comp, ip))
                    {
                        const auto dest = IntCode::run_till_io(comp, input, ip);
                        const auto X = IntCode::run_till_io(comp, input, ip);
                        const auto Y = IntCode::run_till_io(comp, input, ip);

                        if (dest == 255)
                        {
                            all_halted = true;
                            std::println("Part 1: {}", Y);
                            break;
                        }
                        inputs[dest].emplace(X);
                        inputs[dest].emplace(Y);
                    }
                }
            }
        }

    }

    {
        constexpr size_t N_COMP = 50;
        auto main_prog = getInput(argv[1]);
        std::array<IntCode::Program, N_COMP> computers;
        std::array<IntCode::dt, N_COMP> ips{};
        std::array<IntCode::InputQueue, N_COMP> inputs;
        IntCode::dt nat_x{}, nat_y{};
        std::deque<IntCode::dt> prev_y_sent;

        for (size_t i = 0; i<N_COMP; i++)
        {
            computers[i] = main_prog;
            ips[i] = 0;
            if (!IntCode::next_is_inp(computers[i], ips[i]) && !IntCode::next_is_out(computers[i], ips[i]) && !IntCode::halted(computers[i], ips[i]))
            {
                IntCode::run_till_io(computers[i], inputs[i], ips[i]);
            }
            inputs[i].emplace(i);
            // Should ask for network address first
            IntCode::run_till_io(computers[i], inputs[i], ips[i]);
        }

        bool all_halted = false;
        while (!all_halted)
        {
            all_halted = true;
            bool all_idle = true;
            for (size_t i = 0; i<N_COMP; i++)
            {
                auto& comp = computers[i];
                auto& ip = ips[i];
                auto& input = inputs[i];
                bool idle = false;
                if (!IntCode::halted(comp, ip))
                {
                    all_halted = false;
                    if (IntCode::next_is_inp(comp, ip))
                    {
                        if (input.empty())
                        {
                            idle = true;
                            input.emplace(-1);
                            IntCode::run_till_io(comp, input, ip);
                        }else
                        {
                            IntCode::run_till_io(comp, input, ip);
                            IntCode::run_till_io(comp, input, ip);
                        }
                    }else if (IntCode::next_is_out(comp, ip))
                    {
                        const auto dest = IntCode::run_till_io(comp, input, ip);
                        const auto X = IntCode::run_till_io(comp, input, ip);
                        const auto Y = IntCode::run_till_io(comp, input, ip);

                        if (dest == 255)
                        {
                            nat_x = X;
                            nat_y = Y;
                        }else
                        {
                            inputs[dest].emplace(X);
                            inputs[dest].emplace(Y);
                        }
                    }
                }
                all_idle = all_idle && idle;
            }
            if (all_idle)
            {
                inputs[0].emplace(nat_x);
                inputs[0].emplace(nat_y);
                if (prev_y_sent.size() == 2)
                {
                    prev_y_sent.pop_front();
                }
                prev_y_sent.push_back(nat_y);
                if (prev_y_sent.size() == 2 && prev_y_sent.front() == prev_y_sent.back())
                {
                    all_halted = true;
                    std::println("Part 2: {}", prev_y_sent.front());
                }
            }
        }

    }

}
