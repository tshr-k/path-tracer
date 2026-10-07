#include "mesh.h"

#include <ostream>

Triangle::Triangle(std::size_t v_0_index, std::size_t v_1_index, std::size_t v_2_index,
                    std::size_t vn_0_index, std::size_t vn_1_index, std::size_t vn_2_index)
    : v_indices{v_0_index, v_1_index, v_2_index}, vn_indices{vn_0_index, vn_1_index, vn_2_index},
      shade_smooth{(vn_0_index != null_normal) && (vn_1_index != null_normal) && (vn_2_index != null_normal)} {}  // Shade smooth if all 3 vertices have normals

std::ostream& operator<<(std::ostream& os, const Mesh& mesh) {
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
