
#include <iostream>
#include <ostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "../utils/utils.h"
constexpr int WIDTH = 25;
constexpr int HEIGHT = 6;

using layer = std::array<std::array<uint8_t, WIDTH>, HEIGHT>;

auto getInput(const std::string& fp){
    auto lines = read_file(fp);
    std::vector<layer> out;
    const std::unordered_map<char, uint8_t> char_conv = {{'0', 0},
    {'1', 1},
    {'2', 2},
    {'3', 3},
    {'4', 4},
    {'5', 5},
    {'6', 6},
    {'7', 7},
    {'8', 8},
    {'9', 9}};
    for (const auto& l: lines)
    {
        if (l.empty())
        {
            break;
        }

        size_t line_pos = 0;
        while (line_pos < l.size())
        {
            layer tmp;
            for (int i = 0; i<HEIGHT; i++)
            {
                for (int j = 0; j<WIDTH; j++)
                {
                    tmp[i][j] = char_conv.find(l.at(line_pos+i*WIDTH+j))->second;
                }
            }
            out.emplace_back(tmp);
            line_pos += WIDTH*HEIGHT;
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

    const auto inp = getInput(argv[1]);
    {
        layer least_0;
        int least_0_count = std::numeric_limits<int>::max();
        for (const auto& l: inp)
        {
            int n0 = 0;
            for (const auto& r: l)
            {
                for (const auto& ele: r)
                {
                    n0 += ele==0;
                }
            }
            if (n0 < least_0_count)
            {
                least_0 = l;
                least_0_count = n0;
            }
        }
        int n1 = 0;
        int n2 = 0;
        for (const auto& r: least_0)
        {
            for (const auto& ele: r)
            {
                n1 += ele==1;
                n2 += ele==2;
            }
        }
        std::println("Part 1: {}", n1*n2);
    }

    {
        layer out;
        for (int i = 0; i< HEIGHT; i++)
        {
            for (int j = 0; j<WIDTH; j++)
            {
                for (const auto& l: inp)
                {
                    if (l[i][j] == 0)
                    {
                        out[i][j] = 0;
                        break;
                    }
                    if (l[i][j] == 1)
                    {
                        out[i][j] = 1;
                        break;
                    }
                }
            }
        }

        std::println("Part 2: ");
        for (const auto& r: out)
        {
            for (const auto& ele: r)
            {
                if (ele == 1)
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