
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

#include "../utils/utils.h"
#include "../easyTimer.h"

using dt = int32_t;

constexpr std::array<dt, 4> BASE_PATTERN = {0, 1, 0, -1};

std::generator<dt> construct_pattern(const dt idx)
{

    std::vector<dt> n;
    n.reserve(idx*BASE_PATTERN.size());
    for (const auto& ele: BASE_PATTERN)
    {
        for (int i = 0; i < idx; ++i)
        {
            n.emplace_back(ele);
        }
    }
    bool first = true;
    for (const auto& r: std::ranges::views::repeat(n))
    {
        auto itr = r.begin();
        [[unlikely]] if (first)
        {
            itr += idx;

            first = false;
        }
        for (;itr!=r.end();++itr)
        {
            co_yield *itr;
        }
    }
}

std::vector<dt> getInput(const std::string& fp){
    auto lines = read_file(fp);
    std::vector<dt> out;

    for (const auto& l: lines)
    {
        if (l.empty())
        {
            break;
        }

        for (const auto& ele: l)
        {
            out.emplace_back(std::stoll(std::string(1, ele)));
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
        for (int _i = 0; _i < 100; _i++)
        {
            // print_arr(std::span(inp));
            std::vector<dt> tmp(inp.size());
            for (const auto& [i,v]: enumerate(inp))
            {
                auto gen = construct_pattern(i+1);
                auto gen_itr = gen.begin();
                dt accum = 0;
                for (const auto& v2: std::span(inp.begin()+i, inp.end()))
                {
                    const auto pv = *gen_itr;
                    ++gen_itr;
                    // std::print("{}*{} + ", v2, pv);
                    accum += v2*pv;
                }
                tmp[i] = std::abs(accum)%10;
                // std::println(" = {} {}", tmp[i], accum);
            }
            inp = tmp;
        }
        // print_arr(std::span(inp));
        print_arr(std::span(inp.begin(), inp.begin()+8),"");
    }

    {
        auto inp = getInput(argv[1]);
        dt offset = 0;
        for (int i = 0; i< 7; i++)
        {
            offset = offset*10+inp[i];
        }
        std::println("offsetting {}", offset);
        {
            auto inp_cpy = inp;
            for (int i = 0; i< 10000-1; i++)
            {
                inp.insert(inp.end(), inp_cpy.begin(), inp_cpy.end());
            }
        }
        std::println("{}", inp.size());
        {
            decltype(inp) tmp;
            tmp.insert(tmp.begin(), inp.begin()+offset, inp.end());
            inp = std::move(tmp);
        }
        omp_set_num_threads(16);
        for (int _i = 0; _i < 100; _i++)
        {
            std::println("{}", _i);
            easyTimer<std::chrono::seconds> _(std::format("{} took ", _i));

            std::vector<dt> tmp(inp.size());
            #pragma omp parallel for
            for (size_t idx = offset+1; idx <= (inp.size()+offset); idx++)
            {
                dt p=0,n=0;
                auto itr = inp.begin();
                auto e = inp.end();
                itr += idx-1-offset;

                const auto idx2 = idx+idx;
                while (itr < e)
                {
                    auto span = std::span(itr, std::min(e, itr+idx));
                    p = std::accumulate(span.begin(), span.end(), p);
                    itr = itr+idx2;

                    if (itr >= e)
                    {
                        break;
                    }
                    span = std::span(itr, std::min(e, itr+idx));
                    n = std::accumulate(span.begin(), span.end(), n);
                    itr = itr+idx2;
                }
                tmp[idx-1-offset] = std::abs(p-n)%10;
            }
            inp = std::move(tmp);
        }
        print_arr(std::span(inp.begin(), inp.begin()+8),"");
    }
}