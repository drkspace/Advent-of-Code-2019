

#include <iostream>
#include <ostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "../utils/utils.h"



auto getInput(const std::string& fp){
    auto lines = read_file(fp);
    std::vector<std::pair<std::string, std::string>> out;
    for (const auto& l: lines)
    {
        if (l.empty())
        {
            break;
        }

        auto tmp = split(l, ')');
        out.emplace_back(tmp.at(0), tmp.at(1));
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

    const std::string START = "COM";

    {
        const auto inp = getInput(argv[1]);
        std::unordered_map<std::string, std::unordered_set<std::string>> adj_f;
        std::unordered_map<std::string, std::string> adj_b;
        std::unordered_map<std::string, int> cache;
        std::stack<std::string> todo;
        for (const auto& [mb, ob]: inp)
        {
            adj_f[mb].emplace(ob);
            adj_b[ob] = mb;
        }

        cache[START] = 0;
        for (const auto& ele: adj_f[START])
        {
            todo.push(ele);
        }
        while (!todo.empty())
        {
            const auto ele = todo.top();
            todo.pop();
            cache[ele] = cache[adj_b[ele]] + 1;
            for (const auto& ele2: adj_f[ele])
            {
                todo.push(ele2);
            }
        }

        int sum = 0;
        for (const auto& v : cache | std::views::values)
        {
            sum += v;
        }
        std::println("Part 1: {}", sum);
    }

    {
        const auto inp = getInput(argv[1]);
        std::unordered_map<std::string, std::unordered_set<std::string>> adj_f;
        std::unordered_map<std::string, std::string> adj_b;
        std::unordered_map<std::string, int> cache;
        std::stack<std::string> todo;
        for (const auto& [mb, ob]: inp)
        {
            adj_f[mb].emplace(ob);
            adj_b[ob] = mb;
        }
        const std::string YOU = "YOU";
        const std::string SANTA = "SAN";

        std::vector<std::pair<std::string, int>> you_parents;
        std::unordered_map<std::string, int> santa_parents;
        std::string cur = YOU;
        int c = 0;
        while (cur != START)
        {
            cur = adj_b[cur];
            you_parents.emplace_back(cur, c);
            c++;
        }
        cur = SANTA;
        c = 0;
        while (cur != START)
        {
            cur = adj_b[cur];
            santa_parents[cur] = c;
            c++;
        }

        int dist = -1;
        for (const auto& yp: you_parents)
        {
            if (auto f = santa_parents.find(yp.first); f != santa_parents.end())
            {
                dist = yp.second + f->second;
                break;
            }
        }



        std::println("Part 2: {}", dist);
    }
}