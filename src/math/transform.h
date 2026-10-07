#pragma once

#include "matrix4.h"
#include "point.h"

class Ray;

class Transform {
private:
    Point position_{0.0, 0.0, 0.0};
    Scale scale_{1.0, 1.0, 1.0};
    EulerAngles rotation_{0.0, 0.0, 0.0};

    mutable Matrix l_to_g_{}; // Matrix() is identity
    mutable Matrix g_to_l_{};

    mutable bool matrix_is_valid_{true};

    void recalculate_matrices() const;
    void ensure_matrix_validity() const;

public:
    Transform() = default;

    Transform& set_position(const Point& p);
    Transform& set_scale(const Scale& s);
    Transform& set_rotation(const EulerAngles& euler_angles);
    Transform& translate(const Vector& v);
    Transform& scale(const Scale& s);

    [[nodiscard]] Point local_to_global(const Point& p) const;
    [[nodiscard]] Point global_to_local(const Point& p) const;

    [[nodiscard]] Vector local_to_global(const Vector& v) const;
    [[nodiscard]] Vector global_to_local(const Vector& v) const;

    [[nodiscard]] Vector local_to_global_normal(const Vector& normal) const;
    [[nodiscard]] Vector global_to_local_normal(const Vector& normal) const;

    [[nodiscard]] Ray local_to_global(const Ray& r) const;
    [[nodiscard]] Ray global_to_local(const Ray& r) const;
};
