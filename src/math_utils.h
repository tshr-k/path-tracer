#pragma once

#include <cmath>
#include <iostream>
#include <random>


namespace math {
    // Default for internal comparison functions
    constexpr double eps = 1e-6;

    // Prevent self-intersection (shadow acne)
    constexpr double shadow_epsilon = 1e-3;

    // For parallel ray/plane tests
    constexpr double parallel_epsilon = 1e-8;

    // For avoiding zero-width bounding boxes
    constexpr double bbox_eps = 1e-4;

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

    [[nodiscard]] constexpr math::Interval pad(double padding) const {
        return {min - padding, max + padding};
    }

    [[nodiscard]] constexpr math::Interval ensure_size(double min_size) const {
        const double current_size = size();
        if (current_size >= min_size) {
            return *this;
        }
        const double padding = (min_size - current_size) / 2.0;
        return this->pad(padding);
    }
};


inline std::ostream& operator<<(std::ostream& os, const math::Interval& interval) {
    return os << "[" << interval.min << ", " << interval.max << "]";
}