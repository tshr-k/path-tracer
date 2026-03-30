#include <catch2/catch_test_macros.hpp>

#include "../src/sampler.h"

TEST_CASE("RandomSampler produces values in [0, 1)", "[Sampler]") {
    const RandomSampler sampler{100};
    for (int i = 0; i < 10; i++) {
        const double sample_1d = sampler.sample_1d();
        REQUIRE(sample_1d >= 0.0);
        REQUIRE(sample_1d < 1.0);

        const auto [x, y] = sampler.sample_2d();
        REQUIRE(x >= 0.0);
        REQUIRE(x < 1.0);
        REQUIRE(y >= 0.0);
        REQUIRE(y < 1.0);
    }
}

TEST_CASE("StratifiedSampler produces values in [0, 1)", "[Sampler]") {
    const StratifiedSampler sampler{100};
    for (int i = 0; i < 10; i++) {
        const double sample_1d = sampler.sample_1d();
        REQUIRE(sample_1d >= 0.0);
        REQUIRE(sample_1d < 1.0);

        const auto [x, y] = sampler.sample_2d();
        REQUIRE(x >= 0.0);
        REQUIRE(x < 1.0);
        REQUIRE(y >= 0.0);
        REQUIRE(y < 1.0);
    }
}