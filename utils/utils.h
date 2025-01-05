//
// Created by daniel on 12/18/24.
//

#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <ranges>
#include <format>
#include <print>
#include "point.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

inline const auto& enumerate = std::ranges::views::enumerate;


inline auto split(const std::string& inp, const char delim, const bool remove_empty = false) {
    std::string tmp;
    std::vector<std::string> r;
    std::istringstream s(inp);
    while (std::getline(s, tmp, delim)) {
        if (remove_empty && tmp.empty())
        {
            continue;
        }
        r.push_back(tmp);
    }
    r.shrink_to_fit();
    return r;
};

inline std::vector<std::string> read_file(const std::string& fp){
    std::vector<std::string> out;

    auto file = std::ifstream(fp);
    while (file)
    {
        std::string tmp;
        std::getline(file, tmp);
        out.emplace_back(tmp);
    }
    out.shrink_to_fit();
    return out;

}

struct hash_tuple {
    // copied from https://www.geeksforgeeks.org/how-to-create-an-unordered_map-of-pairs-in-c/
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const
    {
        // Hash the first element
        const size_t hash1 = std::hash<T1>{}(p.first);
        // Hash the second element
        const size_t hash2 = std::hash<T2>{}(p.second);
        // Combine the two hash values
        return hash1
               ^ (hash2 + 0x9e3779b9 + (hash1 << 6)
                  + (hash1 >> 2));
    }

    template <class T1, class T2, class T3>
    size_t operator()(const std::tuple<T1, T2, T3>& p) const
    {
        return hash_tuple()(std::make_pair(hash_tuple()(std::make_pair(std::get<0>(p), std::get<1>(p))), std::get<2>(p)));
    }

    template <class T1, class T2, class T3, class T4>
    size_t operator()(const std::tuple<T1, T2, T3, T4>& p) const
    {
        return hash_tuple()(std::make_pair(hash_tuple()(std::make_pair(hash_tuple()(std::make_pair(std::get<0>(p), std::get<1>(p))), std::get<2>(p))), std::get<3>(p)));
    }

};

template<typename T>
struct std::hash<std::set<T>>
{
    std::size_t operator()(std::set<T> const& s) const {
        std::size_t seed = s.size();
        for(auto x : s) {
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = (x >> 16) ^ x;
            seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

template <class T1, class T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p) {
    os << "(" << p.first << "," << p.second << ")";
    return os;
}

enum class Dir{
    UP,DOWN,LEFT,RIGHT
};

template <typename T,typename U>
std::pair<T,U> operator+(const std::pair<T,U> & l,const std::pair<T,U> & r) {
    return {l.first+r.first,l.second+r.second};
}

inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

template<typename R>
void print_arr(R&& arr, const std::string sep = ",")
{
    for (const auto& ele: arr)
    {
        std::print("{}{}", ele, sep);
    }
    std::println("");
}

// template <typename T, size_t N>
// void print_arr(std::span<T, N> arr, const std::string sep = ",")
// {
//     for (const auto& ele: arr)
//     {
//         std::print("{}{}", ele, sep);
//     }
//     std::println("");
// }

template <typename T>
[[nodiscard]] inline T pmod(const T& a, const T& b)
{
    if constexpr (std::is_integral_v<T>)
    {
        return (b + (a % b)) % b;
    } else
    {
        return fmod(b + fmod(a, b), b);
    }
}

template <typename T>
[[nodiscard]] constexpr T sign(const T& v)
{
    return (T(0) < v) - (v < T(0));
}

#endif //UTILS_H
