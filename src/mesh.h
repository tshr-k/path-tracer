#pragma once

#include <cstddef>
#include <vector>

#include "vector3.h"
#include "point.h"


struct TriangleRange {
    std::size_t start_idx;
    std::size_t num_triangles;
};


struct Triangle {
    constexpr Triangle(std::size_t v_0, std::size_t v_1, std::size_t v_2)
        : v_0{v_0}, v_1{v_1}, v_2{v_2} {};

    std::size_t v_0, v_1, v_2;
};


class Mesh {
public:
    std::vector<Point> vertices;
    std::vector<Triangle> triangles;

    constexpr Mesh() = default;
};