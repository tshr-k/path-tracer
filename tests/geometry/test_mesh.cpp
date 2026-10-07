#include "catch2/catch_test_macros.hpp"

#include <string>
#include <sstream>

#include "io/mesh_io.h"
#include "geometry/mesh.h"


TEST_CASE("Mesh can be loaded from an OBJ formatted string", "[Mesh]") {
    std::string smooth_cube_obj = "# Blender 3.5.1\n# www.blender.org\no Cube\nv 1.000000 1.000000 -1.000000\nv 1.000000 -1.000000 -1.000000\nv 1.000000 1.000000 1.000000\nv 1.000000 -1.000000 1.000000\nv -1.000000 1.000000 -1.000000\nv -1.000000 -1.000000 -1.000000\nv -1.000000 1.000000 1.000000\nv -1.000000 -1.000000 1.000000\nvn -0.5774 0.5774 -0.5774\nvn 0.5774 0.5774 0.5774\nvn 0.5774 0.5774 -0.5774\nvn -0.5774 -0.5774 0.5774\nvn 0.5774 -0.5774 0.5774\nvn -0.5774 0.5774 0.5774\nvn -0.5774 -0.5774 -0.5774\nvn 0.5774 -0.5774 -0.5774\nvt 0.625000 0.500000\nvt 0.375000 0.500000\nvt 0.625000 0.750000\nvt 0.375000 0.750000\nvt 0.875000 0.500000\nvt 0.625000 0.250000\nvt 0.125000 0.500000\nvt 0.375000 0.250000\nvt 0.875000 0.750000\nvt 0.625000 1.000000\nvt 0.625000 0.000000\nvt 0.375000 0.000000\nvt 0.375000 1.000000\nvt 0.125000 0.750000\ns 1\nf 5/5/1 3/3/2 1/1/3\nf 3/3/2 8/13/4 4/4/5\nf 7/11/6 6/8/7 8/12/4\nf 2/2/8 8/14/4 6/7/7\nf 1/1/3 4/4/5 2/2/8\nf 5/6/1 2/2/8 6/8/7\nf 5/5/1 7/9/6 3/3/2\nf 3/3/2 7/10/6 8/13/4\nf 7/11/6 5/6/1 6/8/7\nf 2/2/8 4/4/5 8/14/4\nf 1/1/3 3/3/2 4/4/5\nf 5/6/1 1/1/3 2/2/8\n";
    std::stringstream in{smooth_cube_obj};

    Mesh mesh = io::obj::load_mesh(in);

    REQUIRE(mesh.vertices.size() == 8);
    REQUIRE(mesh.vertex_normals.size() == 8);
    REQUIRE(mesh.triangles.size() == 12);

    // Check the first triangle: f 5/5/1 3/3/2 1/1/3
    REQUIRE(mesh.triangles[0].v_indices[0] == 4);
    REQUIRE(mesh.triangles[0].v_indices[1] == 2);
    REQUIRE(mesh.triangles[0].v_indices[2] == 0);
    REQUIRE(mesh.triangles[0].shade_smooth);
    REQUIRE(mesh.triangles[0].vn_indices[0] == 0);
    REQUIRE(mesh.triangles[0].vn_indices[1] == 1);
    REQUIRE(mesh.triangles[0].vn_indices[2] == 2);
}
