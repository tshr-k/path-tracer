#pragma once

#include <memory>

#include "colour.h"
#include "bsdf.h"
#include <variant>


struct HitRecord;

class Material {
private:
    // TODO: Add texture maps and normal maps here. Until then, create_bsdf just returns a constant BSDF for the whole surface

public:
    virtual ~Material() = default;
    virtual std::unique_ptr<BSDF> create_bsdf() const = 0;  // No arguments for now. Might depend on local coordinates in the future
    virtual Colour emission() const { return Colour{0, 0, 0}; }
};


class DiffuseMaterial : public Material {
private:
    const Colour albedo_;

public:
    constexpr DiffuseMaterial(const Colour& albedo) : albedo_{albedo} {}

    std::unique_ptr<BSDF> create_bsdf() const override {
        return std::make_unique<LambertianBRDF>(albedo_);
    }
};


class MetalMaterial : public Material {
private:
    const Colour colour_;
    const double roughness_;

public:
    constexpr MetalMaterial(const Colour& colour, double roughness) : colour_{colour}, roughness_{roughness} {}

    std::unique_ptr<BSDF> create_bsdf() const override {
        return std::make_unique<GGXBRDF>(roughness_, colour_);
    }
};


class DielectricMaterial : public Material {
private:
    const double roughness_;

public:
    constexpr DielectricMaterial(double roughness) : roughness_{roughness} {}

    std::unique_ptr<BSDF> create_bsdf() const override {
        return std::make_unique<GGXBRDF>(roughness_, 0.04);
    }
};


class PlasticMaterial : public Material {
private:
    const Colour albedo_;
    const double roughness_;

public:
    constexpr PlasticMaterial(const Colour& albedo, double roughness) : albedo_{albedo}, roughness_{roughness} {}

    std::unique_ptr<BSDF> create_bsdf() const override {
        auto diffuse_bsdf = std::make_unique<LambertianBRDF>(albedo_);
        auto specular_bsdf = std::make_unique<GGXBRDF>(roughness_, 0.04);
        auto plastic_bsdf = std::make_unique<FresnelBSDF>(std::move(diffuse_bsdf), std::move(specular_bsdf));
        return plastic_bsdf;
    }
};


class EmissiveMaterial : public Material {
private:
    const Colour colour_;
    const double strength_;

public:
    constexpr EmissiveMaterial(const Colour& colour, double strength = 1.0) : colour_{colour}, strength_{strength} {}

    std::unique_ptr<BSDF> create_bsdf() const override {
        return nullptr;  // Purely emissive, scattering is neglected
    }

    constexpr Colour emission() const override {
        return colour_ * strength_;
    }
};


class UberMaterial : public Material {
private:
    const Colour colour_;
    const double metallic_;
    const double roughness_;
    const Colour emission_colour_;
    const double emission_strength_;

public:
    constexpr UberMaterial(const Colour& albedo, double metallic, double roughness, const Colour& emission_colour = Colour{0, 0, 0}, double emission_strength = 1.0)
        : colour_{albedo}, metallic_{metallic}, roughness_{roughness}, emission_colour_{emission_colour}, emission_strength_{emission_strength} {}

    std::unique_ptr<BSDF> create_bsdf() const override {
        // Compute physically-motivated diffuse and specular weights using F0.
        const double F0_scalar = (colour_.r() + colour_.g() + colour_.b()) / 3.0;
        const double k_s = metallic_ * F0_scalar;
        const double k_d = (1.0 - metallic_) * (1.0 - k_s);

        // If one of the lobes is effectively zero, return the other directly.
        if (k_d <= 0.0 && k_s <= 0.0) {
            return std::make_unique<LambertianBRDF>(colour_);
        }
        if (k_d <= 0.0) {
            return std::make_unique<GGXBRDF>(roughness_, colour_);
        }
        if (k_s <= 0.0) {
            return std::make_unique<LambertianBRDF>(colour_);
        }

        auto diffuse_bsdf  = std::make_unique<LambertianBRDF>(colour_);
        auto specular_bsdf = std::make_unique<GGXBRDF>(roughness_, colour_);

        return std::make_unique<MixerBSDF>(std::move(diffuse_bsdf), std::move(specular_bsdf), k_d, k_s);
    }

    constexpr Colour emission() const override {
        return emission_colour_ * emission_strength_;
    }
};

using MaterialVariant = std::variant<DiffuseMaterial, EmissiveMaterial, PlasticMaterial, MetalMaterial, UberMaterial>;
