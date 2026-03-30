#pragma once

#include <cmath>

#include "vector3.h"
#include "ray.h"

struct ONB {
public:
    UnitVector t, b, n;

    ONB(const UnitVector& normal) : n{normal}, t{UnitVector::e_x()}, b{UnitVector::e_y()} {
        double sign = std::copysign(1.0, n.z());
        const double a = -1.0 / (sign + n.z());
        const double b_val = n.x() * n.y() * a;
        
        t = UnitVector(1.0 + sign * n.x() * n.x() * a, sign * b_val, -sign * n.x());
        b = UnitVector(b_val, sign + n.y() * n.y() * a, -n.y());
    }

    Vector global_to_local(const Vector& v) const {
        return Vector(dot(v, t), dot(v, b), dot(v, n));
    }

    Vector local_to_global(const Vector& v) const {
        return v.x() * t + v.y() * b + v.z() * n;
    }
};