#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "geometry/hittable.h"
#include "geometry/instance.h"
#include "geometry/mesh.h"
#include "shading/material.h"
#include "accel/tlas.h"
#include "math/colour.h"
#include "math/transform.h"
#include "camera.h"


class Scene {
private:
    // TODO: Prevent duplicate materials and geometries here
    // Scene owns all cameras, instances, materials and geometries

    std::vector<Camera> cameras_;  // Lightweight, stored by value
    int active_camera_index_;

    std::vector<Instance> instances_;  // Lightweight, stored by value

    std::vector<std::shared_ptr<Mesh>> meshes_;  // Only used for BLAS construction, not needed after that. Heap allocated due to large size
    std::vector<std::unique_ptr<Hittable>> geometries_;  // UnitSpheres or BLASes. Heap allocated for stable addresses and due to large size

    // Contiguous array is cache friendly, plus most materials are the same size
    std::vector<MaterialVariant> materials_;

    std::unique_ptr<TLAS> tlas_;
    mutable bool tlas_is_valid_ = false;  // TLAS is lazily constructed

    Colour background_colour_;  // TODO: Add environment map here later

public:
    Scene();

    // Must be called before scene_.hit() can safely be called from multiple threads:
    // it performs the lazy TLAS build (and the Instance/Transform caches it triggers)
    // single-threaded, so later calls just read an already-valid flag.
    void ensure_tlas_built() const;

    bool hit(const Ray& ray, HitRecord& hit_record, double t_min) const;

    // Pass by value + move semantics is the idiomatic way to transfer ownership
    int add_material(MaterialVariant material);

    int add_geometry(std::unique_ptr<Hittable> geometry);

    int add_mesh(const Mesh& mesh);

    int add_instance(std::size_t geometry_index, std::size_t material_index, const Transform& T);

    void set_background_colour(const Colour& c);

    int add_camera(Camera camera, bool set_active = false);

    const std::vector<Instance>& instances() const;
    const std::vector<MaterialVariant>& materials() const;
    const std::vector<std::unique_ptr<Hittable>>& geometries() const;
    const Camera& active_camera() const;
    const Colour& background_colour() const;
};
