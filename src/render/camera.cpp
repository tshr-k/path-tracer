#include "camera.h"

#include <cmath>

#include "math/ray.h"

Camera::Camera(double aspect_ratio, double focal_length, double horizontal_fov)
    : aspect_ratio_{aspect_ratio}, focal_length_{focal_length},
      viewport_width_{2 * focal_length * std::tan(0.5*horizontal_fov)}, viewport_height_{viewport_width_/aspect_ratio},
      u_{viewport_width_, 0, 0}, v_{0, -viewport_height_, 0},
      viewport_top_left_{-0.5*viewport_width_, 0.5*viewport_height_, -focal_length} {}

Ray Camera::cast_ray(double u, double v) const {
    const Point local_target{viewport_top_left_ + u*u_ + v*v_};
    return T.local_to_global(Ray{{0, 0, 0}, local_target});
}
