#pragma once

#include <array>
#include <cstddef>

#include "math_utils.h"
#include "point.h"
#include "ray.h"

class AABB {
private:
    std::array<math::Interval, 3> intervals_;

public:
    constexpr AABB() {}

    constexpr math::Interval x() const { return intervals_[0]; }
    constexpr math::Interval y() const { return intervals_[1]; }
    constexpr math::Interval z() const { return intervals_[2]; }

    constexpr math::Interval operator[](std::size_t i) const { return intervals_[i]; }

    constexpr AABB(math::Interval x, math::Interval y, math::Interval z)
        : intervals_{x, y, z} {}

    constexpr AABB(const Point& p, const Point& q)
        : intervals_{
            math::Interval{std::fmin(p.x(), q.x()), std::fmax(p.x(), q.x())},
            math::Interval{std::fmin(p.y(), q.y()), std::fmax(p.y(), q.y())},
            math::Interval{std::fmin(p.z(), q.z()), std::fmax(p.z(), q.z())}
        } {}

    constexpr static AABB merge(AABB a, AABB b) {
        return AABB{
            math::Interval::merge(a.x(), b.x()),
            math::Interval::merge(a.y(), b.y()),
            math::Interval::merge(a.z(), b.z())
        };
    }

    constexpr std::size_t longest_axis() {
        std::size_t longest_axis = 0;
        int max_size = 0;
        for (std::size_t axis = 0; axis < 3; ++axis) {
            if (intervals_[axis].size() > max_size) {
                longest_axis = axis;
                max_size = intervals_[axis].size();
            }
        }
        return longest_axis;
    }

    bool hit(const Ray& ray, math::Interval t_range) const {
        for (int axis = 0; axis < 3; ++axis) {
            const double inv_dir = 1.0 / ray.direction()[axis];
            const double orig = ray.origin()[axis];

            double t_0 = (intervals_[axis].min - orig) * inv_dir;
            double t_1 = (intervals_[axis].max - orig) * inv_dir;

            t_range.min = std::fmax(std::fmin(t_0, t_1), t_range.min);
            t_range.max = std::fmin(std::fmax(t_0, t_1), t_range.max);

            if (t_range.max <= t_range.min) {
                return false;
            }
        }
        return true;
    }
};