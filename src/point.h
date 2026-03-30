#pragma once

#include <cstddef>
#include <ostream>

#include "vector3.h"
#include "math_utils.h"

class Point {
private:
    Vector r_;
    
public:
    constexpr Point() : r_{0, 0, 0} {}
    constexpr Point(double x, double y, double z) : r_{x, y, z} {}
    explicit constexpr Point(const Vector& u) : r_{u} {}

    constexpr const Vector& r() const { return r_; }

    constexpr double x() const { return r_[0]; }
    constexpr double y() const { return r_[1]; }
    constexpr double z() const { return r_[2]; }

    constexpr double operator[](std::size_t i) const { return r_[i]; }
    // constexpr double& operator[](std::size_t i) { return r_[i]; }

    constexpr Point& operator+=(const Vector& u) {
        r_ += u;
        return *this;
    }

    constexpr Point& operator-=(const Vector& u) {
        r_ -= u;
        return *this;
    }

    constexpr Vector operator-(const Point& subtrahend) const {
        return this->r() - subtrahend.r();
    }

    UnitVector direction_to(const Point& target) const {
        return unit_vector(target - *this);
    }
};

inline std::ostream& operator<<(std::ostream& out, const Point& p) {
    return out << p.r();
}

constexpr Point midpoint(const Point& p, const Point& q) {
    return Point(0.5 * (p.r() + q.r()));
}

constexpr Point operator+(Point p, const Vector& u) {
    return p += u;
}

namespace math {
    [[nodiscard]] inline bool almost_zero(const Point& p) {
        return almost_zero(p.r());
    }

    [[nodiscard]] inline bool almost_equal(const Point& p, const Point& q) {
        return almost_zero(q - p);
    }
}