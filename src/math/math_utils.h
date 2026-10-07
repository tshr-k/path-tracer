#pragma once

#include <iosfwd>
#include <limits>

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

    [[nodiscard]] bool almost_zero(double a, double epsilon = eps);
    [[nodiscard]] bool almost_equal(double a, double b, double epsilon = eps);

    [[nodiscard]] double degrees_to_radians(double degrees);
    [[nodiscard]] double radians_to_degrees(double radians);

    [[nodiscard]] double random();  // [0, 1)
    [[nodiscard]] double random(double min, double max);  // [min, max)

    struct Interval;
}


struct math::Interval {
    double min;
    double max;

    Interval();
    Interval(double min, double max);

    static Interval merge(const Interval& a, const Interval& b);

    double size() const;
    bool contains(double x) const;
    bool surrounds(double x) const;

    [[nodiscard]] math::Interval pad(double padding) const;
    [[nodiscard]] math::Interval ensure_size(double min_size) const;
};


std::ostream& operator<<(std::ostream& os, const math::Interval& interval);
