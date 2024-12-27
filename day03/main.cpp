//
// Created by daniel on 12/27/24.
//

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../utils/utils.h"
#include "../utils/point.h"

using P = Point2d<int>;

struct Path
{
    enum Dir
    {
        U,D,R,L
    };

    Dir dir;
    int len;

    explicit Path(const std::string& s)
    {
        dir = ctodir(s[0]);
        len = std::stoi(s.substr(1));
    };

    static Dir ctodir(const char c)
    {
        switch (c)
        {
        case 'U':
            return Dir::U;
        case 'D':
            return Dir::D;
        case 'R':
            return Dir::R;
        case 'L':
            return Dir::L;
        default:
            return Dir::U;
        }
    };
};

auto getInput(const std::string& fp){
    auto lines = read_file(fp);
    std::vector<std::vector<Path>> out;
    for (const auto& l: lines)
    {
        if (l.empty())
        {
            break;
        }
        std::vector<Path> tmp;
        for (const auto& ele: split(l, ','))
        {
            tmp.emplace_back(ele);
        }
        out.emplace_back(tmp);

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
        std::unordered_set<P> v1, v2;
        // for(const auto& r: inp){
        //   print_arr(std::span{r});
        // }

        auto p1 = inp.at(0);
        auto p2 = inp.at(1);

        const P o_pos(0,0);
        auto cur_pos = o_pos;
        for (const auto& ele: p1)
        {
            P delta = {};
            switch (ele.dir)
            {
            case Path::D:
                delta = {0,-1};
                break;
            case Path::U:
                delta = {0,1};
                break;
            case Path::L:
                delta = {-1,0};
                break;
            case Path::R:
                delta = {1,0};
                break;
            }
            for (int i = 0; i< ele.len; i++)
            {
                cur_pos += delta;
                v1.emplace(cur_pos);
            }

        }
        cur_pos = o_pos;
        for (const auto& ele: p2)
        {
            P delta = {};
            switch (ele.dir)
            {
            case Path::D:
                delta = {0,-1};
                break;
            case Path::U:
                delta = {0,1};
                break;
            case Path::L:
                delta = {-1,0};
                break;
            case Path::R:
                delta = {1,0};
                break;
            }
            for (int i = 0; i< ele.len; i++)
            {
                cur_pos += delta;
                v2.emplace(cur_pos);
            }

        }
        P best;
        int best_dist = std::numeric_limits<int>::max();
        for (const auto& ele: v1)
        {
            if (v2.contains(ele))
            {
                if (ele.taxicab(o_pos) < best_dist)
                {
                    best = ele;
                    best_dist = ele.taxicab(o_pos);
                }
            }
        }

        std::println("Part 1: {}", best_dist);
    }

    {
        auto inp = getInput(argv[1]);
        std::unordered_map<P, int> v1, v2;
        // for(const auto& r: inp){
        //   print_arr(std::span{r});
        // }

        auto p1 = inp.at(0);
        auto p2 = inp.at(1);

        const P o_pos(0,0);
        auto cur_pos = o_pos;
        int n_steps = 1;
        for (const auto& ele: p1)
        {
            P delta = {};
            switch (ele.dir)
            {
            case Path::D:
                delta = {0,-1};
                break;
            case Path::U:
                delta = {0,1};
                break;
            case Path::L:
                delta = {-1,0};
                break;
            case Path::R:
                delta = {1,0};
                break;
            }
            for (int i = 0; i< ele.len; i++)
            {
                cur_pos += delta;
                v1[cur_pos] = n_steps;
                n_steps++;
            }

        }
        cur_pos = o_pos;
        n_steps = 1;
        for (const auto& ele: p2)
        {
            P delta = {};
            switch (ele.dir)
            {
            case Path::D:
                delta = {0,-1};
                break;
            case Path::U:
                delta = {0,1};
                break;
            case Path::L:
                delta = {-1,0};
                break;
            case Path::R:
                delta = {1,0};
                break;
            }
            for (int i = 0; i< ele.len; i++)
            {
                cur_pos += delta;
                v2[cur_pos] = n_steps;
                n_steps++;
            }

        }

        int best_dist = std::numeric_limits<int>::max();
        for (const auto& [p, l]: v1)
        {
            if (const auto f = v2.find(p); f != v2.end())
            {
                if (l+f->second < best_dist)
                {
                    best_dist = l+f->second;
                }
            }
        }

        std::println("Part 2: {}", best_dist);
    }

}