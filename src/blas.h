#pragma once

#include <cstddef>
#include <memory>
#include <ostream>

#include "aabb.h"
#include "instance.h"
#include "mesh.h"
#include "bvh.h"
#include "point.h"
#include "vector3.h"
#include "ray.h"
#include "hittable.h"

inline bool hit_triangle(const Ray& ray, const std::size_t triangle_index, const Mesh* mesh_ptr, HitRecord& hit_record, double t_min) {
    const Triangle& triangle = mesh_ptr->triangles[triangle_index];
    const Point& v_0 = mesh_ptr->vertices[triangle.v_indices[0]];
    const Point& v_1 = mesh_ptr->vertices[triangle.v_indices[1]];
    const Point& v_2 = mesh_ptr->vertices[triangle.v_indices[2]];
    const Vector edge_1 = v_1 - v_0;
    const Vector edge_2 = v_2 - v_0;
    const Vector h = cross(ray.direction(), edge_2);
    const double det = dot(edge_1, h);

    if (std::abs(det) < math::parallel_epsilon) return false;  // Ray is parallel to triangle. Remove abs for backface culling

    const double f = 1.0 / det;
    const Vector s = ray.origin() - v_0;
    const double u = f * dot(s, h);
    if (u < 0.0 || u > 1.0) return false;

    const Vector q = cross(s, edge_1);
    const double v = f * dot(ray.direction(), q);
    if (v < 0.0 || u + v > 1.0) return false;

    const double t = f * dot(edge_2, q);

    if (t_min < t && t < hit_record.t) {
        hit_record.t = t;
        hit_record.point = ray.at(t);

        if (triangle.shade_smooth) {  // Interpolate vertex normals for smooth shading
            const Vector& n_0 = mesh_ptr->vertex_normals[triangle.vn_indices[0]];
            const Vector& n_1 = mesh_ptr->vertex_normals[triangle.vn_indices[1]];
            const Vector& n_2 = mesh_ptr->vertex_normals[triangle.vn_indices[2]];
            hit_record.normal = unit_vector((1 - u - v) * n_0 + u * n_1 + v * n_2);
        } else {
            hit_record.normal = unit_vector(cross(edge_1, edge_2));
        }
        return true;
    }

    return false;
}


class BLAS : public Hittable {
private:
    BVH<TriangleRange> bvh_;

    std::shared_ptr<Mesh> mesh_;
    const Mesh* const mesh_ptr_;  // Raw pointer for faster access to mesh data in hit function

    void build_bvh() {
        std::vector<TriangleRange> triangle_ranges(mesh_->triangles.size());
        std::vector<AABB> triangle_bboxes(mesh_->triangles.size());
        for (std::size_t i = 0; i < mesh_->triangles.size(); ++i) {
            triangle_ranges[i] = TriangleRange{i, 1};  // Each leaf contains just one triangle, for now
            const Triangle& triangle = mesh_->triangles[i];
            const Point& v_0 = mesh_->vertices[triangle.v_indices[0]];
            const Point& v_1 = mesh_->vertices[triangle.v_indices[1]];
            const Point& v_2 = mesh_->vertices[triangle.v_indices[2]];
            triangle_bboxes[i] = AABB{v_0, v_1, v_2};
        }
        bvh_.build(triangle_ranges, triangle_bboxes);
    }

public:
    BLAS() : mesh_{nullptr}, mesh_ptr_{nullptr} {}
    BLAS(std::shared_ptr<Mesh> mesh) : mesh_{mesh}, mesh_ptr_{mesh.get()} {  // The mesh doesn't change after BLAS construction, so eagerly building the BVH is fine
        build_bvh();
    }

    bool hit(const Ray& local_ray, HitRecord& hit_record, double t_min) const override {
        return bvh_.hit(local_ray, hit_record, t_min, [this](const Ray& ray, HitRecord& hit_record, double t_min, const TriangleRange triangle_range) {
            bool hit_anything = false;

            for (std::size_t i = 0; i < triangle_range.num_triangles; ++i) {
                if (hit_triangle(ray, triangle_range.start_idx + i, mesh_ptr_, hit_record, t_min)) {
                    hit_anything = true;
                }
            }

            return hit_anything;
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

inline std::ostream& operator<<(std::ostream& os, const BLAS& blas) {
    return blas.print(os);
}