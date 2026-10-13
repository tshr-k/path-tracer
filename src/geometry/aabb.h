#pragma once

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iosfwd>

#include "math/math_utils.h"

class Point;
class Ray;

class AABB {
private:
    // Axis bounds are stored in separate min/max arrays, padded to 4 lanes so each loads straight into one
    // __m256d in hit(). The padding lane is (-inf, +inf), which doesn't constrain the slab test
    std::array<double, 4> min_;
    std::array<double, 4> max_;

    void set_axes(math::Interval x, math::Interval y, math::Interval z);

public:
    AABB();
    AABB(math::Interval x, math::Interval y, math::Interval z);
    AABB(std::initializer_list<Point> points);

    math::Interval x() const;
    math::Interval y() const;
    math::Interval z() const;

    math::Interval operator[](std::size_t i) const;

    static AABB merge(AABB a, AABB b);

    std::size_t longest_axis();

    bool hit(const Ray& ray, math::Interval t_range) const;  // Full t_range required since there's no hit_record
};


std::ostream& operator<<(std::ostream& os, const AABB& bbox);
