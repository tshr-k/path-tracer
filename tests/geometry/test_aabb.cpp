#include <catch2/catch_test_macros.hpp>

#include "geometry/aabb.h"
#include "math/ray.h"
#include "math/point.h"
#include "math/vector3.h"
#include "math/math_utils.h"

TEST_CASE("AABB hit works", "[AABB]") {
    const AABB box{Point{0, 0, 0}, Point{1, 1, 1}};

    REQUIRE(box.hit(Ray{Point{-1, 0.5, 0.5}, Vector{1, 0, 0}}, math::Interval{0.001, math::infinity}));
    REQUIRE_FALSE(box.hit(Ray{Point{-1, -1, -1}, Vector{1, 0, 0}}, math::Interval{0.001, math::infinity}));
    REQUIRE_FALSE(box.hit(Ray{Point{-1, 2, 2}, Vector{1, 0, 0}}, math::Interval{0.001, math::infinity}));
}

TEST_CASE("AABB surrounding box works", "[AABB]") {
    const AABB box1{Point{0, 0, 0}, Point{1, 1, 1}};
    const AABB box2{Point{-1, -1, -1}, Point{0.5, 0.5, 0.5}};

    const AABB surrounding_box = AABB::merge(box1, box2);

    REQUIRE(math::almost_equal(surrounding_box.x().min, -1));
    REQUIRE(math::almost_equal(surrounding_box.y().min, -1));
    REQUIRE(math::almost_equal(surrounding_box.z().min, -1));

    REQUIRE(math::almost_equal(surrounding_box.x().max, 1));
    REQUIRE(math::almost_equal(surrounding_box.y().max, 1));
    REQUIRE(math::almost_equal(surrounding_box.z().max, 1));
}
