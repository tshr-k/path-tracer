#include "matrix4.h"

#include <cmath>
#include <ostream>
#include <utility>

#include "math_utils.h"

EulerAngles::EulerAngles() : roll{0}, pitch{0}, yaw{0} {}
EulerAngles::EulerAngles(double r, double p, double y) : roll{r}, pitch{p}, yaw{y} {}

EulerAngles& EulerAngles::operator+=(const EulerAngles& other) {
    roll += other.roll;
    pitch += other.pitch;
    yaw += other.yaw;
    return *this;
}


Scale::Scale() : x{1}, y{1}, z{1} {}
Scale::Scale(double s) : x{s}, y{s}, z{s} {}
Scale::Scale(double s_x, double s_y, double s_z) : x{s_x}, y{s_y}, z{s_z} {}

Scale Scale::inverse() const {
    return Scale{
        math::almost_zero(x) ? 1 : 1.0/x,
        math::almost_zero(y) ? 1 : 1.0/y,
        math::almost_zero(z) ? 1 : 1.0/z,
    };
}

Scale& Scale::operator*=(const Scale& other) {
    x *= other.x;
    y *= other.y;
    z *= other.z;
    return *this;
}

bool Scale::is_invertible() const {
    return !(math::almost_zero(x) || math::almost_zero(y) || math::almost_zero(z));
}


Matrix::Matrix() : m_{} {
    // Identity matrix by default
    m_[0] = m_[5] = m_[10] = m_[15] = 1;
}

Matrix Matrix::scale(double s_x, double s_y, double s_z) {
    Matrix m;
    m.m_[0] = s_x;
    m.m_[5] = s_y;
    m.m_[10] = s_z;
    return m;
}

Matrix Matrix::scale(const Scale& s) {
    return scale(s.x, s.y, s.z);
}

Matrix Matrix::scale(double s) {
    return scale(s, s, s);
}

Matrix Matrix::translation(double tx, double ty, double tz) {
    Matrix m;
    m.m_[3] = tx;
    m.m_[7] = ty;
    m.m_[11] = tz;
    return m;
}

Matrix Matrix::translation(const Vector& v) {
    return translation(v.x(), v.y(), v.z());
}

Matrix Matrix::rotation(double angle, const UnitVector& axis) {
    const double c = std::cos(angle);
    const double s = std::sin(angle);
    const double t = 1 - c;

    const double x = axis.x();
    const double y = axis.y();
    const double z = axis.z();

    Matrix m;
    m.m_[0] = t*x*x + c;
    m.m_[1] = t*x*y - s*z;
    m.m_[2] = t*x*z + s*y;

    m.m_[4] = t*x*y + s*z;
    m.m_[5] = t*y*y + c;
    m.m_[6] = t*y*z - s*x;

    m.m_[8] = t*x*z - s*y;
    m.m_[9] = t*y*z + s*x;
    m.m_[10] = t*z*z + c;

    return m;
}

Matrix Matrix::rotation(double roll, double pitch, double yaw) {
    Matrix y = Matrix::rotation(yaw, UnitVector::e_y());
    Matrix p = Matrix::rotation(pitch, UnitVector::e_x());
    Matrix r = Matrix::rotation(roll, UnitVector::e_z());

    return y * p * r;
}

Matrix Matrix::rotation(const EulerAngles& euler_angles) {
    return Matrix::rotation(euler_angles.roll, euler_angles.pitch, euler_angles.yaw);
}

double Matrix::operator[](std::size_t i) const { return m_[i]; }
double Matrix::operator()(std::size_t r, std::size_t c) const { return m_[r * 4 + c]; }

Point Matrix::operator*(const Point& p) const {
    double x = m_[0]*p.x() + m_[1]*p.y() + m_[2]*p.z() + m_[3];
    double y = m_[4]*p.x() + m_[5]*p.y() + m_[6]*p.z() + m_[7];
    double z = m_[8]*p.x() + m_[9]*p.y() + m_[10]*p.z() + m_[11];

    return Point(x, y, z);
}

Vector Matrix::operator*(const Vector& v) const {
    double x = m_[0]*v.x() + m_[1]*v.y() + m_[2]*v.z();
    double y = m_[4]*v.x() + m_[5]*v.y() + m_[6]*v.z();
    double z = m_[8]*v.x() + m_[9]*v.y() + m_[10]*v.z();

    return Vector(x, y, z);
}

Matrix Matrix::operator*(const Matrix& other) const {
    Matrix res;
    const auto& a = m_;
    const auto& b = other.m_;

    // Row 0
    res.m_[0] = a[0]*b[0] + a[1]*b[4] + a[2]*b[8]  + a[3]*b[12];
    res.m_[1] = a[0]*b[1] + a[1]*b[5] + a[2]*b[9]  + a[3]*b[13];
    res.m_[2] = a[0]*b[2] + a[1]*b[6] + a[2]*b[10] + a[3]*b[14];
    res.m_[3] = a[0]*b[3] + a[1]*b[7] + a[2]*b[11] + a[3]*b[15];

    // Row 1
    res.m_[4] = a[4]*b[0] + a[5]*b[4] + a[6]*b[8]  + a[7]*b[12];
    res.m_[5] = a[4]*b[1] + a[5]*b[5] + a[6]*b[9]  + a[7]*b[13];
    res.m_[6] = a[4]*b[2] + a[5]*b[6] + a[6]*b[10] + a[7]*b[14];
    res.m_[7] = a[4]*b[3] + a[5]*b[7] + a[6]*b[11] + a[7]*b[15];

    // Row 2
    res.m_[8] = a[8]*b[0] + a[9]*b[4] + a[10]*b[8]  + a[11]*b[12];
    res.m_[9] = a[8]*b[1] + a[9]*b[5] + a[10]*b[9]  + a[11]*b[13];
    res.m_[10]= a[8]*b[2] + a[9]*b[6] + a[10]*b[10] + a[11]*b[14];
    res.m_[11]= a[8]*b[3] + a[9]*b[7] + a[10]*b[11] + a[11]*b[15];

    // Row 3
    res.m_[12]= a[12]*b[0] + a[13]*b[4] + a[14]*b[8]  + a[15]*b[12];
    res.m_[13]= a[12]*b[1] + a[13]*b[5] + a[14]*b[9]  + a[15]*b[13];
    res.m_[14]= a[12]*b[2] + a[13]*b[6] + a[14]*b[10] + a[15]*b[14];
    res.m_[15]= a[12]*b[3] + a[13]*b[7] + a[14]*b[11] + a[15]*b[15];

    return res;
}

Matrix& Matrix::operator+=(const Matrix& addend) {
    for (int i = 0; i < 16; i++) {
        this->m_[i] += addend.m_[i];
    }
    return *this;
}

Matrix& Matrix::operator-=(const Matrix& subtrahend) {
    for (int i = 0; i < 16; i++) {
        this->m_[i] -= subtrahend.m_[i];
    }
    return *this;
}

void Matrix::transpose() {
    std::swap(m_[1], m_[4]);
    std::swap(m_[2], m_[8]);
    std::swap(m_[3], m_[12]);
    std::swap(m_[6], m_[9]);
    std::swap(m_[7], m_[13]);
    std::swap(m_[11], m_[14]);
}

std::ostream& operator<<(std::ostream& out, Matrix m) {
    out << m(0, 0) << ' ' << m(0, 1) << ' ' << m(0, 2) << ' ' << m(0, 3) << '\n'
        << m(1, 0) << ' ' << m(1, 1) << ' ' << m(1, 2) << ' ' << m(1, 3) << '\n'
        << m(2, 0) << ' ' << m(2, 1) << ' ' << m(2, 2) << ' ' << m(2, 3) << '\n'
        << m(3, 0) << ' ' << m(3, 1) << ' ' << m(3, 2) << ' ' << m(3, 3);

    return out;
}

Matrix operator+(Matrix m, const Matrix& n) {
    return m += n;
}

Matrix operator-(Matrix m, const Matrix& n) {
    return m -= n;
}

Matrix transpose(Matrix m) {
    m.transpose();
    return m;
}

namespace math {
    bool almost_zero(const Matrix& m) {
        for (int i = 0; i < 16; i++) {
            if (!almost_zero(m[i])) {
                return false;
            }
        }

        return true;
    }

    bool almost_equal(const Matrix& m, const Matrix& n) {
        return almost_zero(m - n);
    }
}
