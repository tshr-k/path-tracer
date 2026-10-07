#pragma once

#include "math/transform.h"

class Ray;

class Camera {
private:
    const double aspect_ratio_;
    const double focal_length_;

    const double viewport_width_;
    const double viewport_height_;

    const Vector u_;
    const Vector v_;
    const Point viewport_top_left_;

public:
    Transform T;

    Camera(double aspect_ratio, double focal_length, double horizontal_fov);

    Ray cast_ray(double u, double v) const;
};
