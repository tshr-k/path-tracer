#pragma once

#include <utility>


class Sampler {
protected:
    const int samples_per_pixel_;
    mutable int current_sample_index_ = 0;

public:
    virtual ~Sampler() = default;
    Sampler(int samples_per_pixel);

    int samples_per_pixel() const;

    virtual void start_pixel(int row, int col) const;
    virtual void next_sample() const;

    virtual double sample_1d() const = 0;
    virtual std::pair<double, double> sample_2d() const = 0;
};


class RandomSampler : public Sampler {
public:
    RandomSampler(int samples_per_pixel);

    double sample_1d() const override;
    std::pair<double, double> sample_2d() const override;
};


class StratifiedSampler : public Sampler {
private:
    int sqrt_samples_;
    double inv_sqrt_samples_;

public:
    StratifiedSampler(int samples_per_pixel);

    double sample_1d() const override;
    std::pair<double, double> sample_2d() const override;
};
