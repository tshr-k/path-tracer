#pragma once

#include <vector>

#include "math/colour.h"
#include "io/image_io.h"


class FrameBuffer {
private:
    int width_;
    int height_;

    std::vector<Colour> buffer_;  // row-major order

public:
    FrameBuffer(int width, int height);

    int width() const;
    int height() const;

    Colour operator()(int row, int col) const;
    Colour& operator()(int row, int col);
};
