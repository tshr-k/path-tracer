#pragma once

#include <array>
#include <cstddef>
#include <iosfwd>
#include <limits>
#include <vector>

#include "math/point.h"
#include "math/vector3.h"


struct TriangleRange {
    std::size_t start_idx;
    std::size_t num_triangles;
};

constexpr std::size_t null_normal = std::numeric_limits<std::size_t>::max();

struct Triangle {
    std::array<std::size_t, 3> v_indices;
    std::array<std::size_t, 3> vn_indices;
    bool shade_smooth = false;

    Triangle(std::size_t v_0_index, std::size_t v_1_index, std::size_t v_2_index,
             std::size_t vn_0_index = null_normal, std::size_t vn_1_index = null_normal, std::size_t vn_2_index = null_normal);
};


class Mesh {
public:
    std::vector<Point> vertices;
    std::vector<Vector> vertex_normals;
    std::vector<Triangle> triangles;

    Mesh() = default;
};

std::ostream& operator<<(std::ostream& os, const Mesh& mesh);
