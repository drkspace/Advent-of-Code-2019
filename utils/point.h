//
// Created by daniel on 12/22/24.
//

#pragma once
#ifndef POINT_H
#define POINT_H

#include <cmath>

template <typename T>
struct Point2d
{
    T x;
    T y;

    Point2d() = default;
    Point2d(T x, T y): x(x), y(y){};
    Point2d(const Point2d& other) = default;

    [[nodiscard]] bool operator==(const Point2d&) const = default;
    [[nodiscard]] auto operator<=>(const Point2d& other) const = default;
    [[nodiscard]] Point2d operator+(const Point2d& other) const
    {
        return {this->x+other.x, this->y+other.y};
    }
    [[nodiscard]] Point2d operator-(const Point2d& other) const
    {
        return {this->x-other.x, this->y-other.y};
    }
    [[nodiscard]] Point2d operator*(const Point2d& other) const
    {
        return {this->x*other.x, this->y*other.y};
    }
    [[nodiscard]] Point2d operator*(const T& other) const
    {
        return {this->x*other, this->y*other};
    }
    [[nodiscard]] Point2d operator/(const Point2d& other) const
    {
        return {this->x/other.x, this->y/other.y};
    }
    [[nodiscard]] Point2d operator/(const T& other) const
    {
        return {this->x/other, this->y/other};
    }

    [[nodiscard]] T radicand() const
    {
        return (this->x*this->x) + (this->y*this->y);
    }

    [[nodiscard]] T abs() const
    {
        return std::sqrt(this->radicand());
    }

    [[nodiscard]] T taxicab(const Point2d& other = {0,0}) const
    {
        return std::abs(this->x-other.x)+std::abs(this->y-other.y);
    }

    Point2d<T>& operator+=(const Point2d& other)
    {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }

    [[nodiscard]] float angle() const
    {
        return std::atan2(y,x);
    }
};

template<typename T>
struct std::hash<Point2d<T>>
{
    std::size_t operator()(const Point2d<T>& p) const noexcept
    {
        const std::size_t h1 = std::hash<T>{}(p.x);
        const std::size_t h2 = std::hash<T>{}(p.y);
        return h1 ^ (h2 << 1);
    }
};

template<typename T>
struct std::formatter<Point2d<T>> : std::formatter<std::string> {
    auto format(Point2d<T> p, format_context& ctx) const {
        return formatter<string>::format(
          std::format("({}, {})", p.x, p.y), ctx);
    }
};

template <typename T>
struct Point3d
{
    T x;
    T y;
    T z;

    Point3d() = default;
    Point3d(T x, T y, T z): x(x), y(y), z(z) {};
    Point3d(const Point3d& other) = default;
    explicit Point3d(const Point2d<T>& other): x(other.x), y(other.y), z(0) {};

    [[nodiscard]] bool operator==(const Point3d&) const = default;
    [[nodiscard]] auto operator<=>(const Point3d& other) const = default;
    [[nodiscard]] Point3d operator+(const Point3d& other) const
    {
        return {this->x+other.x, this->y+other.y, this->z+other.z};
    }
    [[nodiscard]] Point3d operator-(const Point3d& other) const
    {
        return {this->x-other.x, this->y-other.y, this->z-other.z};
    }
    [[nodiscard]] Point3d operator*(const Point3d& other) const
    {
        return {this->x*other.x, this->y*other.y, this->z*other.z};
    }
    [[nodiscard]] Point3d operator*(const T& other) const
    {
        return {this->x*other, this->y*other, this->z*other};
    }
    [[nodiscard]] Point3d operator/(const Point3d& other) const
    {
        return {this->x*other.x, this->y*other.y, this->z/other.z};
    }
    [[nodiscard]] Point3d operator/(const T& other) const
    {
        return {this->x/other, this->y/other, this->z/other};
    }
    [[nodiscard]] T radicand() const
    {
        return (this->x*this->x) + (this->y*this->y) + (this->z*this->z);
    }

    [[nodiscard]] T abs() const
    {
        return std::sqrt(this->radicand());
    }

    Point3d<T>& operator+=(const Point3d& other)
    {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        return *this;
    }

    [[nodiscard]] T sum_abs() const
    {
        return std::abs(this->x)+std::abs(this->y)+std::abs(this->z);
    }

};

template<typename T>
struct std::hash<Point3d<T>>
{
    std::size_t operator()(const Point3d<T>& p) const noexcept
    {
        const std::size_t h1 = std::hash<T>{}(p.x);
        const std::size_t h2 = std::hash<T>{}(p.y);
        const std::size_t h3 = std::hash<T>{}(p.z);
        return (h1 ^ (h2 << 1)) ^ (h3 << 2);
    }
};

template<typename T>
struct std::formatter<Point3d<T>> : std::formatter<std::string> {
    auto format(Point3d<T> p, format_context& ctx) const {
        return formatter<string>::format(
          std::format("({}, {}, {})", p.x, p.y, p.z), ctx);
    }
};

template <typename T>
struct Point4d
{
    T x;
    T y;
    T z;
    T u;

    Point4d() = default;
    Point4d(T x, T y, T z, T u): x(x), y(y), z(z), u(u) {};
    Point4d(const Point4d& other) = default;
    explicit Point4d(const Point2d<T>& other): x(other.x), y(other.y), z(0), u(0) {};
    explicit Point4d(const Point3d<T>& other): x(other.x), y(other.y), z(other.z), u(0) {};

    [[nodiscard]] bool operator==(const Point4d&) const = default;
    [[nodiscard]] auto operator<=>(const Point4d& other) const = default;
    [[nodiscard]] Point4d operator+(const Point4d& other) const
    {
        return {this->x+other.x, this->y+other.y, this->z+other.z, this->u+other.u};
    }
    [[nodiscard]] Point4d operator-(const Point4d& other) const
    {
        return {this->x-other.x, this->y-other.y, this->z-other.z, this->u-other.u};
    }
    [[nodiscard]] Point4d operator*(const Point4d& other) const
    {
        return {this->x*other.x, this->y*other.y, this->z*other.z, this->u*other.u};
    }
    [[nodiscard]] Point4d operator*(const T& other) const
    {
        return {this->x*other, this->y*other, this->z*other, this->u*other};
    }
    [[nodiscard]] Point4d operator/(const Point4d& other) const
    {
        return {this->x*other.x, this->y*other.y, this->z/other.z, this->u/other.u};
    }
    [[nodiscard]] Point4d operator/(const T& other) const
    {
        return {this->x/other, this->y/other, this->z/other, this->u/other};
    }
    [[nodiscard]] T radicand() const
    {
        return (this->x*this->x) + (this->y*this->y) + (this->z*this->z) + (this->u+this*u);
    }

    [[nodiscard]] T abs() const
    {
        return std::sqrt(this->radicand());
    }
};

template<typename T>
struct std::hash<Point4d<T>>
{
    std::size_t operator()(const Point4d<T>& p) const noexcept
    {
        const std::size_t h1 = std::hash<T>{}(p.x);
        const std::size_t h2 = std::hash<T>{}(p.y);
        const std::size_t h3 = std::hash<T>{}(p.z);
        const std::size_t h4 = std::hash<T>{}(p.u);
        return ((h1 ^ (h2 << 1)) ^ (h3 << 2)) ^ (h4 >> 1);
    }
};

template<typename T>
struct std::formatter<Point4d<T>> : std::formatter<std::string> {
    auto format(Point3d<T> p, format_context& ctx) const {
        return formatter<string>::format(
          std::format("({}, {}, {}, {})", p.x, p.y, p.z, p.u), ctx);
    }
};

#endif //POINT_H
