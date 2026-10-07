#include "transform.h"

#include "ray.h"

void Transform::recalculate_matrices() const {
    Matrix t = Matrix::translation(position_.r());
    Matrix r = Matrix::rotation(rotation_);
    Matrix s = Matrix::scale(scale_);
    l_to_g_ = t * r * s;

    Matrix t_inv = Matrix::translation(-position_.r());
    Matrix r_inv = transpose(r);
    Matrix s_inv = Matrix::scale(scale_.inverse());
    g_to_l_ = s_inv * r_inv * t_inv;
}

void Transform::ensure_matrix_validity() const {
    if (!matrix_is_valid_) {
        recalculate_matrices();
        matrix_is_valid_ = true;
    }
}

Transform& Transform::set_position(const Point& p) {
    position_ = p;
    matrix_is_valid_ = false;
    return *this;
}

Transform& Transform::set_scale(const Scale& s) {
    scale_ = s;
    matrix_is_valid_ = false;
    return *this;
}

Transform& Transform::set_rotation(const EulerAngles& euler_angles) {
    rotation_ = euler_angles;
    matrix_is_valid_ = false;
    return *this;
}

Transform& Transform::translate(const Vector& v) {
    position_ += v;
    matrix_is_valid_ = false;
    return *this;
}

Transform& Transform::scale(const Scale& s) {
    scale_ *= s;
    matrix_is_valid_ = false;
    return *this;
}

Point Transform::local_to_global(const Point& p) const {
    ensure_matrix_validity();
    return l_to_g_ * p;
}

Point Transform::global_to_local(const Point& p) const {
    ensure_matrix_validity();
    return g_to_l_ * p;
}

Vector Transform::local_to_global(const Vector& v) const {
    ensure_matrix_validity();
    return l_to_g_ * v;
}

Vector Transform::global_to_local(const Vector& v) const {
    ensure_matrix_validity();
    return g_to_l_ * v;
}

Vector Transform::local_to_global_normal(const Vector& normal) const {
    ensure_matrix_validity();
    return transpose(g_to_l_) * normal;
}

Vector Transform::global_to_local_normal(const Vector& normal) const {
    ensure_matrix_validity();
    return transpose(l_to_g_) * normal;
}

Ray Transform::local_to_global(const Ray& r) const {
    ensure_matrix_validity();
    return Ray{
        l_to_g_ * r.origin(),
        l_to_g_ * r.direction()  // Left unnormalised to preserve lengths under scaling
    };
}

Ray Transform::global_to_local(const Ray& r) const {
    ensure_matrix_validity();
    return Ray{
        g_to_l_ * r.origin(),
        g_to_l_ * r.direction()  // Left unnormalised to preserve lengths under scaling
    };
}
