#pragma once

#include "matrix4.h"
#include "vector3.h"
#include "point.h"
#include "ray.h"

class Transform {
private:
    Point position_{0.0, 0.0, 0.0};
    Scale scale_{1.0, 1.0, 1.0};
    EulerAngles rotation_{0.0, 0.0, 0.0};

    mutable Matrix l_to_g_{}; // Matrix() is identity
    mutable Matrix g_to_l_{};

    mutable bool matrix_is_valid_{true};

    void recalculate_matrices() const {
        Matrix t = Matrix::translation(position_.r());
        Matrix r = Matrix::rotation(rotation_);
        Matrix s = Matrix::scale(scale_);
        l_to_g_ = t * r * s;

        Matrix t_inv = Matrix::translation(-position_.r());
        Matrix r_inv = transpose(r);
        Matrix s_inv = Matrix::scale(scale_.inverse());
        g_to_l_ = s_inv * r_inv * t_inv;
    }

    constexpr void ensure_matrix_validity() const {
        if (!matrix_is_valid_) {
            recalculate_matrices();
            matrix_is_valid_ = true;
        }
    }

public:
    Transform() = default;

    constexpr Transform& set_position(const Point& p) {
        position_ = p;
        matrix_is_valid_ = false;
        return *this;
    }

    constexpr Transform& set_scale(const Scale& s) {
        scale_ = s;
        matrix_is_valid_ = false;
        return *this;
    }

    constexpr Transform& set_rotation(const EulerAngles& euler_angles) {
        rotation_ = euler_angles;
        matrix_is_valid_ = false;
        return *this;
    }

    constexpr Transform& translate(const Vector& v) {
        position_ += v;
        matrix_is_valid_ = false;
        return *this;
    }

    constexpr Transform& scale(const Scale& s) {
        scale_ *= s;
        matrix_is_valid_ = false;
        return *this;
    }

    [[nodiscard]] Point local_to_global(const Point& p) const {
        ensure_matrix_validity();
        return l_to_g_ * p;
    }

    [[nodiscard]] Point global_to_local(const Point& p) const {
        ensure_matrix_validity();
        return g_to_l_ * p;
    }

    [[nodiscard]] Vector local_to_global(const Vector& v) const {
        ensure_matrix_validity();
        return l_to_g_ * v;
    }
    
    [[nodiscard]] Vector global_to_local(const Vector& v) const {
        ensure_matrix_validity();
        return g_to_l_ * v;
    }

    [[nodiscard]] Vector local_to_global_normal(const Vector& normal) const {
        ensure_matrix_validity();
        return transpose(g_to_l_) * normal;
    }

    [[nodiscard]] Vector global_to_local_normal(const Vector& normal) const {
        ensure_matrix_validity();
        return transpose(l_to_g_) * normal;
    }

    [[nodiscard]] Ray local_to_global(const Ray& r) const {
        ensure_matrix_validity();
        return Ray{
            l_to_g_ * r.origin(),
            l_to_g_ * r.direction()  // Left unnormalised to preserve lengths under scaling
        };
    }

    [[nodiscard]] Ray global_to_local(const Ray& r) const {
        ensure_matrix_validity();
        return Ray{
            g_to_l_ * r.origin(),
            g_to_l_ * r.direction()  // Left unnormalised to preserve lengths under scaling
        };
    }
};