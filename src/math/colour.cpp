#include "colour.h"

#include <algorithm>
#include <cmath>
#include <ostream>

#include "vector3.h"
#include "math_utils.h"

Colour::Colour() : c_{0, 0, 0} {}
Colour::Colour(double r, double g, double b) : c_{r, g, b} {}

const Colour Colour::white{1, 1, 1};
const Colour Colour::light_grey{0.8, 0.8, 0.8};
const Colour Colour::dark_grey{0.2, 0.2, 0.2};
const Colour Colour::black{0, 0, 0};

const Colour Colour::red{1, 0, 0};
const Colour Colour::green{0, 1, 0};
const Colour Colour::blue{0, 0, 1};
const Colour Colour::cyan{0, 1, 1};
const Colour Colour::magenta{1, 0, 1};
const Colour Colour::yellow{1, 1, 0};
const Colour Colour::orange{1, 0.5, 0};
const Colour Colour::purple{0.5, 0, 0.5};

const Colour Colour::clamp(const Colour& c) {
    return Colour(
        std::clamp(c.r(), 0.0, 1.0),
        std::clamp(c.g(), 0.0, 1.0),
        std::clamp(c.b(), 0.0, 1.0)
    );
}

const Colour Colour::from_unit_vector(const UnitVector& v) {
    return 0.5 * Colour(v.x()+1, v.y()+1, v.z()+1);
}

double Colour::r() const { return c_[0]; }
double Colour::g() const { return c_[1]; }
double Colour::b() const { return c_[2]; }

double Colour::operator[](std::size_t i) const { return c_[i]; }
double& Colour::operator[](std::size_t i) { return c_[i]; }

Colour& Colour::operator+=(const Colour& d) {
    c_[0] += d.r();
    c_[1] += d.g();
    c_[2] += d.b();
    return *this;
}

Colour& Colour::operator-=(const Colour& d) {
    c_[0] -= d.r();
    c_[1] -= d.g();
    c_[2] -= d.b();
    return *this;
}

Colour& Colour::operator*=(const Colour& d) {
    c_[0] *= d.r();
    c_[1] *= d.g();
    c_[2] *= d.b();
    return *this;
}

Colour& Colour::operator*=(double mu) {
    c_[0] *= mu;
    c_[1] *= mu;
    c_[2] *= mu;
    return *this;
}

Colour& Colour::operator/=(double mu) {
    return (*this) *= (1.0/mu);
}

double Colour::max_component() const {
    return std::max({r(), g(), b()});
}

std::ostream& operator<<(std::ostream& out, const Colour& d) {
    return out << d.r() << ' ' << d.g() << ' ' << d.b();
}

Colour operator+(Colour c, const Colour& d) {
    return c += d;
}

Colour operator-(Colour c, const Colour& d) {
    return c -= d;
}

Colour operator*(Colour c, const Colour& d) {
    return c *= d;
}

Colour operator*(Colour c, double mu) {
    return c *= mu;
}

Colour operator*(double mu, const Colour& c) {
    return c * mu;
}

Colour operator/(Colour c, double mu) {
    return c /= mu;
}

namespace math {
    bool almost_zero(const Colour& c) {
        return almost_zero(c.r()) && almost_zero(c.g()) && almost_zero(c.b());
    }

    bool almost_equal(const Colour& c, const Colour& d) {
        return almost_zero(d - c);
    }

    double gamma_correct(double u) {
        return std::sqrt(u);  // Approximate gamma correction with gamma = 2.0
    }
}
