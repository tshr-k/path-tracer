#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <ostream>

#include "math_utils.h"


class Vector;
constexpr double dot(const Vector& v, const Vector& u);

class Vector {
private:
    std::array<double, 3> v_;

public:
    constexpr Vector() : v_{0, 0, 0} {}
    constexpr Vector(double v_x, double v_y, double v_z) : v_{v_x, v_y, v_z} {}

    constexpr double x() const { return v_[0]; }
    constexpr double y() const { return v_[1]; }
    constexpr double z() const { return v_[2]; }

    constexpr double operator[](std::size_t i) const { return v_[i]; }
    // constexpr double& operator[](std::size_t i) { return v_[i]; }
    
    constexpr Vector operator-() const { return Vector(-v_[0], -v_[1], -v_[2]); }

    constexpr Vector& operator+=(const Vector& u) {
        v_[0] += u.x();
        v_[1] += u.y();
        v_[2] += u.z();
        return *this;
    }

    constexpr Vector& operator-=(const Vector& u) {
        v_[0] -= u.x();
        v_[1] -= u.y();
        v_[2] -= u.z();
        return *this;
    }

    constexpr Vector& operator*=(double mu) {
        v_[0] *= mu;
        v_[1] *= mu;
        v_[2] *= mu;
        return *this;
    }

    constexpr Vector& operator/=(double mu) {
        return *this *= 1.0/mu;
    }

    constexpr double length_sq() const {
        return dot(*this, *this);
    }

    double length() const {
        return std::sqrt(length_sq());
    }
};


struct ONB;

class UnitVector {
private:
    Vector v_;
    explicit constexpr UnitVector(const Vector& v) : v_{v} {}
    explicit constexpr UnitVector(double v_x, double v_y, double v_z) : v_{v_x, v_y, v_z} {}

public:
    friend struct ONB;

    static UnitVector from_vector(Vector v) {
        return UnitVector{v /= v.length()};
    }

    static UnitVector random_sphere() {
        const double z = math::random(-1.0, 1.0);
        const double phi = math::random(0.0, 2.0 * math::pi);
        const double r = std::sqrt(1.0 - z * z);
        return UnitVector{r * std::cos(phi), r * std::sin(phi), z};
    }

    static UnitVector random_hemisphere() {
        const double z = math::random(0.0, 1.0);
        const double phi = math::random(0.0, 2.0 * math::pi);
        const double r = std::sqrt(1.0 - z * z);
        return UnitVector{r * std::cos(phi), r * std::sin(phi), z};
    }

    static UnitVector random_cosine_hemisphere() {
        const double r_sq = math::random();
        const double r = std::sqrt(r_sq);
        const double phi = math::random(0.0, 2.0 * math::pi);
        
        const double x = r * std::cos(phi);
        const double y = r * std::sin(phi);
        const double z = std::sqrt(1.0 - r_sq);
        
        return UnitVector{x, y, z};
    }

    static constexpr UnitVector e_x() { return UnitVector(1, 0, 0); }
    static constexpr UnitVector e_y() { return UnitVector(0, 1, 0); }
    static constexpr UnitVector e_z() { return UnitVector(0, 0, 1); }

    constexpr operator Vector() const { return v_; }

    constexpr double x() const { return v_.x(); }
    constexpr double y() const { return v_.y(); }
    constexpr double z() const { return v_.z(); }

    constexpr UnitVector operator-() const {
        return UnitVector{-v_};
    }
};

inline UnitVector unit_vector(const Vector& v) {
    return UnitVector::from_vector(v);
}

inline UnitVector unit_vector(double v_x, double v_y, double v_z) {
    return UnitVector::from_vector(Vector(v_x, v_y, v_z));
}

constexpr double length_sq(const Vector& v) {
    return v.length_sq();
}

inline double length(const Vector& v) {
    return v.length();
}

inline std::ostream& operator<<(std::ostream& out, const Vector& v) {
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

constexpr Vector operator+(Vector v, const Vector& u) {
    return v += u;
}

constexpr Vector operator-(Vector v, const Vector& u) {
    return v -= u;
}

constexpr Vector operator*(Vector v, double mu) {
    return v *= mu;
}

constexpr Vector operator*(double mu, const Vector& v) {
    return v * mu;
}

constexpr Vector operator/(Vector v, double mu) {
    return v /= mu;
}

constexpr double dot(const Vector& v, const Vector& u) {
    return v.x()*u.x() + v.y()*u.y() + v.z()*u.z();
}

constexpr Vector cross(const Vector& u, const Vector& v) {
    return Vector(u.y() * v.z() - u.z() * v.y(),
                  u.z() * v.x() - u.x() * v.z(),
                  u.x() * v.y() - u.y() * v.x());
}

namespace math {
    [[nodiscard]] inline bool almost_zero(const Vector& v) {
        return almost_zero(v.x()) && almost_zero(v.y()) && almost_zero(v.z());
    }

    [[nodiscard]] inline bool almost_equal(const Vector& u, const Vector& v) {
        return almost_zero(v - u);
    }
}