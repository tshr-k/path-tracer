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
    std::array<math::Interval, 3> intervals_;

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
