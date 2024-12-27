//
// Created by daniel on 12/27/24.
//

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstdio>

#include "../utils/utils.h"

auto getInput(const std::string& fp){
    auto lines = read_file(fp);
    std::pair<int, int> out;
    for (const auto& l: lines)
    {
        if (l.empty())
        {
            break;
        }

        auto nums = split(l, '-');
        out.first = std::stoi(nums[0]);
        out.second = std::stoi(nums[1]);
    }
    return out;
}

std::array<uint8_t, 6> inttoarr(int i)
{
    std::array<uint8_t, 6> arr{};
    for (int idx = 5; idx>=0; idx--)
    {
        arr[idx] = i%10;
         i = i/10;
    }
    return arr;
}

bool has_dupe(const std::array<uint8_t, 6>& arr)
{

    for (const auto&[v1, v2]: arr | std::views::adjacent<2>)
    {
        if (v1 == v2)
        {
            return true;
        }
    }
    return false;
}

bool increases(const std::array<uint8_t, 6>& arr)
{
    for (const auto&[v1, v2]: arr | std::views::adjacent<2>)
    {
        if (v1 > v2)
        {
            return false;
        }
    }
    return true;
}

bool has_short_dupe(const std::array<uint8_t, 6>& arr)
{

    for (const auto&[v1,v2,v3,v4]: arr | std::views::adjacent<4>)
    {
        if (v2==v3 && v1!=v2 && v4!=v3)
        {
            return true;
        }
    }

    return (arr[3] != arr[4] && arr[4] == arr[5]) || (arr[2] != arr[0] && arr[0] == arr[1]);
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
        int match = 0;
        for (int i = inp.first; i <= inp.second; i++)
        {
            auto tmp = inttoarr(i);
            match += has_dupe(tmp) && increases(tmp);
        }
        std::println("Part 1: {}", match);
    }

    {
        int match = 0;
        for (int i = inp.first; i <= inp.second; i++)
        {
            auto tmp = inttoarr(i);
            match += has_dupe(tmp) && increases(tmp) && has_short_dupe(tmp);

        }
        std::println("Part 2: {}", match);
    }
}