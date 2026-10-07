#include <catch2/catch_test_macros.hpp>

#include "math/vector3.h"
#include "math/point.h"
#include "math/ray.h"
#include "geometry/unit_sphere.h"
#include "math/math_utils.h"

TEST_CASE("UnitSphere hit function works for rays originating inside the sphere", "[UnitSphere]") {
    UnitSphere sphere;

    Point origin{0.1, 0, 0.3};

    for (int i = 0; i < 10; i++) {
        UnitVector direction = UnitVector::random_sphere();
        Ray ray{origin, direction};
        HitRecord hit_record;

        double t_min = 0.001;
        REQUIRE(sphere.hit(ray, hit_record, t_min));
        REQUIRE(hit_record.t > 0.001);
        REQUIRE(math::almost_equal(hit_record.point, ray.at(hit_record.t)));
        REQUIRE(math::almost_equal(hit_record.normal, unit_vector(hit_record.point.r())));
    }
}
