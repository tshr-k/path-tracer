#pragma once

#include <cstddef>
#include <ostream>
#include <vector>

#include "bvh.h"
#include "geometry/hittable.h"
#include "geometry/instance.h"


class TLAS : public Hittable {
private:
    BVH<std::size_t> bvh_;  // A size_t index to an Instance in Scene is used as the LeafData

    const std::vector<Instance>* instances_;  // No Instances are added while TLAS is being used, so a raw pointer is fine

public:
    TLAS();

    void build_bvh(const std::vector<Instance>& instances);

    bool hit(const Ray& ray, HitRecord& hit_record, double t_min) const override;

    AABB bounding_box() const override;

    std::size_t num_nodes() const;

    std::ostream& print(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const TLAS& tlas);
