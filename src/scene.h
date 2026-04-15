#pragma once

#include <array>
#include <memory>
#include <vector>

#include "hittable.h"
#include "material.h"
#include "transform.h"
#include "instance.h"
#include "camera.h"
#include "mesh.h"
#include "bvh.h"
#include "tlas.h"
#include "blas.h"


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
    Scene() : active_camera_index_{-1}, background_colour_{Colour::black}, tlas_{std::make_unique<TLAS>()} {}

    bool hit(const Ray& ray, HitRecord& hit_record, double t_min) const {
        if (!tlas_is_valid_) {
            tlas_->build_bvh(instances_);
            tlas_is_valid_ = true;
        }
        return tlas_->hit(ray, hit_record, t_min);
    }

    // Pass by value + move semantics is the idiomatic way to transfer ownership
    void add_material(MaterialVariant material) {
        materials_.push_back(std::move(material));
    }

    void add_geometry(std::unique_ptr<Hittable> geometry) {
        geometries_.push_back(std::move(geometry));
    }

    void add_mesh(const Mesh& mesh) {
        meshes_.push_back(std::make_shared<Mesh>(mesh));
        add_geometry(std::make_unique<BLAS>(meshes_.back()));
    }

    void add_instance(std::size_t geometry_index, std::size_t material_index, const Transform& T) {
        // geometry and material must already be in the scene
        auto instance = Instance{geometries_[geometry_index].get(), material_index};
        instance.set_transform(T);
        instances_.push_back(std::move(instance));
        tlas_is_valid_ = false;
    }

    void set_background_colour(const Colour& c) {
        background_colour_ = c;
    }

    void add_camera(Camera camera, bool set_active = false) {
        cameras_.push_back(std::move(camera));
        if (set_active || active_camera_index_ == -1) {
            active_camera_index_ = cameras_.size() - 1;
        }
    }

    const std::vector<Instance>& instances() const { return instances_; }
    const std::vector<MaterialVariant>& materials() const { return materials_; }
    const std::vector<std::unique_ptr<Hittable>>& geometries() const { return geometries_; }
    const Camera& active_camera() const { return cameras_[active_camera_index_]; }
    const Colour& background_colour() const { return background_colour_; }
};