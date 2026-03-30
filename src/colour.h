#pragma once

#include <array>
#include <cstddef>
#include <ostream>

#include "vector3.h"
#include "math_utils.h"

class Colour;

constexpr Colour operator*(double mu, const Colour& c);

class Colour {
private:
    std::array<double, 3> c_;

public:
    constexpr Colour() : c_{0, 0, 0} {}
    constexpr Colour(double r, double g, double b) : c_{r, g, b} {}

    static const Colour white;
    static const Colour light_grey;
    static const Colour dark_grey;
    static const Colour black;

    static const Colour red;
    static const Colour green;
    static const Colour blue;
    static const Colour cyan;
    static const Colour magenta;
    static const Colour yellow;
    static const Colour orange;
    static const Colour purple;

    [[nodiscard]] static const Colour clamp(const Colour& c) {
        return Colour(
            std::clamp(c.r(), 0.0, 1.0),
            std::clamp(c.g(), 0.0, 1.0),
            std::clamp(c.b(), 0.0, 1.0)
        );
    }

    static const Colour from_unit_vector(const UnitVector& v) {
        return 0.5 * Colour(v.x()+1, v.y()+1, v.z()+1);
    }

    constexpr double r() const { return c_[0]; }
    constexpr double g() const { return c_[1]; }
    constexpr double b() const { return c_[2]; }

    constexpr double operator[](std::size_t i) const { return c_[i]; }
    constexpr double& operator[](std::size_t i) { return c_[i]; }

    constexpr Colour& operator+=(const Colour& d) {
        c_[0] += d.r();
        c_[1] += d.g();
        c_[2] += d.b();
        return *this;
    }

    constexpr Colour& operator-=(const Colour& d) {
        c_[0] -= d.r();
        c_[1] -= d.g();
        c_[2] -= d.b();
        return *this;
    }

    constexpr Colour& operator*=(const Colour& d) {
        c_[0] *= d.r();
        c_[1] *= d.g();
        c_[2] *= d.b();
        return *this;
    }

    constexpr Colour& operator*=(double mu) {
        c_[0] *= mu;
        c_[1] *= mu;
        c_[2] *= mu;
        return *this;
    }

    constexpr Colour& operator/=(double mu) {
        return (*this) *= (1.0/mu);
    }

    [[nodiscard]] constexpr double max_component() const {
        return std::max({r(), g(), b()});
    }
};

inline constexpr Colour Colour::white{1, 1, 1};
inline constexpr Colour Colour::light_grey{0.8, 0.8, 0.8};
inline constexpr Colour Colour::dark_grey{0.2, 0.2, 0.2};
inline constexpr Colour Colour::black{0, 0, 0};
inline constexpr Colour Colour::red{1, 0, 0};
inline constexpr Colour Colour::green{0, 1, 0};
inline constexpr Colour Colour::blue{0, 0, 1};
inline constexpr Colour Colour::cyan{0, 1, 1};
inline constexpr Colour Colour::magenta{1, 0, 1};
inline constexpr Colour Colour::yellow{1, 1, 0};
inline constexpr Colour Colour::orange{1, 0.5, 0};
inline constexpr Colour Colour::purple{0.5, 0, 0.5};

inline std::ostream& operator<<(std::ostream& out, const Colour& d) {
    return out << d.r() << ' ' << d.g() << ' ' << d.b();
}

constexpr Colour operator+(Colour c, const Colour& d) {
    return c += d;
}

constexpr Colour operator-(Colour c, const Colour& d) {
    return c -= d;
}

constexpr Colour operator*(Colour c, const Colour& d) {
    return c *= d;
}

constexpr Colour operator*(Colour c, double mu) {
    return c *= mu;
}

constexpr Colour operator*(double mu, const Colour& c) {
    return c * mu;
}

constexpr Colour operator/(Colour c, double mu) {
    return c /= mu;
}

namespace math {
    [[nodiscard]] inline bool almost_zero(const Colour& c) {
        return almost_zero(c.r()) && almost_zero(c.g()) && almost_zero(c.b());
    }

    [[nodiscard]] inline bool almost_equal(const Colour& c, const Colour& d) {
        return almost_zero(d - c);
    }

    [[nodiscard]] inline double gamma_correct(double u) {
        return std::sqrt(u);  // Approximate gamma correction with gamma = 2.0
    }
}