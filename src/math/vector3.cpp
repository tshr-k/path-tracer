#include "vector3.h"

#include <cmath>
#include <ostream>

#include "math_utils.h"

Vector::Vector() : v_{0, 0, 0} {}
Vector::Vector(double v_x, double v_y, double v_z) : v_{v_x, v_y, v_z} {}

double Vector::x() const { return v_[0]; }
double Vector::y() const { return v_[1]; }
double Vector::z() const { return v_[2]; }

double Vector::operator[](std::size_t i) const { return v_[i]; }

Vector Vector::operator-() const { return Vector(-v_[0], -v_[1], -v_[2]); }

Vector& Vector::operator+=(const Vector& u) {
    v_[0] += u.x();
    v_[1] += u.y();
    v_[2] += u.z();
    return *this;
}

Vector& Vector::operator-=(const Vector& u) {
    v_[0] -= u.x();
    v_[1] -= u.y();
    v_[2] -= u.z();
    return *this;
}

Vector& Vector::operator*=(double mu) {
    v_[0] *= mu;
    v_[1] *= mu;
    v_[2] *= mu;
    return *this;
}

Vector& Vector::operator/=(double mu) {
    return *this *= 1.0/mu;
}

double Vector::length_sq() const {
    return dot(*this, *this);
}

double Vector::length() const {
    return std::sqrt(length_sq());
}


UnitVector::UnitVector(const Vector& v) : v_{v} {}
UnitVector::UnitVector(double v_x, double v_y, double v_z) : v_{v_x, v_y, v_z} {}

UnitVector UnitVector::from_vector(Vector v) {
    return UnitVector{v /= v.length()};
}

UnitVector UnitVector::random_sphere() {
    const double z = math::random(-1.0, 1.0);
    const double phi = math::random(0.0, 2.0 * math::pi);
    const double r = std::sqrt(1.0 - z * z);
    return UnitVector{r * std::cos(phi), r * std::sin(phi), z};
}

UnitVector UnitVector::random_hemisphere() {
    const double z = math::random(0.0, 1.0);
    const double phi = math::random(0.0, 2.0 * math::pi);
    const double r = std::sqrt(1.0 - z * z);
    return UnitVector{r * std::cos(phi), r * std::sin(phi), z};
}

UnitVector UnitVector::random_cosine_hemisphere() {
    const double r_sq = math::random();
    const double r = std::sqrt(r_sq);
    const double phi = math::random(0.0, 2.0 * math::pi);

    const double x = r * std::cos(phi);
    const double y = r * std::sin(phi);
    const double z = std::sqrt(1.0 - r_sq);

    return UnitVector{x, y, z};
}

UnitVector UnitVector::e_x() { return UnitVector(1, 0, 0); }
UnitVector UnitVector::e_y() { return UnitVector(0, 1, 0); }
UnitVector UnitVector::e_z() { return UnitVector(0, 0, 1); }

UnitVector::operator Vector() const { return v_; }

double UnitVector::x() const { return v_.x(); }
double UnitVector::y() const { return v_.y(); }
double UnitVector::z() const { return v_.z(); }

UnitVector UnitVector::operator-() const {
    return UnitVector{-v_};
}

UnitVector unit_vector(const Vector& v) {
    return UnitVector::from_vector(v);
}

UnitVector unit_vector(double v_x, double v_y, double v_z) {
    return UnitVector::from_vector(Vector(v_x, v_y, v_z));
}

double length_sq(const Vector& v) {
    return v.length_sq();
}

double length(const Vector& v) {
    return v.length();
}

std::ostream& operator<<(std::ostream& out, const Vector& v) {
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

Vector operator+(Vector v, const Vector& u) {
    return v += u;
}

Vector operator-(Vector v, const Vector& u) {
    return v -= u;
}

Vector operator*(Vector v, double mu) {
    return v *= mu;
}

Vector operator*(double mu, const Vector& v) {
    return v * mu;
}

Vector operator/(Vector v, double mu) {
    return v /= mu;
}

double dot(const Vector& v, const Vector& u) {
    return v.x()*u.x() + v.y()*u.y() + v.z()*u.z();
}

Vector cross(const Vector& u, const Vector& v) {
    return Vector(u.y() * v.z() - u.z() * v.y(),
                  u.z() * v.x() - u.x() * v.z(),
                  u.x() * v.y() - u.y() * v.x());
}

namespace math {
    bool almost_zero(const Vector& v) {
        return almost_zero(v.x()) && almost_zero(v.y()) && almost_zero(v.z());
    }

    bool almost_equal(const Vector& u, const Vector& v) {
        return almost_zero(v - u);
    }
}
