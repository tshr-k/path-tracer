#pragma once

#include <array>
#include <vector>

#include "hittable.h"
#include "material.h"
#include "transform.h"
#include "instance.h"
#include "camera.h"


class Scene {
private:
    // TODO: Prevent duplicate materials and geometries here
    // Scene owns all cameras, instances, materials and geometries
    std::vector<Camera> cameras_;
    int active_camera_index_;
    
    std::vector<Instance> instances_;

    // Using unique_ptr to allow for polymorphic materials and geometries
    std::vector<MaterialVariant> materials_;
    std::vector<HittableVariant> geometries_;

    Colour background_colour_;  // TODO: Add environment map here later

public:
    Scene() : active_camera_index_{-1}, background_colour_{Colour::black} {};

    bool hit_instance(const Ray& global_ray, HitRecord& hit_record, const math::Interval t_range, const Instance& instance) const {
        const Ray local_ray = instance.T.global_to_local(global_ray);

        if (std::visit([
                &local_ray,
                &hit_record,
                &t_range
            ](const auto& geometry) {
                return geometry.hit(local_ray, hit_record, t_range);
            }, geometries_[instance.geometry_index()])) {
            hit_record.point = instance.T.local_to_global(hit_record.point);
            hit_record.normal = unit_vector(instance.T.local_to_global_normal(hit_record.normal));
            hit_record.material_index = instance.material_index();
            // t is invariant under scaling since the ray direction is left unnormalised, so we don't need to recalculate it here

            return true;
        }

        return false;
    }

    // Pass by value + move semantics is the idiomatic way to transfer ownership
    void add_material(MaterialVariant material) {
        materials_.push_back(std::move(material));
    }

    void add_geometry(HittableVariant geometry) {
        geometries_.push_back(std::move(geometry));
    }

    void add_instance(std::size_t geometry_index, std::size_t material_index, const Transform& T) {
        // geometry and material must already be in the scene
        auto instance = Instance{geometry_index, material_index};
        instance.T = T;
        instances_.push_back(std::move(instance));
    }

    void set_background_colour(const Colour& c) {
        background_colour_ = c;
    }

    void add_camera(const Camera camera, bool set_active = false) {
        cameras_.push_back(std::move(camera));
        if (set_active || active_camera_index_ == -1) {
            active_camera_index_ = cameras_.size() - 1;
        }
    }

    const std::vector<Instance>& world() const { return instances_; }
    const std::vector<MaterialVariant>& materials() const { return materials_; }
    const std::vector<HittableVariant>& geometries() const { return geometries_; }
    const Camera& active_camera() const { return cameras_[active_camera_index_]; }
    const Colour& background_colour() const { return background_colour_; }
};