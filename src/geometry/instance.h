#pragma once

#include <cstddef>

#include "hittable.h"
#include "aabb.h"
#include "math/transform.h"

class Instance : public Hittable {
private:
    Hittable* const geometry_;  // Memory managed by Scene
    const std::size_t material_index_;

    Transform T;

    mutable AABB global_bbox_;
    mutable bool bbox_is_valid_ = false;

    void recalculate_bbox() const;
    void ensure_bbox_validity() const;

public:
    Instance(Hittable* geometry, std::size_t material_index);

    std::size_t material_index() const;

    bool hit(const Ray& global_ray, HitRecord& hit_record, double t_min) const override;

    AABB bounding_box() const override;

    Instance& set_transform(const Transform& transform);
    Instance& set_position(const Point& p);
    Instance& set_scale(const Scale& s);
    Instance& set_rotation(const EulerAngles& euler_angles);
    Instance& translate(const Vector& v);
    Instance& scale(const Scale& s);
};
