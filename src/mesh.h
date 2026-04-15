#pragma once

#include <array>
#include <cstddef>
#include <ostream>
#include <vector>

#include "point.h"
#include "vector3.h"


struct TriangleRange {
    std::size_t start_idx;
    std::size_t num_triangles;
};

constexpr std::size_t null_normal = std::numeric_limits<std::size_t>::max();

struct Triangle {
    std::array<std::size_t, 3> v_indices;
    std::array<std::size_t, 3> vn_indices;
    bool shade_smooth = false;

    constexpr Triangle(std::size_t v_0_index, std::size_t v_1_index, std::size_t v_2_index,
             std::size_t vn_0_index = null_normal, std::size_t vn_1_index = null_normal, std::size_t vn_2_index = null_normal)
        : v_indices{v_0_index, v_1_index, v_2_index}, vn_indices{vn_0_index, vn_1_index, vn_2_index},
          shade_smooth{(vn_0_index != null_normal) && (vn_1_index != null_normal) && (vn_2_index != null_normal)} {}  // Shade smooth if all 3 vertices have normals
};


class Mesh {
public:
    std::vector<Point> vertices;
    std::vector<Vector> vertex_normals;
    std::vector<Triangle> triangles;

    Mesh() = default;        
};

inline std::ostream& operator<<(std::ostream& os, const Mesh& mesh) {
    for (const auto v : mesh.vertices) {
        os << "v " << v.x() << " " << v.y() << " " << v.z() << "\n";
    }
    for (const auto vn : mesh.vertex_normals) {
        os << "vn " << vn.x() << " " << vn.y() << " " << vn.z() << "\n";
    }
    for (const auto t : mesh.triangles) {
        os << "f " << t.v_indices[0] + 1 << " " << t.v_indices[1] + 1 << " " << t.v_indices[2] + 1 << "\n";  // OBJ format is 1-indexed
    }
    return os;
}