#include "sampler.h"

#include <cmath>

#include "math/math_utils.h"

thread_local int Sampler::current_sample_index_ = 0;

Sampler::Sampler(int samples_per_pixel) : samples_per_pixel_{samples_per_pixel} {}

int Sampler::samples_per_pixel() const { return samples_per_pixel_; }

void Sampler::start_pixel(int row, int col) const {
    current_sample_index_ = 0;
}

void Sampler::next_sample() const {
    current_sample_index_++;
}


RandomSampler::RandomSampler(int samples_per_pixel) : Sampler{samples_per_pixel} {}

double RandomSampler::sample_1d() const {
    return math::random();
}

std::pair<double, double> RandomSampler::sample_2d() const {
    return {math::random(), math::random()};
}


StratifiedSampler::StratifiedSampler(int samples_per_pixel) : Sampler{samples_per_pixel} {
    sqrt_samples_ = static_cast<int>(std::sqrt(samples_per_pixel));
    inv_sqrt_samples_ = 1.0 / sqrt_samples_;
}

double StratifiedSampler::sample_1d() const {
    int i = current_sample_index_ % samples_per_pixel_;
    double u = (i + math::random()) / samples_per_pixel_;
    return u;
}

std::pair<double, double> StratifiedSampler::sample_2d() const {
    int i = current_sample_index_ % sqrt_samples_;
    int j = current_sample_index_ / sqrt_samples_;

    double u = (i + math::random()) * inv_sqrt_samples_;
    double v = (j + math::random()) * inv_sqrt_samples_;

    return {u, v};
}
