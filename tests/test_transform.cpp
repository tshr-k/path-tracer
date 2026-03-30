#include <catch2/catch_test_macros.hpp>

#include "../src/vector3.h"
#include "../src/point.h"
#include "../src/matrix4.h"
#include "../src/transform.h"
#include "../src/math_utils.h"

TEST_CASE("Transform identity leaves points and vectors unchanged", "[Transform]") {
    const Transform t;

    const Point p{1, -2, 3};
    const Vector v{-4, 5, -6};

    REQUIRE(math::almost_equal(t.local_to_global(p), p));
    REQUIRE(math::almost_equal(t.global_to_local(p), p));
    REQUIRE(math::almost_equal(t.local_to_global(v), v));
    REQUIRE(math::almost_equal(t.global_to_local(v), v));
}

TEST_CASE("Transform translation affects points but not vectors", "[Transform]") {
    const Transform t = Transform().set_position({1, 2, 3});

    const Point p{1, 0, 0};
    const Vector v{0, 1, 0};

    REQUIRE(math::almost_equal(t.local_to_global(p), Point{2, 2, 3}));
    REQUIRE(math::almost_equal(t.local_to_global(v), v));
}

TEST_CASE("Transform scaling affects points and vectors", "[Transform]") {
    const Transform t = Transform().set_scale({2, 3, 4});

    const Point p{1, -1, 0.5};
    const Vector v{-2, 1, 0.25};

    REQUIRE(math::almost_equal(t.local_to_global(p), Point{2, -3, 2}));
    REQUIRE(math::almost_equal(t.local_to_global(v), Vector{-4, 3, 1}));
}

TEST_CASE("Transform rotates around X axis", "[Transform]") {
    // 90-degree rotation around +X
    const Transform t = Transform().set_rotation({0, math::pi / 2, 0});

    const Point p{0, 1, 0};
    const Vector v{0, 0, 1};

    // Around X: (y, z) -> (0, 1) becomes (0, 0, 1) for point, and (0, -1, 0) for (0,0,1)
    REQUIRE(math::almost_equal(t.local_to_global(p), Point{0, 0, 1}));
    REQUIRE(math::almost_equal(t.local_to_global(v), Vector{0, -1, 0}));
}

TEST_CASE("Transform rotates around Y axis", "[Transform]") {
    // 90-degree rotation around +Y
    const Transform t = Transform().set_rotation({0, 0, math::pi / 2});

    const Point p{1, 0, 0};
    const Vector v{0, 0, 1};

    // Around Y: (x, z) -> (1,0) becomes (0, 0, -1) and (0,0,1) becomes (1,0,0)
    REQUIRE(math::almost_equal(t.local_to_global(p), Point{0, 0, -1}));
    REQUIRE(math::almost_equal(t.local_to_global(v), Vector{1, 0, 0}));
}

TEST_CASE("Transform rotates around Z axis", "[Transform]") {
    // 90-degree rotation around +Z
    const Transform t = Transform().set_rotation({math::pi / 2, 0, 0});

    const Point p{1, 0, 0};
    const Vector v{0, 1, 0};

    REQUIRE(math::almost_equal(t.local_to_global(p), Point{0, 1, 0}));
    REQUIRE(math::almost_equal(t.local_to_global(v), Vector{-1, 0, 0}));
}

TEST_CASE("Transform correctly combines rotation, scale, and translation", "[Transform]") {
    // Rotation: 45 degrees around X, then scale by 2 uniformly, then translate by (1,2,3)
    const Transform t = Transform().set_rotation({0, math::pi / 4, 0}).set_position({1, 2, 3}).set_scale({2, 2, 2});

    const Point p{1, 0, 0};
    const Vector v{0, 1, 0};

    const Point transformed_p = t.local_to_global(p);
    const Vector transformed_v = t.local_to_global(v);

    // Scale then rotate around X leaves (1,0,0) on X axis, then translate
    REQUIRE(math::almost_equal(transformed_p, Point{3, 2, 3}));

    // (0,1,0) scaled by 2 then rotated around X by 45 degrees
    const double r2 = std::sqrt(2.0);
    REQUIRE(math::almost_equal(transformed_v, Vector{0, r2, r2}));
}

TEST_CASE("Transform correctly inverts transformations", "[Transform]") {
    const Transform t = Transform().set_rotation({0, math::pi / 4, 0}).set_position({1, 2, 3}).set_scale({2, 2, 2});

    const Point p{1, 5, -3};
    const Vector v{0, 1, 9};

    const Point transformed_p = t.local_to_global(p);
    const Vector transformed_v = t.local_to_global(v);

    REQUIRE(math::almost_equal(t.global_to_local(transformed_p), p));
    REQUIRE(math::almost_equal(t.global_to_local(transformed_v), v));
}

TEST_CASE("Transform correctly transforms normals", "[Transform]") {
    const Transform t = Transform().set_rotation({0, math::pi / 4, 0}).set_position({1, 2, 3}).set_scale({2, 2, 2});

    const Vector normal{0, 0, 1};

    const Vector transformed_normal = t.local_to_global_normal(normal);

    const double r2 = std::sqrt(2.0);
    // For uniform scale, normals are rotated and scaled by 1/2
    REQUIRE(math::almost_equal(transformed_normal, Vector{0, -r2 / 4.0, r2 / 4.0}));
    REQUIRE(math::almost_equal(t.global_to_local_normal(transformed_normal), normal));
}

TEST_CASE("Transform correctly transforms rays", "[Transform]") {
    const Transform t = Transform().set_rotation({0, math::pi / 4, 0}).set_position({1, 2, 3}).set_scale({2, 2, 2});

    const Ray ray{Point{1, 0, 0}, Vector{0, 1, 0}};

    const Ray transformed_ray = t.local_to_global(ray);

    const double r2 = std::sqrt(2.0);

    REQUIRE(math::almost_equal(transformed_ray.origin(), Point{3, 2, 3}));
    REQUIRE(math::almost_equal(transformed_ray.direction(), Vector{0, r2, r2}));
    REQUIRE(math::almost_equal(t.global_to_local(transformed_ray).origin(), ray.origin()));
    REQUIRE(math::almost_equal(t.global_to_local(transformed_ray).direction(), ray.direction()));
}