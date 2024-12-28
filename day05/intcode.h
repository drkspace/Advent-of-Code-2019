//
// Created by daniel on 12/27/24.
//

#ifndef INTCODE_H
#define INTCODE_H
#include <cstdint>
#include <map>
#include <vector>

#include "../utils/utils.h"

namespace IntCode
{
    using dt = intmax_t;
    using Program = std::vector<dt>;
    enum C
    {
        HALT,ADD,MULT,INP,OUT,JNZ,JZ,LT,EQ,
        ERR // KEEP LAST
    };
    constexpr size_t N_OPS = static_cast<size_t>(C::ERR)+1;
    constexpr auto INP_SIZE = []
    {
        std::vector<std::pair<C, size_t>> tmp = {{C::ERR, 1},
                                                {C::HALT, 1},
                                                {C::ADD, 4},
                                                 {C::MULT, 4},
                                                 {C::INP, 2},
                                                 {C::OUT, 2},
                                                {JNZ, 3},
                                                {JZ, 3},
                                                {LT, 4},
                                                {EQ, 4}};

        std::array<size_t, N_OPS> out{};

        std::sort(tmp.begin(), tmp.end(), [](const std::pair<C, size_t>& a, const std::pair<C, size_t>& b)
        {
            return a.first > b.first;
        });

        for (const auto& [i, kv]: enumerate(tmp))
        {
            out[kv.first] = kv.second;
        }

        return out;
    }();

    [[nodiscard]] inline std::tuple<C, std::vector<uint8_t>> intoc(dt i)
    {

        const auto op = i%100;
        i = i/100;

        C c;
        switch (op){
        case 1:
            c = ADD;
            break;
        case 2:
            c = MULT;
            break;
        case 3:
            c = INP;
            break;
        case 4:
            c = OUT;
            break;
        case 5:
            c = JNZ;
            break;
        case 6:
            c = JZ;
            break;
        case 7:
            c = LT;
            break;
        case 8:
            c = EQ;
            break;
        case 99:
            c = HALT;
            break;
        default:
            c = ERR;
            break;
        }
        std::vector<uint8_t> prefix(INP_SIZE[c]);
        std::ranges::fill(prefix, 0);
        int idx = 0;
        while (i > 0)
        {
            prefix[idx] = i%10;
            idx++;
            i = i/10;
        }
        return {c, prefix};
    };

    inline dt _run_op(Program& prog, const C& op, std::span<dt> inputs, const std::vector<uint8_t>& modes, const dt input, dt& output, dt& ip)
    {
        dt res;
        dt v1, v2, idx_out;
        ip += inputs.size();
        switch (op)
        {
        case C::ADD:
            if (modes[0] == 0)
            {
                v1 = prog[inputs[1]];
            }else
            {
                v1 = inputs[1];
            }

            if (modes[1] == 0)
            {
                v2 = prog[inputs[2]];
            }else
            {
                v2 = inputs[2];
            }
            idx_out = inputs.at(3);
            res = v1+v2;
            prog[idx_out] = res;
            return res;
        case C::MULT:
            {
                if (modes[0] == 0)
                {
                    v1 = prog[inputs[1]];
                }else
                {
                    v1 = inputs[1];
                }

                if (modes[1] == 0)
                {
                    v2 = prog[inputs[2]];
                }else
                {
                    v2 = inputs[2];
                }
                idx_out = inputs.at(3);
                res = v1*v2;
                prog[idx_out] = res;
                return res;
            }
        case C::INP:

            idx_out = inputs.at(1);
            prog[idx_out] = input;
            return input;
        case C::OUT:
            if (modes[0] == 0)
            {
                v1 = prog[inputs[1]];
            }else
            {
                v1 = inputs[1];
            }

            output = v1;
            return output;
        case C::JNZ:
            if (modes[0] == 0)
            {
                v1 = prog[inputs[1]];
            }else
            {
                v1 = inputs[1];
            }
            if (modes[1] == 0)
            {
                v2 = prog[inputs[2]];
            }else
            {
                v2 = inputs[2];
            }
            if (v1 != 0)
            {
                ip = v2;
            }
            return ip;
        case C::JZ:
            if (modes[0] == 0)
            {
                v1 = prog[inputs[1]];
            }else
            {
                v1 = inputs[1];
            }
            if (modes[1] == 0)
            {
                v2 = prog[inputs[2]];
            }else
            {
                v2 = inputs[2];
            }
            if (v1 == 0)
            {
                ip = v2;
            }
            return ip;
        case C::LT:
            if (modes[0] == 0)
            {
                v1 = prog[inputs[1]];
            }else
            {
                v1 = inputs[1];
            }
            if (modes[1] == 0)
            {
                v2 = prog[inputs[2]];
            }else
            {
                v2 = inputs[2];
            }
            idx_out = inputs.at(3);
            prog[idx_out] = static_cast<dt>(v1 < v2);
            return static_cast<dt>(v1 < v2);
        case C::EQ:
            if (modes[0] == 0)
            {
                v1 = prog[inputs[1]];
            }else
            {
                v1 = inputs[1];
            }
            if (modes[1] == 0)
            {
                v2 = prog[inputs[2]];
            }else
            {
                v2 = inputs[2];
            }
            idx_out = inputs.at(3);
            prog[idx_out] = static_cast<dt>(v1 == v2);
            return static_cast<dt>(v1 == v2);
        case C::ERR:
        case C::HALT:
        default:
            return 0;
        }
    };

    inline dt run(Program& prog, const dt input)
    {
        dt ip = 0;
        auto [op, modes] = intoc(prog[ip]);
        dt output = 0;
        while (op != C::HALT)
        {
            _run_op(prog, op, std::span(std::next(prog.begin(), ip), INP_SIZE[op]), modes, input, output, ip);
            std::tie(op, modes) = intoc(prog.at(ip));
        }
        return output;
    };
}

#endif //INTCODE_H
