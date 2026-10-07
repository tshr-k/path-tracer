#pragma once

#include <cstddef>

#include "math/math_utils.h"
#include "math/point.h"
#include "math/vector3.h"
#include "aabb.h"

class Ray;

struct HitRecord {
    double t{math::infinity};  // Upper bound on t for the nearest hit so far, i.e., t_max
    Point point{};
    UnitVector normal = UnitVector::e_x();
    std::size_t material_index{0};
};

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& ray, HitRecord& hit_record, double t_min) const = 0;  // hit_record already contains the nearest hit so far, i.e., t_max

    virtual AABB bounding_box() const = 0;
};
