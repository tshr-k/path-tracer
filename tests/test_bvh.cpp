#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include "../src/aabb.h"
#include "../src/unit_sphere.h"
#include "../src/instance.h"
#include "../src/material.h"
#include "../src/bvh.h"
#include "../src/tlas.h"
#include "../src/blas.h"
#include "../src/vector3.h"
#include "../src/ray.h"
#include "../src/math_utils.h"
#include "../src/hittable.h"
#include "../src/mesh_io.h"
#include "../src/mesh.h"

TEST_CASE("TLAS construction and hit testing works", "[BVH]") {
    // Create a simple scene with 4 instances of UnitSphere at different positions
    std::vector<Instance> instances;
    auto material = DiffuseMaterial{Colour{0.5, 0.5, 0.5}};
    auto sphere = std::make_unique<UnitSphere>();
    for (int i = 0; i < 4; ++i) {
        Transform T;
        T.set_position({i * 2.0, 0, 0});  // Place unit spheres at (0,0,0), (2,0,0), (4,0,0), (6,0,0)
        instances.emplace_back(sphere.get(), 0);
        instances.back().set_transform(T);
    }

    TLAS tlas;
    tlas.build_bvh(instances);
    REQUIRE(tlas.num_nodes() == 7);

    // Test that rays hit the correct spheres
    for (int i = 0; i < 4; ++i) {
        Ray ray{Point{i * 2.0, -5, 0}, Vector{0, 1, 0}};  // Ray pointing upwards towards the spheres
        HitRecord hit_record;
        bool hit = tlas.hit(ray, hit_record, 0.001);
        REQUIRE(hit);
        REQUIRE(math::almost_equal(hit_record.point, Point{i * 2.0, -1, 0}));  // Should hit the sphere at (i*2,0,0) at y=-1
    }
}

TEST_CASE("BLAS construction and hit testing works", "[BVH]") {
    std::string obj_data = "# Blender 3.5.1\n# www.blender.org\no Cube\nv 1.000000 1.000000 -1.000000\nv 1.000000 -1.000000 -1.000000\nv 1.000000 1.000000 1.000000\nv 1.000000 -1.000000 1.000000\nv -1.000000 1.000000 -1.000000\nv -1.000000 -1.000000 -1.000000\nv -1.000000 1.000000 1.000000\nv -1.000000 -1.000000 1.000000\nvn -0.0000 1.0000 -0.0000\nvn -0.0000 -0.0000 1.0000a\nvn -1.0000 -0.0000 -0.0000\nvn -0.0000 -1.0000 -0.0000\nvn 1.0000 -0.0000 -0.0000\nvn -0.0000 -0.0000 -1.0000\nvt 0.625000 0.500000\nvt 0.375000 0.500000\nvt 0.625000 0.750000\nvt 0.375000 0.750000\nvt 0.875000 0.500000\nvt 0.625000 0.250000\nvt 0.125000 0.500000\nvt 0.375000 0.250000\nvt 0.875000 0.750000\nvt 0.625000 1.000000\nvt 0.625000 0.000000\nvt 0.375000 0.000000\nvt 0.375000 1.000000\nvt 0.125000 0.750000\ns 0\nf 5/5/1 3/3/1 1/1/1\nf 3/3/2 8/13/2 4/4/2\nf 7/11/3 6/8/3 8/12/3\nf 2/2/4 8/14/4 6/7/4\nf 1/1/5 4/4/5 2/2/5\nf 5/6/6 2/2/6 6/8/6\nf 5/5/1 7/9/1 3/3/1\nf 3/3/2 7/10/2 8/13/2\nf 7/11/3 5/6/3 6/8/3\nf 2/2/4 4/4/4 8/14/4\nf 1/1/5 3/3/5 4/4/5\nf 5/6/6 1/1/6 2/2/6\n";
    std::stringstream in{obj_data};

    const BLAS blas(std::make_shared<Mesh>(io::obj::load_mesh(in)));
    REQUIRE(blas.num_nodes() > 12); // Should have more than 12 nodes for 12 triangles, due to internal nodes

    // Test that rays hit the correct triangles
    const Ray ray_1{Point{0.5, 0.5, -5}, Vector{0, 0, 1}};  // Ray pointing towards the square
    HitRecord hit_record_1;
    REQUIRE(blas.hit(ray_1, hit_record_1, 0.001));
    REQUIRE(math::almost_equal(hit_record_1.point, Point{0.5, 0.5, -1}));  // Should hit the square at z=-1

    const Ray ray_2{Point{0.5, 0.5, -5}, Vector{0, 0, -1}};  // Ray missing the square
    HitRecord hit_record_2;
    REQUIRE_FALSE(blas.hit(ray_2, hit_record_2, 0.001));

}

TEST_CASE("Triangle intersections work correctly", "[Triangle]") {
    // Create a simple triangle and test ray intersections
    Mesh mesh;
    mesh.vertices = {Point{0, 0, 0}, Point{1, 0, 0}, Point{0, 1, 0}};
    mesh.triangles = {Triangle{0, 1, 2}};

    // Ray pointing towards the triangle
    Ray ray_1{Point{0.25, 0.25, -1}, Vector{0, 0, 1}};
    HitRecord hit_record_1;
    REQUIRE(hit_triangle(ray_1, 0, &mesh, hit_record_1, 0.001));
    REQUIRE(math::almost_equal(hit_record_1.point, Point{0.25, 0.25, 0}));

    // Ray missing the triangle
    Ray ray_2{Point{-1, -1, -1}, Vector{0, 0, 1}};
    HitRecord hit_record_2;
    REQUIRE_FALSE(hit_triangle(ray_2, 0, &mesh, hit_record_2, 0.001));
}