#include <catch2/catch_test_macros.hpp>

#include "render/frame_buffer.h"
#include "math/colour.h"
#include "math/math_utils.h"

TEST_CASE("FrameBuffer reports its dimensions", "[FrameBuffer]") {
    const FrameBuffer frame_buffer{4, 3};

    REQUIRE(frame_buffer.width() == 4);
    REQUIRE(frame_buffer.height() == 3);
}

TEST_CASE("FrameBuffer pixels default-initialise to black", "[FrameBuffer]") {
    const FrameBuffer frame_buffer{3, 2};

    for (int row = 0; row < frame_buffer.height(); row++) {
        for (int col = 0; col < frame_buffer.width(); col++) {
            REQUIRE(math::almost_equal(frame_buffer(row, col), Colour::black));
        }
    }
}

TEST_CASE("FrameBuffer round-trips written pixel values", "[FrameBuffer]") {
    FrameBuffer frame_buffer{4, 3};

    frame_buffer(0, 0) = Colour::red;
    frame_buffer(2, 3) = Colour::green;
    frame_buffer(1, 1) = Colour::blue;

    const FrameBuffer& const_frame_buffer = frame_buffer;
    REQUIRE(math::almost_equal(const_frame_buffer(0, 0), Colour::red));
    REQUIRE(math::almost_equal(const_frame_buffer(2, 3), Colour::green));
    REQUIRE(math::almost_equal(const_frame_buffer(1, 1), Colour::blue));

    // Untouched pixels should be unaffected by the writes above
    REQUIRE(math::almost_equal(const_frame_buffer(0, 1), Colour::black));
}

TEST_CASE("FrameBuffer indexes pixels in row-major order", "[FrameBuffer]") {
    // Non-square so a swapped row/col (or width/height) mix-up would misplace writes
    FrameBuffer frame_buffer{5, 2};

    frame_buffer(0, 1) = Colour::red;
    frame_buffer(1, 0) = Colour::blue;

    REQUIRE(math::almost_equal(frame_buffer(0, 1), Colour::red));
    REQUIRE(math::almost_equal(frame_buffer(1, 0), Colour::blue));

    // Every other pixel, including (0, 1)'s and (1, 0)'s neighbours, stays black
    for (int row = 0; row < frame_buffer.height(); row++) {
        for (int col = 0; col < frame_buffer.width(); col++) {
            if ((row == 0 && col == 1) || (row == 1 && col == 0)) {
                continue;
            }
            REQUIRE(math::almost_equal(frame_buffer(row, col), Colour::black));
        }
    }
}
