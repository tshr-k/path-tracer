#include "bsdf.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "math/math_utils.h"
#include "math/vector3.h"

double BSDF::schlick_fresnel(double cos_theta, double F0) {
    return F0 + (1.0 - F0) * std::pow(1.0 - cos_theta, 5.0);
}

Colour BSDF::schlick_fresnel(double cos_theta, const Colour& F0) {
    return F0 + (Colour{1.0, 1.0, 1.0} - F0) * std::pow(1.0 - cos_theta, 5.0);
}


LambertianBRDF::LambertianBRDF(const Colour& albedo) : albedo_{albedo} {}

Colour LambertianBRDF::evaluate(const UnitVector& w_i, const UnitVector& w_o) const {
    if (w_i.z() <= 0 || w_o.z() <= 0) return Colour{0, 0, 0};
    return albedo_ / math::pi;
}

UnitVector LambertianBRDF::sample(const UnitVector& w_o, double& p) const {
    UnitVector w_i = UnitVector::random_cosine_hemisphere();

    if (w_i.z() <= 0 || w_o.z() <= 0) {
        p = 0.0;
    } else {
        p = w_i.z() / math::pi;
    }

    return w_i;
}

double LambertianBRDF::pdf(const UnitVector& w_i, const UnitVector& w_o) const {
    if (w_i.z() <= 0 || w_o.z() <= 0) return 0.0;
    return w_i.z() / math::pi;
}


double GGXBRDF::ggx_D(const UnitVector& h) const {
    const double n_h = std::max(h.z(), 0.0);       // dot(n, h) = h.z
    const double alpha_sq  = alpha_ * alpha_;
    const double denominator = n_h * n_h * (alpha_sq - 1.0) + 1.0;
    return alpha_sq / (math::pi * denominator * denominator);
}

double GGXBRDF::ggx_G1(const UnitVector& v) const {
    const double n_v = std::max(v.z(), 0.0);  // Local space: n = (0,0,1)
    const double k = alpha_ / 2.0;      // Common approximation
    return n_v / (n_v * (1.0 - k) + k);
}

double GGXBRDF::ggx_G(const UnitVector& wi, const UnitVector& wo) const {
    return ggx_G1(wi) * ggx_G1(wo);
}

UnitVector GGXBRDF::sample_h(double alpha) const {
    const double phi = math::random(0.0, 2.0 * math::pi);
    const double u_2 = math::random();
    const double cos_theta = std::sqrt((1.0 - u_2) / (1.0 + (alpha*alpha - 1.0) * u_2));
    const double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    return unit_vector(
        sin_theta * std::cos(phi),
        sin_theta * std::sin(phi),
        cos_theta
    );
}

GGXBRDF::GGXBRDF(double roughness, double F0) : fresnel_F0_{F0, F0, F0}, alpha_{roughness*roughness} {}

GGXBRDF::GGXBRDF(double roughness, const Colour& F0) : fresnel_F0_{F0}, alpha_{roughness*roughness} {}

Colour GGXBRDF::evaluate(const UnitVector& w_i, const UnitVector& w_o) const {
    if (w_i.z() <= 0 || w_o.z() <= 0) return Colour{0, 0, 0};
    UnitVector h = unit_vector(w_i + w_o);
    const double D = ggx_D(h);
    const double G = ggx_G(w_i, w_o);
    const Colour F = schlick_fresnel(std::max(dot(h, w_o), 0.0), fresnel_F0_);

    return D * G * F / (4.0 * w_i.z() * w_o.z());
}

UnitVector GGXBRDF::sample(const UnitVector& w_o, double& p) const {
    UnitVector h = sample_h(alpha_);

    // Reflect w_o about h to get w_i
    UnitVector w_i = unit_vector(2 * dot(w_o, h) * h - w_o);

    if (w_i.z() <= 0 || w_o.z() <= 0) {
        p = 0.0;
        return w_i;
    }

    const double n_dot_h = std::max(h.z(), 0.0);
    const double wo_dot_h = std::max(dot(w_o, h), 0.0);

    if (wo_dot_h <= 0.0 || n_dot_h <= 0.0) {
        p = 0.0;
        return w_i;
    }

    const double D = ggx_D(h);
    const double pdf_h = D * n_dot_h;
    p = pdf_h / (4.0 * wo_dot_h);

    return w_i;
}

double GGXBRDF::pdf(const UnitVector& w_i, const UnitVector& w_o) const {
    if (w_i.z() <= 0 || w_o.z() <= 0) return 0.0;

    UnitVector h = unit_vector(w_i + w_o);
    const double n_dot_h = std::max(h.z(), 0.0);
    const double wo_dot_h = std::max(dot(w_o, h), 0.0);

    if (wo_dot_h <= 0.0 || n_dot_h <= 0.0) return 0.0;

    const double D = ggx_D(h);
    const double pdf_h = D * n_dot_h;
    return pdf_h / (4.0 * wo_dot_h);
}


int MixerBSDF::sample_component() const {
    const double r = math::random();
    return (r < weight_0_) ? 0 : 1;
}

MixerBSDF::MixerBSDF(std::unique_ptr<BSDF> bsdf_0, std::unique_ptr<BSDF> bsdf_1, double weight_0, double weight_1)
: bsdf_0_{std::move(bsdf_0)}, bsdf_1_{std::move(bsdf_1)} {
    const double sum = weight_0 + weight_1;
    if (sum <= 0.0) {
        throw std::runtime_error("MixerBSDF weights must be positive");
    }
    weight_0_ = weight_0 / sum;
    weight_1_ = weight_1 / sum;
}

Colour MixerBSDF::evaluate(const UnitVector& w_i, const UnitVector& w_o) const {
    Colour total = Colour{0, 0, 0};
    total += weight_0_ * bsdf_0_->evaluate(w_i, w_o);
    total += weight_1_ * bsdf_1_->evaluate(w_i, w_o);
    return total;
}

UnitVector MixerBSDF::sample(const UnitVector& w_o, double& p) const {
    const int idx = sample_component();

    UnitVector w_i = UnitVector::e_z();
    if (idx == 0) {
        w_i = bsdf_0_->sample(w_o, p);
    } else {
        w_i = bsdf_1_->sample(w_o, p);
    }

    // Full mixture PDF at w_i for unbiased mixture sampling
    const double p_0 = bsdf_0_->pdf(w_i, w_o);
    const double p_1 = bsdf_1_->pdf(w_i, w_o);
    p = weight_0_ * p_0 + weight_1_ * p_1;

    return w_i;
}

double MixerBSDF::pdf(const UnitVector& w_i, const UnitVector& w_o) const {
    const double p_0 = bsdf_0_->pdf(w_i, w_o);
    const double p_1 = bsdf_1_->pdf(w_i, w_o);
    return weight_0_ * p_0 + weight_1_ * p_1;
}


FresnelBSDF::FresnelBSDF(std::unique_ptr<BSDF> base_bsdf, std::unique_ptr<BSDF> top_bsdf)
    : base_bsdf_{std::move(base_bsdf)}, top_bsdf_{std::move(top_bsdf)} {}

Colour FresnelBSDF::evaluate(const UnitVector& w_i, const UnitVector& w_o) const {
    const Vector h = unit_vector(w_i + w_o);
    const double F = schlick_fresnel(std::max(dot(h, w_o), 0.0));  // Assuming dielectric fresnel
    // Top layer is already weighted by F in its own evaluate
    return top_bsdf_->evaluate(w_i, w_o) + (1.0 - F) * base_bsdf_->evaluate(w_i, w_o);
}

UnitVector FresnelBSDF::sample(const UnitVector& w_o, double& p) const {
    const double F = schlick_fresnel(std::max(w_o.z(), 0.0));  // Assuming dielectric fresnel
    UnitVector w_i = UnitVector::e_z();
    if (math::random() < F) {
        w_i = top_bsdf_->sample(w_o, p);
    } else {
        w_i = base_bsdf_->sample(w_o, p);
    }

    // Mixture PDF of layered Fresnel BSDF
    const double p_top  = top_bsdf_->pdf(w_i, w_o);
    const double p_base = base_bsdf_->pdf(w_i, w_o);
    p = F * p_top + (1.0 - F) * p_base;

    return w_i;
}

double FresnelBSDF::pdf(const UnitVector& w_i, const UnitVector& w_o) const {
    const double F = schlick_fresnel(std::max(w_o.z(), 0.0));
    const double p_top  = top_bsdf_->pdf(w_i, w_o);
    const double p_base = base_bsdf_->pdf(w_i, w_o);
    return F * p_top + (1.0 - F) * p_base;
}
