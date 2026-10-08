#pragma once

#include <iosfwd>

class Colour;
class FrameBuffer;

namespace io {
    namespace ppm {
        void write_header(std::ostream& out, int image_height, int image_width);
        void write_pixel(std::ostream& out, const Colour& pixel_colour);
        void write_image(std::ostream& out, const FrameBuffer& frame_buffer);
    }
}
