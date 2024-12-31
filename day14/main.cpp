
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

using dt = int64_t;

struct Input
{
    std::string name;
    dt qty{};

    Input() = default;
    Input(std::string  name, const dt& qty): name(std::move(name)), qty(qty){};
};

struct Reaction
{
    std::string output;
    dt out_qty{};
    std::vector<Input> inputs;

    Reaction() = default;
    Reaction(std::string output, const dt& qty, const std::vector<Input>& inputs): output(std::move(output)), out_qty(qty), inputs(inputs){};

};

using Ore_reactions = std::tuple<dt, dt, std::string>;
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
                inputs.emplace_back(ss[1], std::stoll(ss[0]));
            }
            auto s = split(tmp[1], ' ', true);
            reactions[s[1]] = {s[1], std::stoll(s[0]), inputs};
        }


    }
    return {ores, reactions};

}

template <typename T>
[[nodiscard]] bool set_contains(const std::unordered_set<T>& s1, const std::unordered_set<T>& s2)
{
    for (const auto& ele: s2)
    {
        if (!s1.contains(ele))
        {
            return false;
        }
    }
    return true;
}

int main(const int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid args" << std::endl;
        return 1;
    }

    const std::string goal = "FUEL";

    {
        auto [ore_reactions, reactions] = getInput(argv[1]);

        std::unordered_set<std::string> base;
        for (const auto&[i,o,on]: ore_reactions)
        {
            base.emplace(on);
        }

        std::unordered_map<std::string, std::unordered_set<std::string>> reaction_children;
        for (const auto& r: reactions | std::ranges::views::values)
        {
            for (const auto& i: r.inputs)
            {
                reaction_children[i.name].emplace(r.output);
            }
        }

        std::unordered_map<std::string, dt> need;
        std::unordered_set<std::string> done;
        std::unordered_map<std::string, dt> used;

        for (const auto& i : reactions[goal].inputs)
        {
            if (base.contains(i.name))
            {
                used[i.name] += i.qty;
            }else
            {
                need[i.name] += i.qty;
            }
        }
        done.emplace(goal);
        reactions.erase(reactions.find(goal));

        while (!reactions.empty())
        {
            std::vector<std::string> toremove;

            for (const auto& r: reactions | std::ranges::views::values)
            {
                if (set_contains(done, reaction_children[r.output]))
                {
                    // All children reactions for this reaction are done

                    auto cnt = need[r.output];
                    need.erase(need.find(r.output));
                    dt react_count;
                    if (cnt%r.out_qty == 0)
                    {
                        react_count = cnt/r.out_qty;
                    }else
                    {
                        react_count = (cnt+r.out_qty-(cnt%r.out_qty))/r.out_qty;
                    }
                    std::println("Need to make {} {} (Must actually create {})", cnt, r.output, react_count);

                    for (const auto& i : r.inputs)
                    {
                        if (base.contains(i.name))
                        {
                            used[i.name] += i.qty*react_count;
                        }else
                        {
                            need[i.name] += i.qty*react_count;
                        }
                    }
                    toremove.emplace_back(r.output);
                    done.emplace(r.output);
                }
            }

            for (const auto& ele: toremove)
            {
                reactions.erase(reactions.find(ele));
            }
        }

        dt count = 0;
        for (const auto& [name, usage]: used)
        {
            for (const auto&[i,o,on]: ore_reactions)
            {
                if (on == name)
                {
                    if (usage%o == 0)
                    {
                        count += i * usage/o;
                    }else
                    {
                        count += i* (usage+o-(usage%o))/o;
                    }
                }
            }
        }
        std::println("Part 1: {}", count);
    }
 ///////////////////////////////////////////////////////////////
    {
        auto [ore_reactions, reactions] = getInput(argv[1]);

        std::unordered_set<std::string> base;
        for (const auto&[i,o,on]: ore_reactions)
        {
            base.emplace(on);
        }

        std::unordered_map<std::string, std::unordered_set<std::string>> reaction_children;
        for (const auto& r: reactions | std::ranges::views::values)
        {
            for (const auto& i: r.inputs)
            {
                reaction_children[i.name].emplace(r.output);
            }
        }


        std::unordered_map<std::string, dt> avail;


        dt avail_ore = 1000000000000;
        dt n_fuel = 0;
        while (true)
        {

            std::unordered_map<std::string, dt> need;
            std::unordered_set<std::string> done;
            std::unordered_map<std::string, dt> used;
            auto reactions_cpy = reactions;
            for (const auto& i : reactions[goal].inputs)
            {
                if (base.contains(i.name))
                {
                    used[i.name] += i.qty;
                }else
                {
                    need[i.name] += i.qty;
                }
            }
            done.emplace(goal);
            reactions_cpy.erase(reactions_cpy.find(goal));

            while (!reactions_cpy.empty())
            {
                std::vector<std::string> toremove;

                for (const auto& r: reactions_cpy | std::ranges::views::values)
                {
                    if (set_contains(done, reaction_children[r.output]))
                    {
                        // All children reactions for this reaction are done

                        auto cnt = need[r.output];
                        if (cnt >= avail[r.output])
                        {
                            cnt -= avail[r.output];
                            avail[r.output] = 0;
                        }
                        else
                        {
                            avail[r.output] -= cnt;
                            cnt = 0;
                        }

                        need.erase(need.find(r.output));
                        if (cnt > 0)
                        {
                            dt react_count;
                            if (cnt%r.out_qty == 0)
                            {
                                react_count = cnt/r.out_qty;
                            }else
                            {
                                const auto tmp = (cnt+r.out_qty-(cnt%r.out_qty))/r.out_qty;
                                react_count = tmp;
                            }
                            avail[r.output] = react_count*r.out_qty - cnt;

                            for (const auto& i : r.inputs)
                            {
                                if (base.contains(i.name))
                                {
                                    used[i.name] += i.qty*react_count;
                                }else
                                {
                                    need[i.name] += i.qty*react_count;
                                }
                            }
                        }
                        toremove.emplace_back(r.output);
                        done.emplace(r.output);
                    }
                }

                for (const auto& ele: toremove)
                {
                    reactions_cpy.erase(reactions_cpy.find(ele));
                }
            }

            dt tmp_cnt = 0;
            for (auto& [name, usage]: used)
            {
                for (const auto&[i,o,on]: ore_reactions)
                {
                    if (on == name)
                    {
                        if (usage >= avail[name])
                        {
                            usage -= avail[name];
                            avail[name] = 0;
                        }
                        else
                        {
                            avail[name] -= usage;
                            usage = 0;
                        }

                        if (usage%o == 0)
                        {
                            tmp_cnt += i * usage/o;
                            avail[name] = 0;
                        }else
                        {
                            const auto tmp = i*(usage+o-(usage%o))/o;
                            tmp_cnt += tmp;
                            avail[name] = o-(usage%o);
                        }
                    }
                }
            }
            if (tmp_cnt > avail_ore)
            {
                break;
            }else
            {
                n_fuel += 1;
                avail_ore -= tmp_cnt;
            }
        }
        std::println("Part 2: {}", n_fuel);
    }
}