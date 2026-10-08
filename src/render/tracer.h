#pragma once

#include "math/math_utils.h"
#include "render/frame_buffer.h"


class Scene;
class Sampler;
class Colour;
class Ray;

struct Tile {
    int row_0, col_0;
    int row_1, col_1;
};

class Tracer {
private:
    const Scene& scene_;

    Colour ray_colour(const Sampler& sampler, const Ray& ray, int bounce_depth, const math::Interval t_range, Colour& throughput) const;

public:
    Tracer(const Scene& scene);

    void render_pixel(const Sampler& sampler, int row, int col, int image_width, int image_height, int max_bounce_depth, const math::Interval &t_range, FrameBuffer& frame_buffer) const;

    void render_tile(const Tile& tile, const Sampler& sampler, int image_width, int image_height, int max_bounce_depth, const math::Interval &t_range, FrameBuffer& frame_buffer) const;

    void render(const Sampler& sampler, int image_width, int image_height, int tile_size, int num_threads, int max_bounce_depth, const math::Interval t_range, FrameBuffer& frame_buffer) const;
};
