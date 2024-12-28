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
        auto inp = getInput(argv[1]);

        auto out = IntCode::run(inp, 1);
        std::println("Part 1: {}", out);
    }
    {
        auto inp = getInput(argv[1]);

        auto out = IntCode::run(inp, 5);
        std::println("Part 2: {}", out);
    }
}