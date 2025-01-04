
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
        IntCode::run_till_io(prog, tmp, ip);
        constexpr IntCode::dt N = 50;
        for (IntCode::dt i = 0; i< N; i++){
            for (IntCode::dt j = 0; j< N; j++)
            {
                auto prog_cpy = prog;
                auto ip_cpy = ip;
                tmp.push(j);
                tmp.push(i);
                IntCode::run_till_io(prog_cpy, tmp, ip_cpy);
                IntCode::run_till_io(prog_cpy, tmp, ip_cpy);
                if (IntCode::run_till_io(prog_cpy, tmp, ip_cpy) == 1)
                {
                    tb.emplace(j,i);
                }
            }

        }

        for (IntCode::dt i = 0; i< N; i++){
            std::print("{:02d}: ", i);
            for (IntCode::dt j = 0; j< N; j++)
            {
                if (tb.contains({i,j}))
                {
                    std::print("#");
                }
                else
                {
                    std::print(".");
                }
            }
            std::println("");

        }
        std::println("Part 1: {}", tb.size());
    }

    {
        auto prog = getInput(argv[1]);
        IntCode::InputQueue tmp;
        IntCode::dt ip = 0;

        std::unordered_set<P> tb;
        IntCode::run_till_io(prog, tmp, ip);
        constexpr IntCode::dt SHIP_SIZE = 100;
        IntCode::dt prev_start = 0;
        IntCode::dt res = 0;
        std::queue<P> test_points;
        IntCode::dt i = 0;
        while (true){
            if (i%100 == 0)
            {
                std::println("{:08d}", i);
            }
            int cur_run = 0;
            bool last_was_tb = false;
            for (IntCode::dt j = std::max<IntCode::dt>(0,prev_start-50); j< i*2; j++)
            {
                auto prog_cpy = prog;
                auto ip_cpy = ip;
                tmp.push(j);
                tmp.push(i);
                IntCode::run_till_io(prog_cpy, tmp, ip_cpy);
                IntCode::run_till_io(prog_cpy, tmp, ip_cpy);
                if (IntCode::run_till_io(prog_cpy, tmp, ip_cpy) == 1)
                // if (test({j,i}))
                {
                    tb.emplace(j,i);
                    cur_run++;
                    if (last_was_tb)
                    {
                        if (cur_run == SHIP_SIZE)
                        {
                            test_points.emplace(j-SHIP_SIZE+1, i);
                        }
                    }else
                    {
                        prev_start = j;
                    }
                    last_was_tb = true;
                }
                else
                {
                    // Assuming the beam is solid across a row
                    if (last_was_tb)
                    {
                        break;
                    }
                }
            }
            if (!test_points.empty())
            {
                // Only need to test candidate rows once they've gone past SHIP_SIZE rows
                if (auto p = test_points.front(); p.y+SHIP_SIZE-1 == i)
                {
                    test_points.pop();

                    // Assuming the beam is solid
                    // Only need to test the corners

                    bool found = false;
                    while (tb.contains(p))
                    {
                        //The TL corner will always be in
                        //If the TR corner is not in, then we can break out

                        if (!tb.contains(p+P(SHIP_SIZE-1,0)))
                        {
                            break;
                        }
                        auto bl = tb.contains(p+P(0, SHIP_SIZE-1));
                        auto br = tb.contains(p+P(SHIP_SIZE-1, SHIP_SIZE-1));
                        br = true;
                        if ( bl && br )
                        {
                            bool inv = false;
                            for (IntCode::dt ii = 0; ii < SHIP_SIZE; ii++)
                            {

                                for (IntCode::dt jj = 0; jj < SHIP_SIZE; jj++)
                                {
                                    if (!tb.contains(p+P(ii,jj)))
                                    {
                                        inv = true;
                                        break;
                                    }
                                }
                                if (inv)
                                {
                                    break;
                                }
                            }
                            if (!inv)
                            {
                                found = true;
                                break;
                            }
                        }
                        p += {1,0};
                    }

                    if (found)
                    {
                        std::println("{}", p);
                        // for (IntCode::dt _i = p.y-30; _i< (p.y+SHIP_SIZE+2); _i++){
                        //     std::print("{:02d}: ", _i);
                        //     for (IntCode::dt _j = p.x-30; _j< (p.x+SHIP_SIZE+10); _j++)
                        //     {
                        //         P tp(_j, _i);
                        //         if (tb.contains(tp))
                        //         {
                        //             if (tp == p || p+P(SHIP_SIZE-1, 0) == tp || p+P(SHIP_SIZE-1, SHIP_SIZE-1) == tp || p+P(0, SHIP_SIZE-1) == tp)
                        //             {
                        //                 std::print("\e[0;31m#\e[0m");
                        //             }else
                        //             {
                        //                 std::print("#");
                        //             }
                        //
                        //         }
                        //         else
                        //         {
                        //             std::print(".");
                        //         }
                        //     }
                        //     std::println("");

                        // }
                        res = p.x*10000+p.y;
                        // std::println("{}", res);
                        break;
                    }
                }
            }
            i++;
        }

        // gt 7691361
        // neq 7731369
        // neq 7641353
        // new 7691362
        std::println("Part 2: {}", res);
    }


}
