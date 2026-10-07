#pragma once

#include <memory>

#include "math/colour.h"

class UnitVector;

class BSDF {
protected:
    // Schlick Fresnel for scalar reflectance (dielectrics)
    static double schlick_fresnel(double cos_theta, double F0 = 0.04);

    // Schlick Fresnel for color reflectance (metals)
    static Colour schlick_fresnel(double cos_theta, const Colour& F0);

public:
    virtual ~BSDF() = default;

    // All functions work in local coordinates, where the surface normal is (0, 0, 1)
    virtual Colour evaluate(const UnitVector& w_i, const UnitVector& w_o) const = 0;

    // Also writes the probability density of the sampled direction into p
    virtual UnitVector sample(const UnitVector& w_o, double& p) const = 0;

    // PDF of sampling w_i given w_o under this BSDF's sampling strategy.
    // This is useful for mixtures and for multiple importance sampling.
    virtual double pdf(const UnitVector& w_i, const UnitVector& w_o) const = 0;
};


class LambertianBRDF : public BSDF {
private:
    const Colour albedo_;

public:
    LambertianBRDF(const Colour& albedo);

    Colour evaluate(const UnitVector& w_i, const UnitVector& w_o) const override;
    UnitVector sample(const UnitVector& w_o, double& p) const override;
    double pdf(const UnitVector& w_i, const UnitVector& w_o) const override;
};


class GGXBRDF : public BSDF {
private:
    const Colour fresnel_F0_;
    const double alpha_;

    double ggx_D(const UnitVector& h) const;
    double ggx_G1(const UnitVector& v) const;
    double ggx_G(const UnitVector& wi, const UnitVector& wo) const;

    UnitVector sample_h(double alpha) const;

public:
    // In both constructors, alpha = roughness^2, allowing better artistic control
    // Dielectric constructor: F0 is the reflectance at normal incidence
    GGXBRDF(double roughness, double F0 = 0.04);

    // Metallic constructor: F0 is the reflectance color at normal incidence
    GGXBRDF(double roughness, const Colour& F0);

    Colour evaluate(const UnitVector& w_i, const UnitVector& w_o) const override;
    UnitVector sample(const UnitVector& w_o, double& p) const override;
    double pdf(const UnitVector& w_i, const UnitVector& w_o) const override;
};


class MixerBSDF : public BSDF {  // Binary mixture of exactly two BSDF components
private:
    std::unique_ptr<BSDF> bsdf_0_;
    std::unique_ptr<BSDF> bsdf_1_;
    double weight_0_;
    double weight_1_;

    int sample_component() const;

public:
    MixerBSDF(std::unique_ptr<BSDF> bsdf_0, std::unique_ptr<BSDF> bsdf_1, double weight_0, double weight_1);

    Colour evaluate(const UnitVector& w_i, const UnitVector& w_o) const override;
    UnitVector sample(const UnitVector& w_o, double& p) const override;
    double pdf(const UnitVector& w_i, const UnitVector& w_o) const override;
};


class FresnelBSDF : public BSDF {
private:
    std::unique_ptr<BSDF> base_bsdf_;
    std::unique_ptr<BSDF> top_bsdf_;

public:
    FresnelBSDF(std::unique_ptr<BSDF> base_bsdf, std::unique_ptr<BSDF> top_bsdf);

    Colour evaluate(const UnitVector& w_i, const UnitVector& w_o) const override;
    UnitVector sample(const UnitVector& w_o, double& p) const override;
    double pdf(const UnitVector& w_i, const UnitVector& w_o) const override;
};
