#include <catch2/catch_test_macros.hpp>

#include "../src/vector3.h"
#include "../src/point.h"
#include "../src/camera.h"

TEST_CASE("Camera cast_ray produces rays in the correct direction", "[Camera]") {
    const double aspect_ratio = 16.0 / 9.0;
    const double focal_length = 1.0;

    const double h_fov = math::degrees_to_radians(90);
    const double viewport_width = 2 * focal_length * std::tan(0.5*h_fov);
    const double viewport_height = viewport_width / aspect_ratio;

    Camera camera{aspect_ratio, focal_length, h_fov};

    // Test center of the viewport
    Ray ray_center = camera.cast_ray(0.5, 0.5);
    REQUIRE(math::almost_equal(unit_vector(ray_center.direction()), -UnitVector::e_z()));

    // Test top-left corner of the viewport
    Ray ray_top_left = camera.cast_ray(0, 0);
    REQUIRE(math::almost_equal(unit_vector(ray_top_left.direction()), unit_vector({-viewport_width/2, viewport_height/2, -focal_length})));

    // Test bottom-right corner of the viewport
    Ray ray_bottom_right = camera.cast_ray(1, 1);
    REQUIRE(math::almost_equal(unit_vector(ray_bottom_right.direction()), unit_vector({viewport_width/2, -viewport_height/2, -focal_length})));
}