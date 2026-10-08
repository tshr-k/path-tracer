#include "math_utils.h"

#include <algorithm>
#include <cmath>
#include <ostream>
#include <random>

namespace math {
    bool almost_zero(double a, double epsilon) {
        return std::abs(a) < epsilon;
    }

    bool almost_equal(double a, double b, double epsilon) {
        return almost_zero(a - b, epsilon);
    }

    double degrees_to_radians(double degrees) {
        return degrees * pi / 180.0;
    }

    double radians_to_degrees(double radians) {
        return radians * 180.0 / pi;
    }

    double random() {
        // Avoid correlation between tiles' noise and possible corruption of rng due to races
        static thread_local std::mt19937 generator{std::random_device{}()};
        static thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);
        return distribution(generator);
    }

    double random(double min, double max) {
        return min + (max - min) * random();
    }
}


math::Interval::Interval() : min{math::infinity}, max{-math::infinity} {}
math::Interval::Interval(double min, double max) : min{min}, max{max} {}

math::Interval math::Interval::merge(const Interval& a, const Interval& b) {
    return {std::min(a.min, b.min), std::max(a.max, b.max)};
}

double math::Interval::size() const {
    return max - min;
}

bool math::Interval::contains(double x) const {
    return min <= x && x <= max;
}

bool math::Interval::surrounds(double x) const {
    return min < x && x < max;
}

math::Interval math::Interval::pad(double padding) const {
    return {min - padding, max + padding};
}

math::Interval math::Interval::ensure_size(double min_size) const {
    const double current_size = size();
    if (current_size >= min_size) {
        return *this;
    }
    const double padding = (min_size - current_size) / 2.0;
    return this->pad(padding);
}


std::ostream& operator<<(std::ostream& os, const math::Interval& interval) {
    return os << "[" << interval.min << ", " << interval.max << "]";
}
