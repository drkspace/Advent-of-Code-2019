//
// Created by daniel on 12/26/24.
//

#include <iostream>
#include <vector>
#include <string>
#include "../utils/utils.h"

std::vector<int> getInput(const std::string& fp){
    auto lines = read_file(fp);
    std::vector<int> out;
    out.reserve(lines.size());
    for (const auto& l: lines)
    {
        if (l.empty())
        {
            break;
        }
        out.emplace_back(std::stoi(l));
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

        int count = 0;
        for (const auto& ele: inp)
        {
            count += (ele/3) - 2;
        }

        std::println("{}", count);
    }

    {
        const auto inp = getInput(argv[1]);

        int count = 0;
        for (const auto& ele: inp)
        {
            int tmp = (ele/3) -2;

            while (tmp > 0)
            {
                count += tmp;
                tmp = (tmp/3) -2;
            }
        }

        std::println("{}", count);
    }
}