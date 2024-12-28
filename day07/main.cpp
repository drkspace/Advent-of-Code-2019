//
// Created by daniel on 12/27/24.
//

#include <iostream>
#include <ostream>

#include "intcode.h"
#include "../utils/utils.h"

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
            out.emplace_back(std::stoll(ele));
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
        const auto inp = getInput(argv[1]);


        IntCode::dt max_power = 0;
        std::vector<IntCode::dt> opts = {0,1,2,3,4};
        do
        {
            IntCode::dt last_pow = 0;
            for (const auto& ele: opts)
            {
                auto tmp_prog = inp;
                std::queue<IntCode::dt> tmp;
                tmp.push(ele);
                tmp.push(last_pow);

                last_pow = IntCode::run(tmp_prog, tmp);
            }
            if (last_pow > max_power)
            {
                max_power = last_pow;
                print_arr(std::span(opts));
            }
        } while (std::next_permutation(opts.begin(), opts.end()));


        std::println("Part 1: {}", max_power);
    }

    {
        const auto inp = getInput(argv[1]);


        IntCode::dt max_power = 0;
        std::array<IntCode::dt, 5> opts = {5,6,7,8,9};
        do
        {
            std::vector<IntCode::Program> progs(opts.size());
            std::vector<IntCode::dt> ips(opts.size());
            std::array<std::queue<IntCode::dt>, 5> outputs{};
            for (const auto& [i,ele]: enumerate(opts))
            {
                progs[i] = inp;
                ips[i] = 0;
                std::queue<IntCode::dt> tmp;
                tmp.push(ele);
                IntCode::run_till_io(progs[i], tmp, ips[i]);
            }
            {
                std::queue<IntCode::dt> tmp;
                tmp.push(0);
                IntCode::run_till_io(progs[0], tmp, ips[0]);
            }

            bool done = false;
            while (!done)
            {
                for (const auto&[i, prog, ip]: std::views::zip(std::views::iota(0), progs, ips))
                {
                    std::queue<IntCode::dt> tmp;
                    if (IntCode::next_is_inp(prog, ip))
                    {
                        auto prev_idx = pmod<IntCode::dt>(i-1, opts.size());
                        if (outputs[prev_idx].empty())
                        {
                            continue;
                        }
                        tmp.push(outputs[prev_idx].front());
                        outputs[prev_idx].pop();
                        IntCode::run_till_io(prog, tmp, ip);
                    }else if (IntCode::next_is_out(prog, ip)){
                        while (!IntCode::next_is_inp(prog, ip) && !IntCode::halted(prog, ip))
                        {
                            outputs[i].push(IntCode::run_till_io(prog, tmp, ip));
                        }
                    }else if (IntCode::halted(prog, ip) && i == 4)
                    {
                       done = true;
                    }
                }
            }

            if (outputs.back().front() > max_power)
            {
                max_power = outputs.back().front();
                print_arr(std::span(opts));
            }
            // break;
        } while (std::next_permutation(opts.begin(), opts.end()));


        std::println("Part 1: {}", max_power);
    }
}