#include "ray.h"

Ray::Ray(const Point& origin, const Vector& direction)
    : origin_{origin}, direction_{direction} {}

Ray::Ray(const Point& from, const Point& to)
    : origin_{from}, direction_{to - from} {}

const Point& Ray::origin() const {
    return origin_;
}

const Vector& Ray::direction() const {
    return direction_;
}

Point Ray::at(double t) const {
    return origin_ + direction_ * t;
}

Point Ray::operator()(double t) const {
    return at(t);
}
