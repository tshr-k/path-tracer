#pragma once

#include "math/math_utils.h"

class Scene;
class Sampler;
class Colour;
class Ray;

class Tracer {
private:
    const Scene& scene_;

    Colour ray_colour(const Sampler& sampler, const Ray& ray, int bounce_depth, const math::Interval t_range, Colour& throughput) const;

public:
    Tracer(const Scene& scene);

    void render(const Sampler& sampler, int image_height, int image_width, int max_bounce_depth, const math::Interval t_range) const;
};
