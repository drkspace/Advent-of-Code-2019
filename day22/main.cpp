
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

enum class MOVES
{
    DITNS,CUT,DWI,
};

std::vector<std::pair<MOVES, dt>> getInput(const std::string& fp){
    auto lines = read_file(fp);
    std::vector<std::pair<MOVES, dt>> out;

    for (const auto& [i, l]: enumerate(lines))
    {
        if (l.empty())
        {
            break;
        }

        if (l.contains("deal into new stack"))
        {
            out.emplace_back(MOVES::DITNS, 0);
        }else if (l.contains("cut "))
        {
            out.emplace_back(MOVES::CUT, std::stol(split(l, ' ')[1]));
        }else if (l.contains("deal with increment "))
        {
            out.emplace_back(MOVES::DWI, std::stol(split(l, ' ')[3]));
        }else
        {
            std::println("INVALID: {}", l);
        }

    }
    return out;

}

[[nodiscard]] dt powmod(__int128_t r, unsigned long long e, const dt m)
{
    __int128_t result = 1;
    r = pmod<__int128_t>(r, m);
    e %= m;
    while (e > 0)
    {
        if ((e & 1) == 1)
        {
            result = (result*r) % m;
        }
        e = e>>1;
        r = (r*r) % m;
    }

    return static_cast<dt>(result);
}

int main(const int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid args" << std::endl;
        return 1;
    }

    {
        constexpr size_t N_CARDS = 10007;
        std::array<int, N_CARDS> deck{};
        std::iota(deck.begin(), deck.end(), 0);

        auto moves = getInput(argv[1]);
        for (auto&[m, n]: moves)
        {
            switch (m)
            {
            case MOVES::DITNS:
                std::println("Reversing");
                std::reverse(deck.begin(), deck.end());
                break;
            case MOVES::DWI:
                std::println("DWI {}", n);
                {
                size_t table_idx = 0;
                decltype(deck) table{};
                for (size_t i = 0; i<N_CARDS; i++)
                {
                    table[table_idx] = deck[i];
                    table_idx = pmod(table_idx+n, N_CARDS);
                }
                deck = table;
                }
                break;
            case MOVES::CUT:
                {
                    std::println("CUT {}", n);
                    if (n < 0)
                    {
                        n = N_CARDS+n;
                    }
                    std::vector<int> tmp;
                    tmp.insert(tmp.begin(), deck.begin(), deck.begin()+n);
                    for (size_t i = 0; i<N_CARDS-n; i++)
                    {
                        deck[i] = deck[i+n];
                    }
                    for (size_t i = N_CARDS-n; i<N_CARDS; i++)
                    {
                        deck[i] = tmp[i-(N_CARDS-n)];
                    }
                }
                break;
            }

        }
        // print_arr(std::span(deck));

        if constexpr (N_CARDS > 2019)
        {
            std::println("Part 1: {}", std::distance(deck.begin(), std::ranges::find(deck, 2019)));
        }
    }


    {
        constexpr dt N_CARDS = 119315717514047; // PRIME
        constexpr dt N_APPLY = 101741582076661; // PRIME
        /* Since they are both prime,
         *
         */
        auto moves = getInput(argv[1]);
        dt b = 0;
        dt m = 1;
        for (auto&[mo, n]: moves)
        {
            dt this_b{}, this_m{};

            switch (mo)
            {
            case MOVES::DITNS:
                this_b = N_CARDS-1;
                this_m = -1;
                break;
            case MOVES::CUT:
                if (n < 0)
                {
                    n = N_CARDS+n;
                }
                this_b = -1*n;
                this_m = 1;
                break;
            case MOVES::DWI:
                this_b = 0;
                this_m = n;
                break;
            }
            m = pmod(this_m*m, N_CARDS);
            b = pmod(b*this_m + this_b, N_CARDS);
        }
        // std::println("X_{{n+1}} = {} + {}*X_n", b, m);

        /* The new generating function (I don't think that's the proper name for this)
         * is stored in `b` and `m` and takes the form of $X_{n+1} = b+m\timesX_n (mod N)$.
         * To calculate out the multiple applications, we just need to apply the function
         * multiple times. After applying it N_APPLIES ($M$) times:
         * $b\prime = b + bm + bm^2 + bm^3 + \ldots + bm^{M-1}$
         * $m\prime = m^M$
         *
         * To calculate `m`, you just need to implement a pow function that takes
         * the modulus each step (to not overflow). To calculate `b`, we see it's a power series
         * and the formula for that is $S_n = a\left(\dfrac{1-r^{n+1}}{1-r}\right)$ (see https://en.wikipedia.org/wiki/Geometric_series#Convergence_of_the_series_and_its_proof).
         * For our case, $a=b$, $r=m$, and $n=M$. We can also multiply the top and bottom of the fraction
         * by $-1$ to make sure they are positive.
         *
         * Now we have $X_{n+1} = b\prime + m\prime X_n$. For part 2, we are given $N_{n+1}=2020$, so we
         * need to solve for $X_n$. We can just subtract $b\prime$ over, but to divide $m\prime$, we need to find
         * its multiplicative inverse, which, using Fermat's little theorem and noticing the number of cards is prime
         * we know the inverse is $m\prime^{N-2} (mod N)$, so we multiply that on both sides to find
         * $m\prime^{N-2}\left(X_{n+1}-b\prime\right) = X_n$. We then just have to calculate out.
         */


        __int128_t tmp = powmod(m, N_APPLY, N_CARDS)-1;
        tmp = b * tmp;

        // I know it will be positive, so it's ok using %
        tmp = tmp % N_CARDS;
        tmp = tmp * powmod(m-1, N_CARDS-2, N_CARDS);
        b = tmp % N_CARDS;

        m = powmod(m, N_APPLY, N_CARDS);
        // std::println("X_{{n+1}} = {} + {}*X_n", b, m);
        if constexpr (N_CARDS > 2019)
        {
            __int128_t res = pmod(2020-b, N_CARDS);
            res = res * powmod(m, N_CARDS-2, N_CARDS);
            res = res % N_CARDS;
            std::println("Part 2: {}", res);
            std::println("{}", (res*m+b) % N_CARDS);
        }
    }

}