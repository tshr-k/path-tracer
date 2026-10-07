#pragma once

#include <memory>
#include <variant>

#include "math/colour.h"

class BSDF;

class Material {
private:
    // TODO: Add texture maps and normal maps here. Until then, create_bsdf just returns a constant BSDF for the whole surface

public:
    virtual ~Material() = default;
    virtual std::unique_ptr<BSDF> create_bsdf() const = 0;  // No arguments for now. Might depend on local coordinates in the future
    virtual Colour emission() const;
};


class DiffuseMaterial : public Material {
private:
    const Colour albedo_;

public:
    DiffuseMaterial(const Colour& albedo);

    std::unique_ptr<BSDF> create_bsdf() const override;
};


class MetalMaterial : public Material {
private:
    const Colour colour_;
    const double roughness_;

public:
    MetalMaterial(const Colour& colour, double roughness);

    std::unique_ptr<BSDF> create_bsdf() const override;
};


class DielectricMaterial : public Material {
private:
    const double roughness_;

public:
    DielectricMaterial(double roughness);

    std::unique_ptr<BSDF> create_bsdf() const override;
};


class PlasticMaterial : public Material {
private:
    const Colour albedo_;
    const double roughness_;

public:
    PlasticMaterial(const Colour& albedo, double roughness);

    std::unique_ptr<BSDF> create_bsdf() const override;
};


class EmissiveMaterial : public Material {
private:
    const Colour colour_;
    const double strength_;

public:
    EmissiveMaterial(const Colour& colour, double strength = 1.0);

    std::unique_ptr<BSDF> create_bsdf() const override;

    Colour emission() const override;
};


class UberMaterial : public Material {
private:
    const Colour colour_;
    const double metallic_;
    const double roughness_;
    const Colour emission_colour_;
    const double emission_strength_;

public:
    UberMaterial(const Colour& albedo, double metallic, double roughness,
                 const Colour& emission_colour = Colour{0, 0, 0}, double emission_strength = 1.0);

    std::unique_ptr<BSDF> create_bsdf() const override;

    Colour emission() const override;
};

using MaterialVariant = std::variant<DiffuseMaterial, EmissiveMaterial, PlasticMaterial, MetalMaterial, UberMaterial>;
