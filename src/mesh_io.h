#pragma once

#include <array>
#include <cstddef>
#include <fstream>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "mesh.h"

namespace io {
    namespace obj {
        inline Mesh load_mesh(std::istream& in) {
            Mesh mesh;

            std::string line;
            while (std::getline(in, line)) {
                std::istringstream iss{line};
                std::string prefix;
                iss >> prefix;

                if (prefix == "v") {
                    double x, y, z;
                    iss >> x >> y >> z;
                    mesh.vertices.emplace_back(x, y, z);
                } else if (prefix == "vn") {
                    double x, y, z;
                    iss >> x >> y >> z;
                    mesh.vertex_normals.emplace_back(x, y, z);
                } else if (prefix == "f") {
                    std::array<std::size_t, 3> v_indices;
                    std::array<std::size_t, 3> vn_indices;

                    for (size_t i = 0; i < 3; i++){
                        std::string vertex_info;
                        iss >> vertex_info;

                        size_t v_index = 0, vt_index = 0, vn_index = 0;
                        std::sscanf(vertex_info.c_str(), "%zu/%zu/%zu", &v_index, &vt_index, &vn_index);  // TODO: Handle 1//1 case
                        v_indices[i] = v_index - 1;  // OBJ format is 1-indexed
                        vn_indices[i] = vn_index - 1;
                    }
                    mesh.triangles.emplace_back(v_indices[0], v_indices[1], v_indices[2], vn_indices[0], vn_indices[1], vn_indices[2]);
                }
            }

            return mesh;
        }

        inline Mesh load_mesh(const std::string& filename) {
            std::ifstream in{filename};
            if (!in) {
                throw std::runtime_error("Failed to open file: " + filename);
            }
            return load_mesh(in);
        }
    }
}