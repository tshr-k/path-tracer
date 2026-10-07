#pragma once

#include <array>
#include <cstddef>
#include <iosfwd>

class UnitVector;

class Colour;
Colour operator*(double mu, const Colour& c);

class Colour {
private:
    std::array<double, 3> c_;

public:
    Colour();
    Colour(double r, double g, double b);

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

    [[nodiscard]] static const Colour clamp(const Colour& c);

    static const Colour from_unit_vector(const UnitVector& v);

    double r() const;
    double g() const;
    double b() const;

    double operator[](std::size_t i) const;
    double& operator[](std::size_t i);

    Colour& operator+=(const Colour& d);
    Colour& operator-=(const Colour& d);
    Colour& operator*=(const Colour& d);
    Colour& operator*=(double mu);
    Colour& operator/=(double mu);

    [[nodiscard]] double max_component() const;
};

std::ostream& operator<<(std::ostream& out, const Colour& d);

Colour operator+(Colour c, const Colour& d);
Colour operator-(Colour c, const Colour& d);
Colour operator*(Colour c, const Colour& d);
Colour operator*(Colour c, double mu);
Colour operator/(Colour c, double mu);

namespace math {
    [[nodiscard]] bool almost_zero(const Colour& c);
    [[nodiscard]] bool almost_equal(const Colour& c, const Colour& d);

    [[nodiscard]] double gamma_correct(double u);
}
