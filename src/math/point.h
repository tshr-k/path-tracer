#pragma once

#include <cstddef>
#include <iosfwd>

#include "vector3.h"

class Point {
private:
    Vector r_;

public:
    Point();
    Point(double x, double y, double z);
    explicit Point(const Vector& u);

    const Vector& r() const;

    double x() const;
    double y() const;
    double z() const;

    double operator[](std::size_t i) const;

    Point& operator+=(const Vector& u);
    Point& operator-=(const Vector& u);

    Vector operator-(const Point& subtrahend) const;

    UnitVector direction_to(const Point& target) const;
};

std::ostream& operator<<(std::ostream& out, const Point& p);

Point midpoint(const Point& p, const Point& q);
Point operator+(Point p, const Vector& u);

namespace math {
    [[nodiscard]] bool almost_zero(const Point& p);
    [[nodiscard]] bool almost_equal(const Point& p, const Point& q);
}
