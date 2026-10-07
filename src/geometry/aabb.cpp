#include "aabb.h"

#include <algorithm>
#include <cmath>
#include <ostream>

#include "math/point.h"
#include "math/ray.h"

AABB::AABB() : intervals_{} {}

AABB::AABB(math::Interval x, math::Interval y, math::Interval z)
    : intervals_{x.ensure_size(math::bbox_eps), y.ensure_size(math::bbox_eps), z.ensure_size(math::bbox_eps)} {}

AABB::AABB(std::initializer_list<Point> points) {
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

math::Interval AABB::x() const { return intervals_[0]; }
math::Interval AABB::y() const { return intervals_[1]; }
math::Interval AABB::z() const { return intervals_[2]; }

math::Interval AABB::operator[](std::size_t i) const { return intervals_[i]; }

AABB AABB::merge(AABB a, AABB b) {
    return AABB{
        math::Interval::merge(a.x(), b.x()),
        math::Interval::merge(a.y(), b.y()),
        math::Interval::merge(a.z(), b.z())
    };
}

std::size_t AABB::longest_axis() {
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

bool AABB::hit(const Ray& ray, math::Interval t_range) const {
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


std::ostream& operator<<(std::ostream& os, const AABB& bbox) {
    return os << "AABB[x" << bbox.x() << ", y" << bbox.y() << ", z" << bbox.z() << "]";
}
