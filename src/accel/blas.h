#pragma once

#include <cstddef>
#include <memory>
#include <ostream>

#include "bvh.h"
#include "geometry/hittable.h"
#include "geometry/mesh.h"

bool hit_triangle(const Ray& ray, std::size_t triangle_index, const Mesh* mesh_ptr, HitRecord& hit_record, double t_min);


class BLAS : public Hittable {
private:
    BVH<TriangleRange> bvh_;

    std::shared_ptr<Mesh> mesh_;
    const Mesh* const mesh_ptr_;  // Raw pointer for faster access to mesh data in hit function

    void build_bvh();

public:
    BLAS();
    BLAS(std::shared_ptr<Mesh> mesh);  // The mesh doesn't change after BLAS construction, so eagerly building the BVH is fine

    bool hit(const Ray& local_ray, HitRecord& hit_record, double t_min) const override;

    AABB bounding_box() const override;

    std::size_t num_nodes() const;

    std::ostream& print(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const BLAS& blas);
