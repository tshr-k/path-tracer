#pragma once

#include <array>

#include "vector3.h"
#include "point.h"

class Ray {
private:
    const Point origin_;
    const Vector direction_;  // Not necessarily a unit vector

    // Cached for AABB::hit(), padded to 4 lanes for AVX
    std::array<double, 4> origin_4_;
    std::array<double, 4> inv_direction_4_;

    void cache_slab_data();

public:
    Ray(const Point& origin, const Vector& direction);
    Ray(const Point& from, const Point& to);

    const Point& origin() const;
    const Vector& direction() const;

    const std::array<double, 4>& origin_4() const;
    const std::array<double, 4>& inv_direction_4() const;

    Point at(double t) const;
    Point operator()(double t) const;
};
