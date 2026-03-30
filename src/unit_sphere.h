#pragma once

#include <cmath>

#include "vector3.h"
#include "hittable.h"
#include "math_utils.h"

class UnitSphere : public Hittable {
private:
    const AABB bounding_box_;

public:
    UnitSphere() : bounding_box_{AABB(Point{-1, -1, -1}, Point{1, 1, 1})} {}

    bool hit(const Ray& local_ray, HitRecord& hit_record, const math::Interval t_range) const override {
        const Vector origin = local_ray.origin().r();
        const Vector direction = local_ray.direction();

        const double a = length_sq(direction);  // Ray direction is not necessarily a unit vector
        const double half_b = dot(origin, direction);
        const double c = length_sq(origin) - 1.0;

        const double discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        const double sqrt_d = std::sqrt(discriminant);

        double t = (-half_b - sqrt_d) / a;
        if (t < t_range.min || t_range.max < t) {
            t = (-half_b + sqrt_d) / a;
            if (t < t_range.min || t_range.max < t)
                return false;
        }

        hit_record.t = t;
        hit_record.point = local_ray.at(hit_record.t);
        hit_record.normal = unit_vector(hit_record.point.r());

        return true;
    }

    constexpr AABB bounding_box() const override {
        return bounding_box_;
    }
};