
#include <iostream>
#include <map>
#include <numeric>
#include <ostream>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "../utils/utils.h"

using P = Point3d<int64_t>;

auto getInput(const std::string& fp){
    auto lines = read_file(fp);
    std::vector<P> out;

    for (const auto& l: lines)
    {
        if (l.empty())
        {
            break;
        }

        auto tmp = split(l.substr(1, l.size()-2), ',');

        out.emplace_back(
            std::stol(split(tmp[0], '=')[1]),
            std::stol(split(tmp[1], '=')[1]),
            std::stol(split(tmp[2], '=')[1])
        );
    }
    return out;

}

template <typename T>
[[nodiscard]] bool span_eq_rev(std::span<T> s1, std::span<T> s2)
{
    if (s1.size() != s2.size())
    {
        return false;
    }
    for (size_t i = 0; i< s1.size(); i++)
    {
        if (s1[i] != s2[s1.size()-i-1])
        {
            return false;
        }
    }
    return true;
}

template <typename T>
[[nodiscard]] bool span_eq(std::span<T> s1, std::span<T> s2)
{
    if (s1.size() != s2.size())
    {
        return false;
    }
    for (size_t i = 0; i< s1.size(); i++)
    {
        if (s1[i] != s2[i])
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

    {
        auto inp = getInput(argv[1]);
        std::vector<P> velos(inp.size());
        // print_arr(std::span(inp));
        // print_arr(std::span(velos));
        for (int _i = 0; _i<1000; _i++)
        {
            for (size_t i = 0; i<inp.size(); i++)
            {
                for (size_t j = i+1; j < inp.size(); j++)
                {
                    const auto g = inp[i]- inp[j];
                    const P signs = {sign(g.x), sign(g.y), sign(g.z)};
                    velos[j] += signs;
                    velos[i] += signs*-1;
                }
            }
            for (size_t i = 0; i<inp.size(); i++)
            {
                inp[i] += velos[i];
            }

        }
        // print_arr(std::span(inp));
        // print_arr(std::span(velos));
        uint64_t energy = 0;
        for (size_t i = 0; i<inp.size(); i++)
        {
            energy += inp[i].sum_abs()*velos[i].sum_abs();
        }
        //lt 14645
        std::println("Part 1: {}", energy);
    }

    {
        auto inp = getInput(argv[1]);

        std::vector<std::vector<P>> pos_hist, velo_hist;
        std::vector<P> velos(inp.size());


        pos_hist.emplace_back(inp);
        {
            std::vector<P> tmp;
            for (size_t i = 0; i<inp.size(); i++)
            {
                tmp.emplace_back(0,0,0);
            }
            velo_hist.emplace_back(tmp);
        }

        for (int _i = 0; _i<1000000; _i++)
        {
            for (size_t i = 0; i<inp.size(); i++)
            {
                for (size_t j = i+1; j < inp.size(); j++)
                {
                    const auto g = inp[i]- inp[j];
                    const P signs = {sign(g.x), sign(g.y), sign(g.z)};
                    velos[j] += signs;
                    velos[i] += signs*-1;
                }
            }

            for (size_t i = 0; i<inp.size(); i++)
            {
                inp[i] += velos[i];
            }
            velo_hist.emplace_back(velos);
            pos_hist.emplace_back(inp);
        }

        std::vector<P> rep_time(inp.size());
        std::vector<P> rep_timev(inp.size());
        for (size_t i = 0; i<inp.size(); i++)
        {
            std::vector<int64_t> x_hist, y_hist, z_hist, xv_hist, yv_hist, zv_hist;
            bool x_conf=false, y_conf=false, z_conf=false, xv_conf=false, yv_conf=false, zv_conf=false;
            for (size_t j = 0; j<pos_hist.size(); j++)
            {

                if (x_conf && y_conf && z_conf && xv_conf && yv_conf && zv_conf)
                {
                    break;
                }

                const auto p = pos_hist[j][i];
                const auto v = velo_hist[j][i];

                if (!x_conf)
                {
                    x_hist.emplace_back(p.x);
                    if (const auto s = x_hist.size(); span_eq_rev(std::span(x_hist.begin(), x_hist.begin() + s/2),
                                                              std::span(x_hist.begin() + s/2, x_hist.end())))
                    {
                        std::vector<int64_t> tmp;
                        for (size_t k = j+1; k<pos_hist.size() && k<(j+s+1); k++)
                        {
                            tmp.emplace_back(pos_hist[k][i].x);
                        }
                        if (std::equal(x_hist.begin(), x_hist.end(), tmp.begin()))
                        {
                            rep_time[i].x = s;
                            x_conf = true;
                        }
                    }
                }

                if (!y_conf)
                {
                    y_hist.emplace_back(p.y);
                    if (const auto s = y_hist.size(); span_eq_rev(std::span(y_hist.begin(), y_hist.begin() + s/2),
                                                              std::span(y_hist.begin() + s/2, y_hist.end())))
                    {
                        std::vector<int64_t> tmp;
                        for (size_t k = j+1; k<pos_hist.size() && k<(j+s+1); k++)
                        {
                            tmp.emplace_back(pos_hist[k][i].y);
                        }
                        if (std::equal(y_hist.begin(), y_hist.end(), tmp.begin()))
                        {
                            rep_time[i].y = s;
                            y_conf = true;
                        }
                    }
                }

                if (!z_conf)
                {
                    z_hist.emplace_back(p.z);
                    if (const auto s = z_hist.size(); span_eq_rev(std::span(z_hist.begin(), z_hist.begin() + s/2),
                                                              std::span(z_hist.begin() + s/2, z_hist.end())))
                    {
                        std::vector<int64_t> tmp;
                        for (size_t k = j+1; k<pos_hist.size() && k<(j+s+1); k++)
                        {
                            tmp.emplace_back(pos_hist[k][i].z);
                        }
                        if (std::equal(z_hist.begin(), z_hist.end(), tmp.begin()))
                        {
                            rep_time[i].z = s;
                            z_conf = true;
                        }
                    }
                }

                if (!xv_conf)
                {
                    xv_hist.emplace_back(v.x);
                    if (const auto s = xv_hist.size(); span_eq(std::span(xv_hist.begin(), xv_hist.begin() + s/2),
                                                              std::span(xv_hist.begin() + s/2, xv_hist.end())))
                    {
                        std::vector<int64_t> tmp;
                        for (size_t k = j+1; k<velo_hist.size() && k<(j+s+1); k++)
                        {
                            tmp.emplace_back(velo_hist[k][i].x);
                        }
                        if (std::equal(xv_hist.begin(), xv_hist.end(), tmp.begin()))
                        {
                            rep_timev[i].x = s/2;
                            xv_conf = true;
                        }
                    }
                }

                if (!yv_conf)
                {
                    yv_hist.emplace_back(v.y);
                    if (const auto s = yv_hist.size(); span_eq(std::span(yv_hist.begin(), yv_hist.begin() + s/2),
                                                              std::span(yv_hist.begin() + s/2, yv_hist.end())))
                    {
                        std::vector<int64_t> tmp;
                        for (size_t k = j+1; k<velo_hist.size() && k<(j+s+1); k++)
                        {
                            tmp.emplace_back(velo_hist[k][i].y);
                        }
                        if (std::equal(yv_hist.begin(), yv_hist.end(), tmp.begin()))
                        {
                            rep_timev[i].y = s/2;
                            yv_conf = true;
                        }
                    }
                }

                if (!zv_conf)
                {
                    zv_hist.emplace_back(v.z);
                    if (const auto s = zv_hist.size(); span_eq(std::span(zv_hist.begin(), zv_hist.begin() + s/2),
                                                              std::span(zv_hist.begin() + s/2, zv_hist.end())))
                    {
                        std::vector<int64_t> tmp;
                        for (size_t k = j+1; k<velo_hist.size() && k<(j+s+1); k++)
                        {
                            tmp.emplace_back(velo_hist[k][i].z);
                        }
                        if (std::equal(zv_hist.begin(), zv_hist.end(), tmp.begin()))
                        {
                            rep_timev[i].z = s/2;
                            zv_conf = true;
                        }
                    }
                }
            }
        }

        uint64_t lcm = 1;
        for (const auto& [x,y,z]: rep_time)
        {
            lcm = std::lcm(lcm, x);
            lcm = std::lcm(lcm, y);
            lcm = std::lcm(lcm, z);
        }
        for (const auto& [x,y,z]: rep_timev)
        {
            lcm = std::lcm(lcm, x);
            lcm = std::lcm(lcm, y);
            lcm = std::lcm(lcm, z);
        }
        std::println("{}", lcm);
    }
}