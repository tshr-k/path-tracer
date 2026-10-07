#pragma once

#include <array>
#include <cstddef>
#include <iosfwd>

#include "vector3.h"
#include "point.h"

struct EulerAngles {
    double roll, pitch, yaw;

    EulerAngles();
    EulerAngles(double r, double p, double y);

    EulerAngles& operator+=(const EulerAngles& other);
};

struct Scale {
    double x, y, z;

    Scale();
    Scale(double s);
    Scale(double s_x, double s_y, double s_z);

    Scale inverse() const;

    Scale& operator*=(const Scale& other);

    [[nodiscard]] bool is_invertible() const;
};

class Matrix {
private:
    std::array<double, 16> m_; // Row-major order

public:
    Matrix();

    static Matrix scale(double s_x, double s_y, double s_z);
    static Matrix scale(const Scale& s);
    static Matrix scale(double s);

    static Matrix translation(double tx, double ty, double tz);
    static Matrix translation(const Vector& v);

    static Matrix rotation(double angle, const UnitVector& axis);
    static Matrix rotation(double roll, double pitch, double yaw);
    static Matrix rotation(const EulerAngles& euler_angles);

    double operator[](std::size_t i) const;
    double operator()(std::size_t r, std::size_t c) const;

    Point operator*(const Point& p) const;
    Vector operator*(const Vector& v) const;
    Matrix operator*(const Matrix& other) const;

    Matrix& operator+=(const Matrix& addend);
    Matrix& operator-=(const Matrix& subtrahend);

    void transpose();
};

std::ostream& operator<<(std::ostream& out, Matrix m);

Matrix operator+(Matrix m, const Matrix& n);
Matrix operator-(Matrix m, const Matrix& n);

[[nodiscard]] Matrix transpose(Matrix m);

namespace math {
    [[nodiscard]] bool almost_zero(const Matrix& m);
    [[nodiscard]] bool almost_equal(const Matrix& m, const Matrix& n);
}
