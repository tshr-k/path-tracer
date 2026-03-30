#pragma once

#include "ray.h"
#include "point.h"
#include "vector3.h"
#include "transform.h"

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
   
    Camera(double aspect_ratio, double focal_length, double horizontal_fov)
        : aspect_ratio_{aspect_ratio}, focal_length_{focal_length},
          viewport_width_{2 * focal_length * std::tan(0.5*horizontal_fov)}, viewport_height_{viewport_width_/aspect_ratio},
          u_{viewport_width_, 0, 0}, v_{0, -viewport_height_, 0},
          viewport_top_left_{-0.5*viewport_width_, 0.5*viewport_height_, -focal_length} {}

    Ray cast_ray(double u, double v) const {
        const Point local_target{viewport_top_left_ + u*u_ + v*v_};
        return T.local_to_global(Ray{{0, 0, 0}, local_target});
    }
};