#pragma once

#include <vector>
#include <array>

#include "hittable.h"
#include "material.h"
#include "transform.h"
#include "instance.h"
#include "camera.h"


class Scene {
private:
    // TODO: Prevent duplicate materials and geometries here
    // Scene owns all materials, geometries, instances and cameras, so we can use raw pointers to refer to them
    std::vector<std::unique_ptr<Instance>> instances_;
    std::vector<std::unique_ptr<Material>> materials_;
    std::vector<std::unique_ptr<Hittable>> geometries_;

    std::vector<std::unique_ptr<Camera>> cameras_;
    int active_camera_index_;

    Colour background_colour_;  // TODO: Add environment map here later

public:
    Scene() : active_camera_index_{-1}, background_colour_{Colour::black} {};

    void add_material(std::unique_ptr<Material> material) {
        materials_.push_back(std::move(material));
    }

    void add_geometry(std::unique_ptr<Hittable> geometry) {
        geometries_.push_back(std::move(geometry));
    }

    void add_instance(Hittable* geometry, Material* material, const Transform& T) {
        // geometry and material must already be in the scene
        auto instance = std::make_unique<Instance>(geometry, material);
        instance->T = T;
        instances_.push_back(std::move(instance));
    }

    void set_background_colour(const Colour& c) {
        background_colour_ = c;
    }

    void add_camera(std::unique_ptr<Camera> camera, bool set_active = false) {
        cameras_.push_back(std::move(camera));
        if (set_active || active_camera_index_ == -1) {
            active_camera_index_ = cameras_.size() - 1;
        }
    }

    const std::vector<std::unique_ptr<Instance>>& world() const { return instances_; }
    const std::vector<std::unique_ptr<Material>>& materials() const { return materials_; }
    const std::vector<std::unique_ptr<Hittable>>& geometries() const { return geometries_; }
    const Camera& active_camera() const { return *cameras_[active_camera_index_]; }
    const Colour& background_colour() const { return background_colour_; }
};