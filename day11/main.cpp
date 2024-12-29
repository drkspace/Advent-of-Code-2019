//
// Created by daniel on 12/27/24.
//

#include <iostream>
#include <ostream>
#include <unordered_set>

#include "../day09/intcode.h"
#include "../utils/utils.h"

using P = Point2d<int>;
using cmap = std::unordered_set<P>;

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

int getColor(const cmap& white, const P& pos)
{
    if (white.contains(pos))
    {
        return 1;
    }
    return 0;
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
        tmp.push(0);
        IntCode::dt ip = 0;

        P cur_pos = {0,0};
        auto dir = Dir::UP;
        cmap black;
        cmap white;

        IntCode::run_till_io(prog, tmp, ip);
        while (!IntCode::halted(prog, ip)){
            if (IntCode::next_is_inp(prog, ip))
            {
                tmp.push(getColor(white, cur_pos));
                IntCode::run_till_io(prog, tmp, ip);
            }
            else if (IntCode::next_is_out(prog, ip))
            {
                // Run twice to get the color and dir
                const auto c = IntCode::run_till_io(prog, tmp, ip);
                const auto d = IntCode::run_till_io(prog, tmp, ip);
                if (c == 0)
                {
                    if (auto f = white.find(cur_pos); f != white.end())
                    {
                        white.erase(f);
                    }
                    black.emplace(cur_pos);
                } else
                {
                    if (auto f = black.find(cur_pos); f != black.end())
                    {
                        black.erase(f);
                    }
                    white.emplace(cur_pos);
                }

                if (d == 0)
                {
                    switch (dir)
                    {
                        case Dir::UP:
                            dir = Dir::LEFT;
                            cur_pos += {-1,0};
                            break;
                        case Dir::LEFT:
                            dir = Dir::DOWN;
                            cur_pos += {0,1};
                            break;
                        case Dir::DOWN:
                            dir = Dir::RIGHT;
                            cur_pos += {1,0};
                            break;
                        case Dir::RIGHT:
                            dir = Dir::UP;
                            cur_pos += {0,-1};
                            break;
                    }
                }
                else
                {
                    switch (dir)
                    {
                    case Dir::UP:
                        dir = Dir::RIGHT;
                        cur_pos += {1,0};
                        break;
                    case Dir::LEFT:
                        dir = Dir::UP;
                        cur_pos += {0,-1};
                        break;
                    case Dir::DOWN:
                        dir = Dir::LEFT;
                        cur_pos += {-1,0};
                        break;
                    case Dir::RIGHT:
                        dir = Dir::DOWN;
                        cur_pos += {0,1};
                        break;
                    }
                }
            }
        }

        std::println("Part 1: {}+{} = {}", black.size(),white.size(), black.size()+white.size());
    }

    {
        auto prog = getInput(argv[1]);
        std::queue<IntCode::dt> tmp;
        tmp.push(1);
        IntCode::dt ip = 0;

        P cur_pos = {0,0};
        auto dir = Dir::UP;
        cmap black;
        cmap white;

        IntCode::run_till_io(prog, tmp, ip);
        while (!IntCode::halted(prog, ip)){
            if (IntCode::next_is_inp(prog, ip))
            {
                tmp.push(getColor(white, cur_pos));
                IntCode::run_till_io(prog, tmp, ip);
            }
            else if (IntCode::next_is_out(prog, ip))
            {
                // Run twice to get the color and dir
                const auto c = IntCode::run_till_io(prog, tmp, ip);
                const auto d = IntCode::run_till_io(prog, tmp, ip);
                if (c == 0)
                {
                    if (auto f = white.find(cur_pos); f != white.end())
                    {
                        white.erase(f);
                    }
                    black.emplace(cur_pos);
                } else
                {
                    if (auto f = black.find(cur_pos); f != black.end())
                    {
                        black.erase(f);
                    }
                    white.emplace(cur_pos);
                }

                if (d == 0)
                {
                    switch (dir)
                    {
                        case Dir::UP:
                            dir = Dir::LEFT;
                            cur_pos += {-1,0};
                            break;
                        case Dir::LEFT:
                            dir = Dir::DOWN;
                            cur_pos += {0,1};
                            break;
                        case Dir::DOWN:
                            dir = Dir::RIGHT;
                            cur_pos += {1,0};
                            break;
                        case Dir::RIGHT:
                            dir = Dir::UP;
                            cur_pos += {0,-1};
                            break;
                    }
                }
                else
                {
                    switch (dir)
                    {
                    case Dir::UP:
                        dir = Dir::RIGHT;
                        cur_pos += {1,0};
                        break;
                    case Dir::LEFT:
                        dir = Dir::UP;
                        cur_pos += {0,-1};
                        break;
                    case Dir::DOWN:
                        dir = Dir::LEFT;
                        cur_pos += {-1,0};
                        break;
                    case Dir::RIGHT:
                        dir = Dir::DOWN;
                        cur_pos += {0,1};
                        break;
                    }
                }
            }
        }

        int minx, miny;
        minx = std::ranges::min(white, [](const P& a, const P& b)
        {
            return a.x < b.x;
        }).x;
        miny = std::ranges::min(white, [](const P& a, const P& b)
        {
            return a.y < b.y;
        }).y;

        std::vector<std::vector<bool>> isWhite;
        for (const auto& ele: white)
        {
            if (isWhite.size() <= static_cast<size_t>(ele.y-miny))
            {
                isWhite.resize(ele.y-miny+1);
            }
            if (isWhite[ele.y-miny].size() <= static_cast<size_t>(ele.x-minx))
            {
                isWhite[ele.y-miny].resize(ele.x-minx+1);
            }
            isWhite.at(ele.y-miny).at(ele.x-minx) = true;
        }

        std::println("Part 2:");
        for (const auto& r: isWhite)
        {
            for (const auto& v: r)
            {
                if (v)
                {
                    std::print("█");
                }
                else
                {
                    std::print(" ");
                }
            }
            std::println("");
        }
    }

}