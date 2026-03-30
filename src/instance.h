#pragma once

#include "aabb.h"
#include "hittable.h"
#include "material.h"
#include "transform.h"
#include "math_utils.h"

class Instance : public Hittable {
private:
    const Hittable* const geometry_;  // Lifetime is managed by Scene
    Material* const material_;

public:
    Transform T;

    Instance(const Hittable* geometry, Material* const material)
        : geometry_{geometry}, material_{material} {}

    const Hittable* geometry() const { return geometry_; }
    const Material* material() const { return material_; }
    
    bool hit(const Ray& global_ray, HitRecord& hit_record, const math::Interval t_range) const override {
        const Ray local_ray = T.global_to_local(global_ray);

        if (geometry_->hit(local_ray, hit_record, t_range)) {
            hit_record.point = T.local_to_global(hit_record.point);
            hit_record.normal = unit_vector(T.local_to_global_normal(hit_record.normal));
            hit_record.material = material_;
            // t is invariant under scaling since the ray direction is left unnormalised, so we don't need to recalculate it here

            return true;
        }

        return false;
    }

    AABB bounding_box() const override {
        const AABB local_box = geometry_->bounding_box();
        return AABB{
            math::Interval{T.local_to_global(Point{local_box.x().min, 0, 0}).x(), T.local_to_global(Point{local_box.x().max, 0, 0}).x()},
            math::Interval{T.local_to_global(Point{0, local_box.y().min, 0}).y(), T.local_to_global(Point{0, local_box.y().max, 0}).y()},
            math::Interval{T.local_to_global(Point{0, 0, local_box.z().min}).z(), T.local_to_global(Point{0, 0, local_box.z().max}).z()}
        };
    }
};