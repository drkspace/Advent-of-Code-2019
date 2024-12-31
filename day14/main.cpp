
#include <iostream>
#include <map>
#include <numeric>
#include <ostream>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <bits/ranges_algo.h>

#include "../utils/utils.h"

struct Input
{
    std::string name;
    int qty{};

    Input() = default;
    Input(std::string  name, const int& qty): name(std::move(name)), qty(qty){};
};

struct Reaction
{
    std::string output;
    int out_qty{};
    std::vector<Input> inputs;

    Reaction() = default;
    Reaction(std::string output, const int& qty, const std::vector<Input>& inputs): output(std::move(output)), out_qty(qty), inputs(inputs){};

};

using Ore_reactions = std::tuple<int, int, std::string>;
using Reactions = std::unordered_map<std::string, Reaction>;


std::tuple<std::vector<Ore_reactions>, Reactions> getInput(const std::string& fp){
    auto lines = read_file(fp);
    std::vector<Ore_reactions> ores;
    Reactions reactions;

    for (const auto& l: lines)
    {
        if (l.empty())
        {
            break;
        }


        if (l.contains(" ORE =>"))
        {
            auto tmp = split(l, ' ');
            ores.emplace_back(
                std::stoi(tmp[0]),
                std::stoi(tmp[3]),
                tmp[4]
            );
        }else
        {
            const auto tmp = split(l, '>');
            std::vector<Input> inputs;
            for (const auto& s: split(tmp[0], ','))
            {
                auto ss = split(s, ' ', true);
                inputs.emplace_back(ss[1], std::stoi(ss[0]));
            }
            auto s = split(tmp[1], ' ', true);
            reactions[s[1]] = {s[1], std::stoi(s[0]), inputs};
        }


    }
    return {ores, reactions};

}

int main(const int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid args" << std::endl;
        return 1;
    }

    auto [ore_reactions, reactions] = getInput(argv[1]);

    using BaseCosts = std::unordered_map<std::string, int>;
    std::unordered_map<std::string, BaseCosts> bc;
    std::unordered_set<std::string> base;
    for (const auto&[i,o,on]: ore_reactions)
    {
        bc[on] = {{on,1}};
        base.emplace(on);
    }


    while (!reactions.empty())
    {
        std::vector<std::string> to_remove;
        for (const auto&[on, reac]: reactions)
        {
            if (std::ranges::all_of(reac.inputs.begin(), reac.inputs.end(), [&](const Input& i)
            {
                return bc.contains(i.name);
            }))
            {
                BaseCosts tmp;
                for (const auto& i: reac.inputs)
                {
                    for (const auto& [p,v]: bc[i.name])
                    {
                        tmp[p] += v * i.qty;
                    }

                }
                bc[on] = tmp;
                to_remove.emplace_back(on);
            }
        }
        for (const auto& ele: to_remove)
        {
            reactions.erase(reactions.find(ele));
        }

    }
    const auto& fuel_needs = bc["FUEL"];
    int ore_needs = 0;
    for (const auto&[name, n]: fuel_needs)
    {
        // If 10:3 and need 5
        // need 20 ore
        // ceil(5/3)*10

        std::println("Need {} {}s", n, name);
        for (const auto&[i,o,on]: ore_reactions)
        {
            if (on == name)
            {
                if (pmod(n,o) == 0)
                {
                    auto tmp =i * (n/o);
                    std::println("Need {} ores for {}", tmp, name);
                    ore_needs += tmp;
                }else
                {
                    auto tmp = i * ((n+o-pmod(n,o))/o);
                    std::println("Need {} ores for {}", tmp, name);
                    ore_needs += tmp;
                }
                break;
            }
        }
    }
    std::println("Part 1: {}", ore_needs);
}