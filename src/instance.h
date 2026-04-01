#pragma once

#include <cstddef>

#include "aabb.h"
#include "hittable.h"
#include "material.h"
#include "transform.h"
#include "math_utils.h"

class Instance {
private:
    std::size_t geometry_index_;
    std::size_t material_index_;

public:
    Transform T;

    Instance(std::size_t geometry_index, std::size_t material_index)
        : geometry_index_{geometry_index}, material_index_{material_index} {}
    
    std::size_t geometry_index() const { return geometry_index_; }
    std::size_t material_index() const { return material_index_; }

    // AABB bounding_box() const override {
    //     const AABB local_box = geometry_->bounding_box();
    //     return AABB{
    //         math::Interval{T.local_to_global(Point{local_box.x().min, 0, 0}).x(), T.local_to_global(Point{local_box.x().max, 0, 0}).x()},
    //         math::Interval{T.local_to_global(Point{0, local_box.y().min, 0}).y(), T.local_to_global(Point{0, local_box.y().max, 0}).y()},
    //         math::Interval{T.local_to_global(Point{0, 0, local_box.z().min}).z(), T.local_to_global(Point{0, 0, local_box.z().max}).z()}
    //     };
    // }
};