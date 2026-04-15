#pragma once

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>

#include "math_utils.h"
#include "point.h"
#include "ray.h"

class AABB {
private:
    std::array<math::Interval, 3> intervals_;

public:
    constexpr math::Interval x() const { return intervals_[0]; }
    constexpr math::Interval y() const { return intervals_[1]; }
    constexpr math::Interval z() const { return intervals_[2]; }

    constexpr math::Interval operator[](std::size_t i) const { return intervals_[i]; }

    constexpr AABB() : intervals_{} {}

    constexpr AABB(math::Interval x, math::Interval y, math::Interval z)
        : intervals_{x.ensure_size(math::bbox_eps), y.ensure_size(math::bbox_eps), z.ensure_size(math::bbox_eps)} {}

    constexpr AABB(std::initializer_list<Point> points) {
        if (points.size() == 0) {
            intervals_ = {math::Interval(), math::Interval(), math::Interval()};
            return;
        }

        math::Interval x_interval{};  // Empty interval with min = +inf and max = -inf
        math::Interval y_interval{};
        math::Interval z_interval{};

        for (const Point& p : points) {
            x_interval.max = std::max(x_interval.max, p.x());
            x_interval.min = std::min(x_interval.min, p.x());
            y_interval.max = std::max(y_interval.max, p.y());
            y_interval.min = std::min(y_interval.min, p.y());
            z_interval.max = std::max(z_interval.max, p.z());
            z_interval.min = std::min(z_interval.min, p.z());
        }

        intervals_ = {
            math::Interval{x_interval.min, x_interval.max}.ensure_size(math::bbox_eps),
            math::Interval{y_interval.min, y_interval.max}.ensure_size(math::bbox_eps),
            math::Interval{z_interval.min, z_interval.max}.ensure_size(math::bbox_eps)
        };
    }

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

    bool hit(const Ray& ray, math::Interval t_range) const {  // Full t_range required since there's no hit_record
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


inline std::ostream& operator<<(std::ostream& os, const AABB& bbox) {
    return os << "AABB[x" << bbox.x() << ", y" << bbox.y() << ", z" << bbox.z() << "]";
}