
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
            std::print("{}", c);
        }
        std::println("");
    }
}

[[nodiscard]] P next_ball_loc(IntCode::Program prog, IntCode::dt ip, const P& paddle_pos)
{
    std::queue<IntCode::dt> tmp;
    P prev_pos{}, cur_pos{};
    while (!IntCode::halted(prog, ip)){

        if (IntCode::next_is_inp(prog, ip))
        {
            tmp.push(0);
            IntCode::run_till_io(prog, tmp, ip);
        }
        else if (IntCode::next_is_out(prog, ip))
        {

            const auto x = IntCode::run_till_io(prog, tmp, ip);
            const auto y = IntCode::run_till_io(prog, tmp, ip);
            const auto tid = IntCode::run_till_io(prog, tmp, ip);

            if (tid == 4 && x == paddle_pos.x && (y+1) == paddle_pos.y)
            {
                continue;
            }
            if (tid == 4)
            {
                prev_pos = cur_pos;
                cur_pos = {x,y};
                std::println("Found ball @ {} (prev = {})", cur_pos, prev_pos);
                // for (int i = 0; i<24; i++)
                // {
                //     for (int j = 0; j<37; j++)
                //     {
                //         std::print("{}", prog.program[639+i*37+j]);
                //     }
                //     std::println("");
                // }
            }
            if ((tid == 4) && (y == paddle_pos.y || (y+1) == paddle_pos.y || ((y+2) == paddle_pos.y && cur_pos.y == prev_pos.y && std::abs(cur_pos.x-prev_pos.x) == 2)) )
            {
                return {x,y};
            }
            if (tid == 4 && y > paddle_pos.y)
            {
                break;
            }
        }
    }
    return {0,0};
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
        P paddle_pos{}, target{}, ball_pos{}, prev_ball_pos{};
        int frame = 0;
        IntCode::run_till_io(prog, tmp, ip);
        while (!IntCode::halted(prog, ip)){
            if (IntCode::next_is_inp(prog, ip))
            {
                // auto next = next_ball_loc(prog, ip, paddle_pos);

                if (ball_pos.x == paddle_pos.x)
                {
                    tmp.push(0);
                    std::println("Not moving");
                } else if (ball_pos.x > paddle_pos.x)
                {
                    tmp.push(1);
                    paddle_pos.x += 1;
                    std::println("Moving right");
                }else
                {
                    tmp.push(-1);
                    paddle_pos.x -= 1;
                    std::println("Moving left");
                }
                std::println("{}, {}", paddle_pos, ball_pos);

                frame++;
                std::println("Frame {}", frame);
                display(map, score);
                std::println("=====================================");
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
                        // std::println("Prev pos: {}", paddle_pos);
                        prev_ball_pos = ball_pos;
                        ball_pos = {x,y};
                        // std::println("Cur pos: {}", paddle_pos);
                    }
                }
            }
        }
        std::println("{}", score);
    }
}