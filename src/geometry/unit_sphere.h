#pragma once

#include "hittable.h"
#include "aabb.h"

class UnitSphere : public Hittable {
private:
    const AABB bounding_box_;

public:
    UnitSphere();

    bool hit(const Ray& local_ray, HitRecord& hit_record, double t_min) const override;

    AABB bounding_box() const override;
};
