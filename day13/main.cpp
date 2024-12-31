
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

void display(const std::unordered_map<P, IntCode::dt>& map, const IntCode::dt score)
{
    std::println("Score: {}", score);
    std::array<std::array<char, 37>, 24> out{};
    std::string out_str;
    for (const auto& [k,v]: map)
    {
        switch (v)
        {
        case 1:
            out[k.y][k.x] = '+';
            break;
        case 2:
            out[k.y][k.x] = '#';
            break;
        case 3:
            out[k.y][k.x] = '-';
            break;
        case 4:
            out[k.y][k.x] = 'o';
            break;
        case 0:
        default:
            out[k.y][k.x] = ' ';
            break;
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

        std::unordered_map<P, IntCode::dt> map;

        IntCode::run_till_io(prog, tmp, ip);
        while (!IntCode::halted(prog, ip)){
            if (IntCode::next_is_inp(prog, ip))
            {
                IntCode::run_till_io(prog, tmp, ip);
            }
            else if (IntCode::next_is_out(prog, ip))
            {
                // Run twice to get the color and dir
                const auto x = IntCode::run_till_io(prog, tmp, ip);
                const auto y = IntCode::run_till_io(prog, tmp, ip);
                const auto tid = IntCode::run_till_io(prog, tmp, ip);
                map[{x,y}] = tid;
            }
        }
        int n_blocks =0;
        for (const auto& v: map | std::ranges::views::values)
        {
            n_blocks += v == 2;
        }
        std::println("{}", n_blocks);
    }

    {
        auto prog = getInput(argv[1]);
        prog.set(0, 2);
        std::queue<IntCode::dt> tmp;
        IntCode::dt ip = 0;

        std::unordered_map<P, IntCode::dt> map;
        IntCode::dt score = 0;
        P paddle_pos{}, ball_pos{};
        IntCode::run_till_io(prog, tmp, ip);
        while (!IntCode::halted(prog, ip)){
            if (IntCode::next_is_inp(prog, ip))
            {

                if (ball_pos.x == paddle_pos.x)
                {
                    tmp.push(0);
                } else if (ball_pos.x > paddle_pos.x)
                {
                    tmp.push(1);
                    paddle_pos.x += 1;
                }else
                {
                    tmp.push(-1);
                    paddle_pos.x -= 1;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000/75));
                std::print("\033[2J\033[H");
                display(map, score);
                IntCode::run_till_io(prog, tmp, ip);

            }
            else if (IntCode::next_is_out(prog, ip))
            {
                // Run twice to get the color and dir
                const auto x = IntCode::run_till_io(prog, tmp, ip);
                const auto y = IntCode::run_till_io(prog, tmp, ip);
                const auto tid = IntCode::run_till_io(prog, tmp, ip);
                if (x == -1 && y == 0)
                {
                    score = tid;
                }else
                {
                    map[{x,y}] = tid;
                    if (tid == 3)
                    {
                        paddle_pos = {x,y};
                    }
                    if (tid == 4)
                    {
                        ball_pos = {x,y};
                    }
                }
            }
        }
        std::println("Final Score (Part 2): {}", score);
    }
}