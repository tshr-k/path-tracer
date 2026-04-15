#pragma once

#include <cstddef>
#include <memory>

#include "aabb.h"
#include "hittable.h"
#include "material.h"
#include "transform.h"
#include "math_utils.h"

class Instance : public Hittable {
private:
    Hittable* const geometry_;  // Memory managed by Scene
    const std::size_t material_index_;
    
    Transform T;

    mutable AABB global_bbox_;
    mutable bool bbox_is_valid_ = false;

    void recalculate_bbox() const {
        const AABB local_bbox = geometry_->bounding_box();
        global_bbox_ = AABB{
            T.local_to_global(Point{local_bbox.x().min, local_bbox.y().min, local_bbox.z().min}),
            T.local_to_global(Point{local_bbox.x().max, local_bbox.y().min, local_bbox.z().min}),
            T.local_to_global(Point{local_bbox.x().min, local_bbox.y().max, local_bbox.z().min}),
            T.local_to_global(Point{local_bbox.x().max, local_bbox.y().max, local_bbox.z().min}),
            T.local_to_global(Point{local_bbox.x().min, local_bbox.y().min, local_bbox.z().max}),
            T.local_to_global(Point{local_bbox.x().max, local_bbox.y().min, local_bbox.z().max}),
            T.local_to_global(Point{local_bbox.x().min, local_bbox.y().max, local_bbox.z().max}),
            T.local_to_global(Point{local_bbox.x().max, local_bbox.y().max, local_bbox.z().max})
        };
    }

    void ensure_bbox_validity() const {
        if (!bbox_is_valid_) {
            recalculate_bbox();
            bbox_is_valid_ = true;
        }
    }
    
public:
    Instance(Hittable* geometry, std::size_t material_index)
        : geometry_{geometry}, material_index_{material_index} {}
    
    std::size_t material_index() const { return material_index_; }

    bool hit(const Ray& global_ray, HitRecord& hit_record, double t_min) const override {
        const Ray local_ray = T.global_to_local(global_ray);

        if (geometry_->hit(local_ray, hit_record, t_min)) {
            hit_record.point = T.local_to_global(hit_record.point);
            hit_record.normal = unit_vector(T.local_to_global_normal(hit_record.normal));
            hit_record.material_index = material_index_;
            // t is invariant under scaling since the ray direction is left unnormalised, so we don't need to recalculate it here
            return true;
        }

        return false;
    }

    AABB bounding_box() const override {
        ensure_bbox_validity();
        return global_bbox_;
    }

    constexpr Instance& set_transform(const Transform& transform) {
        T = transform;
        bbox_is_valid_ = false;
        return *this;
    }

    constexpr Instance& set_position(const Point& p) {
        T.set_position(p);
        bbox_is_valid_ = false;
        return *this;
    }

    constexpr Instance& set_scale(const Scale& s) {
        T.set_scale(s);
        bbox_is_valid_ = false;
        return *this;
    }

    constexpr Instance& set_rotation(const EulerAngles& euler_angles) {
        T.set_rotation(euler_angles);
        bbox_is_valid_ = false;
        return *this;
    }

    constexpr Instance& translate(const Vector& v) {
        T.translate(v);
        bbox_is_valid_ = false;
        return *this;
    }

    constexpr Instance& scale(const Scale& s) {
        T.scale(s);
        bbox_is_valid_ = false;
        return *this;
    }
};