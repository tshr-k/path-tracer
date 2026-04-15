#pragma once

#include <cstddef>
#include <memory>
#include <ostream>
#include <vector>

#include "bvh.h"
#include "instance.h"
#include "hittable.h"
#include "aabb.h"


class TLAS : public Hittable {
private:
    BVH<std::size_t> bvh_;  // A size_t index to an Instance in Scene is used as the LeafData

    const std::vector<Instance>* instances_;  // No Instances are added while TLAS is being used, so a raw pointer is fine
    
public:
    TLAS() : instances_{nullptr} {}

    void build_bvh(const std::vector<Instance>& instances) {
        instances_ = &instances;  // Update pointer to the new instances vector in Scene

        std::vector<std::size_t> instance_indices(instances.size());
        std::vector<AABB> instance_bboxes(instances.size());
        for (std::size_t i = 0; i < instances.size(); ++i) {
            instance_indices[i] = i;
            instance_bboxes[i] = instances[i].bounding_box();
        }
        bvh_.build(instance_indices, instance_bboxes);
    }

    bool hit(const Ray& ray, HitRecord& hit_record, double t_min) const override {
        return bvh_.hit(ray, hit_record, t_min, [this](const Ray& ray, HitRecord& hit_record, double t_min, const std::size_t instance_index) {
            return (*instances_)[instance_index].hit(ray, hit_record, t_min);
        });
    }

    AABB bounding_box() const override {
        return bvh_.bounding_box();
    }

    std::size_t num_nodes() const {
        return bvh_.num_nodes();
    }

    std::ostream& print(std::ostream& os) const {
        return bvh_.print(os);
    }
};

inline std::ostream& operator<<(std::ostream& os, const TLAS& tlas) {
    return tlas.print(os);
}