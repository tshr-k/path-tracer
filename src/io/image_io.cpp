#include "image_io.h"

#include <ostream>

#include "math/colour.h"
#include "math/math_utils.h"

namespace io {
    namespace ppm {
        void write_header(std::ostream& out, int image_height, int image_width) {
            out << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        }

        void write_pixel(std::ostream& out, const Colour& pixel_colour) {
            const double r = math::gamma_correct(pixel_colour.r());
            const double g = math::gamma_correct(pixel_colour.g());
            const double b = math::gamma_correct(pixel_colour.b());

            // Map [0, 1] to {0, 1, ..., 255}
            const int r_byte = int(255.999 * r);
            const int g_byte = int(255.999 * g);
            const int b_byte = int(255.999 * b);

            out << r_byte << ' ' << g_byte << ' ' << b_byte << '\n';
        }
    }
}
