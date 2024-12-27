//
// Created by daniel on 12/26/24.
//

#include <iostream>
#include <vector>
#include <string>
#include "../utils/utils.h"


namespace IntCode
{
    using dt = uintmax_t;
    using Program = std::vector<dt>;
    enum C
    {
        ERR,HALT,ADD,MULT
    };

    [[nodiscard]] C intoc(const dt& i)
    {
        switch (i){
        case 1:
            return ADD;
        case 2:
            return MULT;
        case 99:
            return HALT;
        default:
            return ERR;
        }
    };

    dt _run_op(Program& prog, const C& op, const dt& idx1, const dt& idx2, const dt& idx_out)
    {
        dt res;
        switch (op)
        {
        case C::ADD:
            res = prog[idx1]+prog[idx2];
            prog[idx_out] = res;
            return res;
        case C::MULT:
            res = prog[idx1]*prog[idx2];
            prog[idx_out] = res;
            return res;
        case C::ERR:
        case C::HALT:
        default:
            return 0;
        }
    };

    void run(Program& prog)
    {
        size_t ip = 0;
        auto op = intoc(prog[ip]);
        while (op != C::HALT)
        {
            auto idx1 = prog[ip+1];
            auto idx2 = prog[ip+2];
            auto idx_out = prog[ip+3];
            _run_op(prog, op, idx1, idx2, idx_out);
            ip += 4;
            op = intoc(prog.at(ip));
        }
    };
};

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
            out.emplace_back(std::stoull(ele));
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
        auto inp = getInput(argv[1]);

        IntCode::run(inp);
        std::println("Part 1: {}", inp[0]);
    }
    {
        const auto inp = getInput(argv[1]);
        bool found = false;
        for (IntCode::dt noun = 0; noun<=99; noun++)
        {
            for (IntCode::dt verb = 0; verb<=99; verb++)
            {
                auto tmp_inp = inp;
                tmp_inp[1] = noun;
                tmp_inp[2] = verb;
                IntCode::run(tmp_inp);
                if (tmp_inp[0] == 19690720)
                {
                    std::println("{}", 100*noun + verb);
                    found = true;
                    break;
                }
            }
            if (found)
            {
                break;
            }
        }


    }
}