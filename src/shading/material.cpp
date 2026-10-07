#include "material.h"

#include "shading/bsdf.h"

Colour Material::emission() const { return Colour{0, 0, 0}; }


DiffuseMaterial::DiffuseMaterial(const Colour& albedo) : albedo_{albedo} {}

std::unique_ptr<BSDF> DiffuseMaterial::create_bsdf() const {
    return std::make_unique<LambertianBRDF>(albedo_);
}


MetalMaterial::MetalMaterial(const Colour& colour, double roughness) : colour_{colour}, roughness_{roughness} {}

std::unique_ptr<BSDF> MetalMaterial::create_bsdf() const {
    return std::make_unique<GGXBRDF>(roughness_, colour_);
}


DielectricMaterial::DielectricMaterial(double roughness) : roughness_{roughness} {}

std::unique_ptr<BSDF> DielectricMaterial::create_bsdf() const {
    return std::make_unique<GGXBRDF>(roughness_, 0.04);
}


PlasticMaterial::PlasticMaterial(const Colour& albedo, double roughness) : albedo_{albedo}, roughness_{roughness} {}

std::unique_ptr<BSDF> PlasticMaterial::create_bsdf() const {
    auto diffuse_bsdf = std::make_unique<LambertianBRDF>(albedo_);
    auto specular_bsdf = std::make_unique<GGXBRDF>(roughness_, 0.04);
    auto plastic_bsdf = std::make_unique<FresnelBSDF>(std::move(diffuse_bsdf), std::move(specular_bsdf));
    return plastic_bsdf;
}


EmissiveMaterial::EmissiveMaterial(const Colour& colour, double strength) : colour_{colour}, strength_{strength} {}

std::unique_ptr<BSDF> EmissiveMaterial::create_bsdf() const {
    return nullptr;  // Purely emissive, scattering is neglected
}

Colour EmissiveMaterial::emission() const {
    return colour_ * strength_;
}


UberMaterial::UberMaterial(const Colour& albedo, double metallic, double roughness, const Colour& emission_colour, double emission_strength)
    : colour_{albedo}, metallic_{metallic}, roughness_{roughness}, emission_colour_{emission_colour}, emission_strength_{emission_strength} {}

std::unique_ptr<BSDF> UberMaterial::create_bsdf() const {
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

Colour UberMaterial::emission() const {
    return emission_colour_ * emission_strength_;
}
