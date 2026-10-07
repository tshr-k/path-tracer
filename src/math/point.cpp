#include "point.h"

#include <ostream>

#include "math_utils.h"

Point::Point() : r_{0, 0, 0} {}
Point::Point(double x, double y, double z) : r_{x, y, z} {}
Point::Point(const Vector& u) : r_{u} {}

const Vector& Point::r() const { return r_; }

double Point::x() const { return r_[0]; }
double Point::y() const { return r_[1]; }
double Point::z() const { return r_[2]; }

double Point::operator[](std::size_t i) const { return r_[i]; }

Point& Point::operator+=(const Vector& u) {
    r_ += u;
    return *this;
}

Point& Point::operator-=(const Vector& u) {
    r_ -= u;
    return *this;
}

Vector Point::operator-(const Point& subtrahend) const {
    return this->r() - subtrahend.r();
}

UnitVector Point::direction_to(const Point& target) const {
    return unit_vector(target - *this);
}

std::ostream& operator<<(std::ostream& out, const Point& p) {
    return out << p.r();
}

Point midpoint(const Point& p, const Point& q) {
    return Point(0.5 * (p.r() + q.r()));
}

Point operator+(Point p, const Vector& u) {
    return p += u;
}

namespace math {
    bool almost_zero(const Point& p) {
        return almost_zero(p.r());
    }

    bool almost_equal(const Point& p, const Point& q) {
        return almost_zero(q - p);
    }
}
