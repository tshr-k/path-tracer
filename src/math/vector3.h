#pragma once

#include <array>
#include <cstddef>
#include <iosfwd>

class Vector;
double dot(const Vector& v, const Vector& u);

class Vector {
private:
    std::array<double, 3> v_;

public:
    Vector();
    Vector(double v_x, double v_y, double v_z);

    double x() const;
    double y() const;
    double z() const;

    double operator[](std::size_t i) const;

    Vector operator-() const;

    Vector& operator+=(const Vector& u);
    Vector& operator-=(const Vector& u);
    Vector& operator*=(double mu);
    Vector& operator/=(double mu);

    double length_sq() const;
    double length() const;
};


struct ONB;

class UnitVector {
private:
    Vector v_;
    explicit UnitVector(const Vector& v);
    explicit UnitVector(double v_x, double v_y, double v_z);

public:
    friend struct ONB;

    static UnitVector from_vector(Vector v);

    static UnitVector random_sphere();
    static UnitVector random_hemisphere();
    static UnitVector random_cosine_hemisphere();

    static UnitVector e_x();
    static UnitVector e_y();
    static UnitVector e_z();

    operator Vector() const;

    double x() const;
    double y() const;
    double z() const;

    UnitVector operator-() const;
};

UnitVector unit_vector(const Vector& v);
UnitVector unit_vector(double v_x, double v_y, double v_z);

double length_sq(const Vector& v);
double length(const Vector& v);

std::ostream& operator<<(std::ostream& out, const Vector& v);

Vector operator+(Vector v, const Vector& u);
Vector operator-(Vector v, const Vector& u);
Vector operator*(Vector v, double mu);
Vector operator*(double mu, const Vector& v);
Vector operator/(Vector v, double mu);

Vector cross(const Vector& u, const Vector& v);

namespace math {
    [[nodiscard]] bool almost_zero(const Vector& v);
    [[nodiscard]] bool almost_equal(const Vector& u, const Vector& v);
}
