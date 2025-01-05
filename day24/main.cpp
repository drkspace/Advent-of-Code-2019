
#include <iostream>
#include <map>
#include <numeric>
#include <ostream>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <generator>
#include <omp.h>
#include <queue>

#include "../utils/utils.h"
#include "../easyTimer.h"

using dt = int64_t;

using Map = std::array<std::array<char, 5>, 5>;

Map getInput(const std::string& fp){
    auto lines = read_file(fp);
    Map out;

    for (const auto& [i, l]: enumerate(lines))
    {
        if (l.empty())
        {
            break;
        }
        for (const auto& [j,c]: enumerate(l))
        {
            out[i][j] = c;
        }

    }
    return out;

}

[[nodiscard]] dt score(const Map& map)
{
    int i = 0;
    dt score = 0;
    for (const auto& r: map)
    {
        for (const auto& c: r)
        {
            if (c=='#')
            {
                score |= 1<<i;
            }
            i++;
        }
    }
    return score;
}

[[nodiscard]] Map next_step(const Map& prev)
{
    Map next;
    for (size_t i = 0; i< prev.size(); i++)
    {
        for (size_t j = 0; j< prev.size(); j++)
        {
            int n_adj = 0;
            if (i > 0 && prev[i-1][j] == '#')
            {
                n_adj++;
            }
            if (i < (prev.size()-1) && prev[i+1][j] == '#')
            {
                n_adj++;
            }
            if (j > 0 && prev[i][j-1] == '#')
            {
                n_adj++;
            }
            if (j < (prev[i].size()-1) && prev[i][j+1] == '#')
            {
                n_adj++;
            }

            if (prev[i][j] == '.')
            {
                if (n_adj == 1 || n_adj == 2)
                {
                    next[i][j] = '#';
                }else
                {
                    next[i][j] = '.';
                }
            }else
            {
                if (n_adj!=1)
                {
                    next[i][j] = '.';
                }else
                {
                    next[i][j] = '#';
                }
            }
        }
    }
    return next;
}

template <size_t N>
[[nodiscard]] std::array<Map, N> next_step(const std::array<Map, N>& prev_rec)
{
    std::array<Map, N> next_rec{};
    for (size_t level = 0; level < N; level++)
    {
        auto& prev = prev_rec[level];
        auto& next = next_rec[level];
        for (size_t i = 0; i< prev.size(); i++)
        {
            for (size_t j = 0; j< prev.size(); j++)
            {
                if (i==2 && j == 2)
                {
                    next[i][j] = '?';
                    continue;
                }
                int n_adj = 0;
                n_adj += i > 0 && prev[i-1][j] == '#';
                n_adj += i < (prev.size()-1) && prev[i+1][j] == '#';
                n_adj += j > 0 && prev[i][j-1] == '#';
                n_adj += j < (prev[i].size()-1) && prev[i][j+1] == '#';

                // Deal with the inner edges
                if (i==1 && j == 2)
                {
                    for (const auto& c: prev_rec[level+1][0])
                    {
                        n_adj += c == '#';
                    }
                }
                if (i==2 && j == 1)
                {
                    for (const auto& r: prev_rec[level+1])
                    {
                        n_adj += r[0] == '#';
                    }
                }
                if (i==2 && j == 3){
                    for (const auto& r: prev_rec[level+1])
                    {
                        n_adj += r.back() == '#';
                    }
                }
                if (i==3 && j == 2)
                {
                    for (const auto& c: prev_rec[level+1].back())
                    {
                        n_adj += c == '#';
                    }
                }

                // Deal with the outer edges
                if (level != 0)
                {
                    const auto& above = prev_rec[level-1];
                    if (i==0)
                    {
                        n_adj += above[1][2] == '#';
                    }
                    else if (i == (prev.size()-1))
                    {
                        n_adj += above[3][2] == '#';
                    }
                    if (j == 0)
                    {
                        n_adj += above[2][1] == '#';
                    }
                    else if (j == (prev[0].size()-1))
                    {
                        n_adj += above[2][3] == '#';
                    }
                }
                if (prev[i][j] == '.')
                {
                    if (n_adj == 1 || n_adj == 2)
                    {
                        next[i][j] = '#';
                    }else
                    {
                        next[i][j] = '.';
                    }
                }else
                {
                    if (n_adj!=1)
                    {
                        next[i][j] = '.';
                    }else
                    {
                        next[i][j] = '#';
                    }
                }
            }
        }
    }
    return next_rec;
}


int main(const int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid args" << std::endl;
        return 1;
    }

    {
        auto map = getInput(argv[1]);

        std::unordered_set<dt> found;

        while (true)
        {
            for (const auto& r: map)
            {
                for (const auto& c: r)
                {
                    std::print("{}", c);
                }
                std::println("");
            }
            const auto s = score(map);
            std::println("score: {}", s);

            if (found.contains(s))
            {
                break;
            }
            found.emplace(s);

            map = next_step(map);
        }
    }
    std::print("\n\n");
    {
        const auto inp_map = getInput(argv[1]);
        constexpr size_t N_MINUTES = 200;
        std::array<Map, N_MINUTES*2+1> maps{};


        for (auto& map: maps)
        {
            for (auto& r: map)
            {
                r.fill('.');
            }
        }
        maps[N_MINUTES] = inp_map;
        for (size_t i = 0; i< N_MINUTES; i++)
        {
            maps = next_step(maps);
        }
        size_t count = 0;
        for (const auto& [i,map]: enumerate(maps))
        {
            std::println("Depth {}", i-static_cast<long>(N_MINUTES));
            for (const auto& r: map)
            {
                for (const auto& c: r)
                {
                    count += c == '#';
                    std::print("{}", c);
                }
                std::println("");
            }
            std::println("");
        }

        std::println("Part 2: {}", count);
    }


}