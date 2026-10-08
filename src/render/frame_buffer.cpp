#include "frame_buffer.h"

#include <vector>

#include "math/colour.h"

FrameBuffer::FrameBuffer(int width, int height)
    : width_{width}, height_{height}, buffer_{std::vector<Colour>(width*height)} {}

int FrameBuffer::width() const {
    return width_;
}

int FrameBuffer::height() const {
    return height_;
}

Colour FrameBuffer::operator()(int row, int col) const {
    return buffer_[width_ * row + col];
}


Colour& FrameBuffer::operator()(int row, int col) {
    return buffer_[width_ * row + col];
}

void io::ppm::write_image(std::ostream& out, const FrameBuffer& frame_buffer) {
    int width = frame_buffer.width();
    int height = frame_buffer.height();
    write_header(out, height, width);

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            Colour c = frame_buffer(row, col);
            io::ppm::write_pixel(out, Colour::clamp(c));
        }
    }
}