//
// Created by daniel on 12/27/24.
//

#ifndef INTCODE_H
#define INTCODE_H
#include <cstdint>
#include <map>
#include <vector>
#include <queue>

#include "../utils/utils.h"

namespace IntCode
{
    using dt = intmax_t;
    using InputQueue = std::queue<dt>;
    struct Program
    {
        std::vector<dt> program;
        dt relative_base = 0;

        Program() = default;
        Program(const Program& other) = default;
        explicit Program(const std::vector<dt>& program): program(program), relative_base(0){};

        void set(const dt address, const dt val)
        {
            if (address < 0)
            {
                std::println("PANIC");
                return;
            }
            if (static_cast<size_t>(address)+1 > program.size())
            {
                // std::println("Resizing to {}", address+1);
                program.resize(address+1, 0);
            }
            program.at(address) = val;
        }
    };

    enum C
    {
        HALT,ADD,MULT,INP,OUT,JNZ,JZ,LT,EQ,RBA,
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
                                                {EQ, 4},
                                                {RBA, 2}};

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
        case 9:
            c = RBA;
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

    [[nodiscard]] inline bool next_is_inp(const Program& prog, const dt& ip)
    {
        return std::get<0>(intoc(prog.program[ip])) == C::INP;
    }

    [[nodiscard]] inline bool next_is_out(const Program& prog, const dt& ip)
    {
        return std::get<0>(intoc(prog.program[ip])) == C::OUT;
    }

    [[nodiscard]] inline bool halted(const Program& prog, const dt& ip)
    {
        return std::get<0>(intoc(prog.program[ip])) == C::HALT;
    }

    inline dt _get_mode_val(const dt& mode, const Program& prog, const dt& arg)
    {
        switch (mode)
        {
        case 0:
            if (static_cast<size_t>(arg) >= prog.program.size())
            {
                return 0;
            }
            else
            {
                return prog.program.at(arg);
            }
        case 1:
            return arg;
        case 2:
            if (static_cast<size_t>(arg+prog.relative_base) >= prog.program.size())
            {
                return 0;
            }
            else
            {
                return prog.program.at(arg+prog.relative_base);
            }
        default:
            return 0;
        }
    }

    inline dt _get_mode_val_idx(const dt& mode, const Program& prog, const dt& arg)
    {
        switch (mode)
        {
        case 0:
            return arg;
        case 2:
            return arg+prog.relative_base;
        case 1:
        default:
            std::println("INVALID MODE: {}", mode);
            return arg;
        }
    }

    inline dt _run_op(Program& prog, const C& op, std::span<dt> arguments, const std::vector<uint8_t>& modes, std::queue<dt>& inputs, dt& output, dt& ip)
    {
        dt res;
        dt v1, v2, idx_out;
        ip += arguments.size();
        switch (op)
        {
        case C::ADD:
            v1 = _get_mode_val(modes[0], prog, arguments[1]);
            v2 = _get_mode_val(modes[1], prog, arguments[2]);
            idx_out = _get_mode_val_idx(modes[2], prog, arguments.at(3));
            res = v1+v2;
            prog.set(idx_out, res);
            return res;
        case C::MULT:
            {
                v1 = _get_mode_val(modes[0], prog, arguments[1]);
                v2 = _get_mode_val(modes[1], prog, arguments[2]);
                idx_out = _get_mode_val_idx(modes[2], prog, arguments.at(3));
                res = v1*v2;
                prog.set(idx_out, res);
                return res;
            }
        case C::INP:

            idx_out = _get_mode_val_idx(modes[0], prog, arguments.at(1));
            if (inputs.empty())
            {
                std::println("panic");
            }
            v1 = inputs.front();
            inputs.pop();
            prog.set(idx_out, v1);
            return v1;
        case C::OUT:
            v1 = _get_mode_val(modes[0], prog, arguments.at(1));
            output = v1;
            return output;

        case C::JNZ:
            v1 = _get_mode_val(modes[0], prog, arguments.at(1));
            v2 = _get_mode_val(modes[1], prog, arguments.at(2));

            if (v1 != 0)
            {
                ip = v2;
            }
            return ip;
        case C::JZ:
            v1 = _get_mode_val(modes[0], prog, arguments.at(1));
            v2 = _get_mode_val(modes[1], prog, arguments.at(2));

            if (v1 == 0)
            {
                ip = v2;
            }
            return ip;
        case C::LT:
            v1 = _get_mode_val(modes[0], prog, arguments.at(1));
            v2 = _get_mode_val(modes[1], prog, arguments.at(2));
            idx_out = _get_mode_val_idx(modes[2], prog, arguments.at(3));

            prog.set(idx_out, v1 < v2);
            return v1 < v2;
        case C::EQ:
            v1 = _get_mode_val(modes[0], prog, arguments.at(1));
            v2 = _get_mode_val(modes[1], prog, arguments.at(2));
            idx_out = _get_mode_val_idx(modes[2], prog, arguments.at(3));

            prog.set(idx_out, v1 == v2);
            return v1 == v2;
        case C::RBA:
            v1 = _get_mode_val(modes[0], prog, arguments.at(1));
            prog.relative_base += v1;
            return prog.relative_base;
        case C::ERR:
        case C::HALT:
        default:
            return 0;
        }
    };

    inline dt run(Program& prog, std::queue<dt>& inputs)
    {
        dt ip = 0;
        auto [op, modes] = intoc(prog.program[ip]);
        dt output = 0;
        while (op != C::HALT)
        {
            _run_op(prog, op, std::span(std::next(prog.program.begin(), ip), INP_SIZE[op]), modes, inputs, output, ip);
            std::tie(op, modes) = intoc(prog.program.at(ip));
        }
        return output;
    };

    /**
     * \brief Runs the program until the next IO command or HALT. If the instruction at ip is an IO command,
     * it will be run.
     * \return
     */
    inline dt run_till_io(Program& prog, std::queue<dt>& inputs, dt& ip)
    {
        auto [op, modes] = intoc(prog.program[ip]);
        dt output = 0;
        if (op == C::HALT)
        {
            return 0;
        }
        do {
            _run_op(prog, op, std::span(std::next(prog.program.begin(), ip), INP_SIZE[op]), modes, inputs, output, ip);
            std::tie(op, modes) = intoc(prog.program.at(ip));
        } while (op != C::HALT && op != C::INP && op != C::OUT);
        return output;
    };
}

#endif //INTCODE_H
