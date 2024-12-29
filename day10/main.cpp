
#include <iostream>
#include <map>
#include <numeric>
#include <ostream>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "../utils/utils.h"

using P = Point2d<int>;

auto getInput(const std::string& fp){
    auto lines = read_file(fp);
    std::vector<P> out;

    for (const auto& [row,l]: enumerate(lines))
    {
        if (l.empty())
        {
            break;
        }

        for (const auto& [col,c]: enumerate(l))
        {
            if (c == '#')
            {
                out.emplace_back(col, row);
            }
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
    auto comp = [](const P& a, const P& b)
    {
        return a.radicand() > b.radicand();
    };
    auto comp2 = [](const P& a, const P& b)
    {
        auto tmpa = pmod(a.angle()*180./M_PI+360., 360.);
        if (a.x == 0 && a.y == 1)
        {
            tmpa = 90.;
        }
        auto tmpb = pmod(b.angle()*180./M_PI+360., 360.);
        if (b.x == 0 && b.y == 1)
        {
            tmpb = 90.;
        }
        return pmod(pmod(tmpa-90, 360.), 360.) < pmod(pmod(tmpb-90., 360.), 360.);
    };
    using dist_type = std::map<P, std::set<P, decltype(comp)>, decltype(comp2)>;
    size_t best_count = 0;
    dist_type best_dists;
    for (size_t i = 0; i < inp.size(); ++i)
    {

        dist_type distances;
        for (size_t j = 0; j < inp.size(); ++j)
        {
            if (j == i)
            {
                continue;
            }
            auto tmp = inp[i]-inp[j];
            tmp = tmp/std::gcd(tmp.x, tmp.y);

            distances[tmp].emplace(inp[j]);
        }
        if (distances.size() > best_count)
        {
            best_count = distances.size();
            best_dists = distances;
        }
    }
    std::println("Part 1: {}", best_count);

    int n_del = 0;
    while (n_del < 200)
    {
        for (auto& v : best_dists | std::views::values)
        {
            if (!v.empty())
            {
                auto tmp = *(v.begin());
                // std::println("#{} = {}", n_del+1, tmp);
                v.erase(v.begin());
                n_del++;
                if (n_del == 200)
                {
                    std::println("Part 2: {} -> {}", tmp, tmp.x*100+tmp.y);
                    break;
                }
            }
        }
    }

}