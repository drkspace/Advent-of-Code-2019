//
// Created by daniel on 12/31/24.
//

#ifndef FRACTION_H
#define FRACTION_H
#include <numeric>
#include <type_traits>


template <typename T>
struct Fraction
{
    // I don't this is the modern way of making sure it's an int
    static_assert(std::is_integral_v<T>);
    T num = 0;
    T denom = 1;

    Fraction() = default;
    Fraction(const T& num, const T& denom): num(num), denom(denom){};

    Fraction<T> reduce()
    {
        auto gcd = std::gcd(num, denom);
        num = num/gcd;
        denom = denom/gcd;
        return *this;
    }

    [[nodiscard]] Fraction<T> operator+(const Fraction<T>& other) const
    {
        auto new_denom = std::lcm(this->denom, other.denom);
        return Fraction(this->num*(new_denom/this->denom) + other.num*(new_denom/other.denom), new_denom);

    }

    [[nodiscard]] Fraction<T> operator*(const Fraction<T>& other) const
    {
        return Fraction(this->num*other.num, this->denom*other.denom);
    }

    [[nodiscard]] Fraction<T> operator*(const T& other) const
    {
        return Fraction(this->num*other, this->denom);
    }
    [[nodiscard]] Fraction<T> operator/(const T& other) const
    {
        return Fraction(this->num, this->denom*other);
    }

    template <typename OT=T>
    [[nodiscard]] OT value()const
    {
        return static_cast<OT>(this->num)/static_cast<OT>(this->denom);
    }
};

#endif //FRACTION_H
