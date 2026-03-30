#pragma once

#include <utility>

#include "math_utils.h"


class Sampler {
protected:
    const int samples_per_pixel_;
    mutable int current_sample_index_ = 0;

public:
    virtual ~Sampler() = default;
    constexpr Sampler(int samples_per_pixel) : samples_per_pixel_{samples_per_pixel} {}
    
    constexpr int samples_per_pixel() const { return samples_per_pixel_; }

    virtual void start_pixel(int row, int col) const {
        current_sample_index_ = 0;
    }

    virtual void next_sample() const {
        current_sample_index_++;
    }

    virtual double sample_1d() const = 0;
    virtual std::pair<double, double> sample_2d() const = 0;
};


class RandomSampler : public Sampler {
public:
    constexpr RandomSampler(int samples_per_pixel) : Sampler{samples_per_pixel} {}

    double sample_1d() const override {
        return math::random();
    }

    std::pair<double, double> sample_2d() const override {
        return {math::random(), math::random()};
    }
};


class StratifiedSampler : public Sampler {
private:
    int sqrt_samples_;
    double inv_sqrt_samples_;

public:
    StratifiedSampler(int samples_per_pixel) : Sampler{samples_per_pixel} {
        sqrt_samples_ = static_cast<int>(std::sqrt(samples_per_pixel));
        inv_sqrt_samples_ = 1.0 / sqrt_samples_;
    }

    double sample_1d() const override {
        int i = current_sample_index_ % samples_per_pixel_;
        double u = (i + math::random()) / samples_per_pixel_;
        return u;
    }

    std::pair<double, double> sample_2d() const override {
        int i = current_sample_index_ % sqrt_samples_;
        int j = current_sample_index_ / sqrt_samples_;

        double u = (i + math::random()) * inv_sqrt_samples_;
        double v = (j + math::random()) * inv_sqrt_samples_;
        
        return {u, v};
    }
};