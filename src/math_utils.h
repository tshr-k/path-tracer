#pragma once

#include <cmath>
#include <random>

namespace math {
    constexpr double eps = 1e-6;

    constexpr double pi = 3.14159265358979323846;

    constexpr double infinity = std::numeric_limits<double>::infinity();

    [[nodiscard]] inline bool almost_zero(double a, double epsilon = eps) {
        return std::abs(a) < epsilon;
    }

    [[nodiscard]] inline bool almost_equal(double a, double b, double epsilon = eps) {
        return almost_zero(a - b, epsilon);
    }

    [[nodiscard]] inline double degrees_to_radians(double degrees) {
        return degrees * pi / 180.0;
    }

    [[nodiscard]] inline double radians_to_degrees(double radians) {
        return radians * 180.0 / pi;
    }

    [[nodiscard]] inline double random() {
        // [0, 1)
        static std::uniform_real_distribution<double> distribution(0.0, 1.0);
        static std::mt19937 generator;
        return distribution(generator);
    }

    [[nodiscard]] inline double random(double min, double max) {
        // [min, max)
        return min + (max - min) * random();
    }

    struct Interval;
}

struct math::Interval {
    double min;
    double max;

    constexpr Interval() : min{math::infinity}, max{-math::infinity} {}
    constexpr Interval(double min, double max) : min{min}, max{max} {}
    
    constexpr static Interval merge(const Interval a, const Interval b) {
        return {std::min(a.min, b.min), std::max(a.max, b.max)};
    }

    constexpr double size() const {
        return max - min;
    }

    constexpr bool contains(double x) const {
        return min <= x && x <= max;
    }

    constexpr bool surrounds(double x) const {
        return min < x && x < max;
    }

    [[noexcept]] constexpr math::Interval expand(double padding) const {
        return {min - padding, max + padding};
    }
};