#pragma once

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
                } else if (prefix == "f") {
                    std::size_t v_0, v_1, v_2;
                    iss >> v_0 >> v_1 >> v_2;
                    // OBJ format uses 1-based indexing
                    mesh.triangles.push_back({v_0 - 1, v_1 - 1, v_2 - 1});
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