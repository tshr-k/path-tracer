#include <catch2/catch_test_macros.hpp>

#include <iostream>

#include "../src/vector3.h"
#include "../src/bsdf.h"

TEST_CASE("LambertianBRDF evaluates to albedo / pi for valid directions") {
    const Colour albedo{0.8, 0.1, 0.1};
    LambertianBRDF brdf{albedo};

    const UnitVector w_i = UnitVector::e_z();  // Above the surface
    const UnitVector w_o = UnitVector::e_z();  // Above the surface

    const Colour expected = albedo / math::pi;
    REQUIRE(math::almost_equal(brdf.evaluate(w_i, w_o), expected));
}

TEST_CASE("LambertianBRDF evaluates to zero for invalid directions") {
    const Colour albedo{0.8, 0.1, 0.1};
    LambertianBRDF brdf{albedo};

    const UnitVector w_i = -UnitVector::e_z();  // Below the surface
    const UnitVector w_o = UnitVector::e_z();  // Above the surface

    REQUIRE(math::almost_zero(brdf.evaluate(w_i, w_o)));
}

TEST_CASE("LambertianBRDF samples directions above the surface and has correct PDF") {
    const Colour albedo{0.8, 0.1, 0.1};
    LambertianBRDF brdf{albedo};

    const UnitVector w_o = UnitVector::e_z();  // Above the surface

    for (int i = 0; i < 10; i++) {
        double p = 0.0;
        const UnitVector w_i = brdf.sample(w_o, p);
        // Sampled direction should be above the surface
        REQUIRE(w_i.z() > 0);

        // PDF should match the evaluate function
        const double expected_p = w_i.z() / math::pi;
        REQUIRE(math::almost_equal(p, expected_p));
    }
}