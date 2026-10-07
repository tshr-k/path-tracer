#include "unit_sphere.h"

#include <cmath>

#include "math/point.h"
#include "math/ray.h"
#include "math/vector3.h"

UnitSphere::UnitSphere() : bounding_box_{Point{-1, -1, -1}, Point{1, 1, 1}} {}

bool UnitSphere::hit(const Ray& local_ray, HitRecord& hit_record, double t_min) const {
    const Vector origin = local_ray.origin().r();
    const Vector direction = local_ray.direction();

    const double a = length_sq(direction);  // Ray direction is not necessarily a unit vector
    const double half_b = dot(origin, direction);
    const double c = length_sq(origin) - 1.0;

    const double discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    const double sqrt_d = std::sqrt(discriminant);

    double t = (-half_b - sqrt_d) / a;
    if (t < t_min || hit_record.t < t) {  // The hit record contains the nearest hit so far
        t = (-half_b + sqrt_d) / a;
        if (t < t_min || hit_record.t < t)
            return false;
    }

    hit_record.t = t;
    hit_record.point = local_ray.at(hit_record.t);
    hit_record.normal = unit_vector(hit_record.point.r());

    return true;
}

AABB UnitSphere::bounding_box() const {
    return bounding_box_;
}
