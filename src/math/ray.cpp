#include "ray.h"

void Ray::cache_slab_data() {
    origin_4_ = {origin_.x(), origin_.y(), origin_.z(), 0.0};
    inv_direction_4_ = {1.0 / direction_.x(), 1.0 / direction_.y(), 1.0 / direction_.z(), 1.0};
}

Ray::Ray(const Point& origin, const Vector& direction)
    : origin_{origin}, direction_{direction} {
    cache_slab_data();
}

Ray::Ray(const Point& from, const Point& to)
    : origin_{from}, direction_{to - from} {
    cache_slab_data();
}

const Point& Ray::origin() const {
    return origin_;
}

const Vector& Ray::direction() const {
    return direction_;
}

const std::array<double, 4>& Ray::origin_4() const {
    return origin_4_;
}

const std::array<double, 4>& Ray::inv_direction_4() const {
    return inv_direction_4_;
}

Point Ray::at(double t) const {
    return origin_ + direction_ * t;
}

Point Ray::operator()(double t) const {
    return at(t);
}
