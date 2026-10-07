#include "instance.h"

#include "math/ray.h"

void Instance::recalculate_bbox() const {
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

void Instance::ensure_bbox_validity() const {
    if (!bbox_is_valid_) {
        recalculate_bbox();
        bbox_is_valid_ = true;
    }
}

Instance::Instance(Hittable* geometry, std::size_t material_index)
    : geometry_{geometry}, material_index_{material_index} {}

std::size_t Instance::material_index() const { return material_index_; }

bool Instance::hit(const Ray& global_ray, HitRecord& hit_record, double t_min) const {
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

AABB Instance::bounding_box() const {
    ensure_bbox_validity();
    return global_bbox_;
}

Instance& Instance::set_transform(const Transform& transform) {
    T = transform;
    bbox_is_valid_ = false;
    return *this;
}

Instance& Instance::set_position(const Point& p) {
    T.set_position(p);
    bbox_is_valid_ = false;
    return *this;
}

Instance& Instance::set_scale(const Scale& s) {
    T.set_scale(s);
    bbox_is_valid_ = false;
    return *this;
}

Instance& Instance::set_rotation(const EulerAngles& euler_angles) {
    T.set_rotation(euler_angles);
    bbox_is_valid_ = false;
    return *this;
}

Instance& Instance::translate(const Vector& v) {
    T.translate(v);
    bbox_is_valid_ = false;
    return *this;
}

Instance& Instance::scale(const Scale& s) {
    T.scale(s);
    bbox_is_valid_ = false;
    return *this;
}
