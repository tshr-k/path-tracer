#pragma once

#include "vector3.h"
#include "point.h"

class Ray {
private:
    const Point origin_;
    const Vector direction_;  // Not necessarily a unit vector
    
public:
    constexpr Ray(const Point& origin, const Vector& direction)
        : origin_{origin}, direction_{direction} {}

    constexpr Ray(const Point& from, const Point& to)
        : origin_{from}, direction_{to - from} {}
    constexpr const Point& origin() const {
        return origin_;
    }

    constexpr const Vector& direction() const {
        return direction_;
    }

    constexpr Point at(double t) const {
        return origin_ + direction_ * t;
    }

    constexpr Point operator()(double t) const {
        return at(t);
    }
};