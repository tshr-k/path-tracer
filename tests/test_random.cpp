#include <catch2/catch_test_macros.hpp>

#include "../src/vector3.h"

TEST_CASE("Random number generation works", "[Maths]") {
    for (int i = 0; i < 10; i++) {
        const double r = math::random();  // [0, 1)
        REQUIRE(r >= 0.0);
        REQUIRE(r < 1.0);
    }
}

TEST_CASE("Random unit vector generation works", "[Maths]") {
    for (int i = 0; i < 10; i++) {
        const UnitVector v = UnitVector::random_sphere();
        REQUIRE(math::almost_equal(length(v), 1.0));
    }
}

TEST_CASE("Random hemisphere vector generation works", "[Maths]") {
    for (int i = 0; i < 10; i++) {
        const UnitVector v = UnitVector::random_hemisphere();
        REQUIRE(math::almost_equal(length(v), 1.0));
        REQUIRE(v.z() >= 0.0);  // Upper hemisphere only
    }
}

TEST_CASE("Random cosine-weighted hemisphere vector generation works", "[Maths]") {
    for (int i = 0; i < 10; i++) {
        const UnitVector v = UnitVector::random_cosine_hemisphere();
        REQUIRE(math::almost_equal(length(v), 1.0));
        REQUIRE(v.z() >= 0.0);  // Upper hemisphere only
    }
}