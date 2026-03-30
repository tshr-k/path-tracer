#include <catch2/catch_test_macros.hpp>

#include "../src/vector3.h"
#include "../src/point.h"
#include "../src/matrix4.h"
#include "../src/math_utils.h"
#include "../src/onb.h"

TEST_CASE("Comparison functions work", "[Maths]") {
    REQUIRE(math::almost_zero(-0.001, 0.0012));
    REQUIRE(math::almost_equal(3.0, 3.0000003));
    
    Vector v{8.43e-7, 3.2e-9, -9.87e-7};
    REQUIRE(math::almost_zero(v));

    Point p{2, 1e-10, 3e-9};
    Point q{2, 0, 3e-9};
    REQUIRE(math::almost_equal(p, q));

    REQUIRE_FALSE(math::almost_equal(v, p.r()));
}

TEST_CASE("Interval class works", "[Maths]") {
    math::Interval i{3, math::infinity};
    REQUIRE(i.contains(7));
    REQUIRE_FALSE(i.contains(-0.1));
    REQUIRE_FALSE(i.surrounds(3));
    REQUIRE(i.surrounds(3.00001));

    math::Interval j = i.expand(2);
    REQUIRE(j.contains(1));
}

TEST_CASE("Degrees to radians conversion works", "[Maths]") {
    REQUIRE(math::almost_equal(math::degrees_to_radians(0), 0));
    REQUIRE(math::almost_equal(math::degrees_to_radians(180), math::pi));
    REQUIRE(math::almost_equal(math::degrees_to_radians(360), 2*math::pi));
}

TEST_CASE("Vector algebra works", "[Vector]") {
    Vector v{1, 2, 3};
    Vector u{4, 5, 6};

    REQUIRE(math::almost_equal(v + u, Vector(5, 7, 9)));
    REQUIRE(math::almost_equal(v - u, Vector(-3, -3, -3)));
    REQUIRE(math::almost_equal(-v, Vector(-1, -2, -3)));

    REQUIRE(math::almost_equal(v * 2, Vector(2, 4, 6)));
    REQUIRE(math::almost_equal(v / 2, Vector(0.5, 1, 1.5)));

    REQUIRE(math::almost_equal(dot(v, u), 32));
    REQUIRE(math::almost_equal(cross(v, u), Vector(-3, 6, -3)));

    REQUIRE(math::almost_equal(v.length_sq(), 14));
    REQUIRE(math::almost_equal(v.length(), std::sqrt(14)));

    REQUIRE(math::almost_equal(unit_vector(v), Vector(1/std::sqrt(14), 2/std::sqrt(14), 3/std::sqrt(14))));
}

TEST_CASE("Matrix algebra works", "[Matrix]") {
    Matrix I{};
    Matrix S = Matrix::scale(2);
    Matrix T = Matrix::translation(1, 2, 3);

    Point p{1, 1, 1};
    REQUIRE(math::almost_equal(I * p, p));
    REQUIRE(math::almost_equal(S * p, Point(2, 2, 2)));
    REQUIRE(math::almost_equal(T * p, Point(2, 3, 4)));

    Matrix R = Matrix::rotation(math::pi / 2, UnitVector::e_z());
    REQUIRE(math::almost_equal(R * Point(1, 0, 0), Point(0, 1, 0)));

    REQUIRE(math::almost_equal(R * transpose(R), I));
}

TEST_CASE("ONB transformations work", "[ONB]") {
    ONB uvw{UnitVector::from_vector(Vector(1, 2, 2))};

    Vector a{1, 2, 2};
    REQUIRE(math::almost_equal(uvw.global_to_local(a), Vector(0, 0, 3)));
    
    Vector b{4, 1, 3};
    REQUIRE(math::almost_equal(uvw.local_to_global(uvw.global_to_local(b)), b));
}