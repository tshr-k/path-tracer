#pragma once

#include <iosfwd>
#include <string>

class Mesh;

namespace io {
    namespace obj {
        Mesh load_mesh(std::istream& in);
        Mesh load_mesh(const std::string& filename);
    }
}
