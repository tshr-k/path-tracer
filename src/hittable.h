#pragma once

#include <limits>

#include "ray.h"
#include "point.h"
#include "vector3.h"
#include "material.h"
#include "math_utils.h"
#include "aabb.h"

struct HitRecord {
    double t{math::infinity};
    Point point{};
    UnitVector normal = UnitVector::e_x();
    Material* material = nullptr;  // Raw pointer to avoid overhead of increments. Lifetime is managed by Scene
};

class Hittable {
public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray& ray, HitRecord& hit_record, const math::Interval t_range) const = 0;

    virtual AABB bounding_box() const = 0;
};