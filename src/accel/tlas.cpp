#include "tlas.h"

#include "geometry/aabb.h"
#include "math/ray.h"

TLAS::TLAS() : instances_{nullptr} {}

void TLAS::build_bvh(const std::vector<Instance>& instances) {
    instances_ = &instances;  // Update pointer to the new instances vector in Scene

    std::vector<std::size_t> instance_indices(instances.size());
    std::vector<AABB> instance_bboxes(instances.size());
    for (std::size_t i = 0; i < instances.size(); ++i) {
        instance_indices[i] = i;
        instance_bboxes[i] = instances[i].bounding_box();
    }
    bvh_.build(instance_indices, instance_bboxes);
}

bool TLAS::hit(const Ray& ray, HitRecord& hit_record, double t_min) const {
    return bvh_.hit(ray, hit_record, t_min, [this](const Ray& ray, HitRecord& hit_record, double t_min, const std::size_t instance_index) {
        return (*instances_)[instance_index].hit(ray, hit_record, t_min);
    });
}

AABB TLAS::bounding_box() const {
    return bvh_.bounding_box();
}

std::size_t TLAS::num_nodes() const {
    return bvh_.num_nodes();
}

std::ostream& TLAS::print(std::ostream& os) const {
    return bvh_.print(os);
}

std::ostream& operator<<(std::ostream& os, const TLAS& tlas) {
    return tlas.print(os);
}
