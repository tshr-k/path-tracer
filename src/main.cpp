#include <iostream>

#include "colour.h"
#include "camera.h"
#include "image_io.h"
#include "mesh.h"
#include "mesh_io.h"
#include "material.h"
#include "unit_sphere.h"
#include "scene.h"
#include "sampler.h"
#include "tracer.h"
#include "math_utils.h"

int main() {
    const int image_height = 512;
    const int image_width = 512;
    io::ppm::write_header(std::cout, image_height, image_width);

    Scene scene;
    scene.set_background_colour(Colour::dark_grey);

    scene.add_material(DiffuseMaterial{Colour::light_grey});
    scene.add_material(EmissiveMaterial{Colour::orange, 5.0});
    scene.add_material(EmissiveMaterial{Colour::white, 3.0});
    scene.add_material(DiffuseMaterial{Colour::dark_grey});

    scene.add_geometry(std::make_unique<UnitSphere>());
    scene.add_mesh(io::obj::load_mesh("assets/smooth_monkey.obj"));

    scene.add_instance(1, 0, Transform().set_scale(2).set_rotation({0, 0, math::degrees_to_radians(45)}));  // Monkey
    scene.add_instance(0, 1, Transform().set_position({-10, 10, 0}).set_scale(4));
    scene.add_instance(0, 2, Transform().set_position({15, 15, 0}).set_scale(10));
    scene.add_instance(0, 3, Transform().set_position({0, -104, 0}).set_scale(100));  // Ground plane

    const double aspect_ratio = ((double) image_width) / image_height;
    Camera camera {aspect_ratio, 1, math::degrees_to_radians(60)};
    camera.T.set_position({0, 0, 7});
    scene.add_camera(camera);

    const Tracer tracer{scene};
    const RandomSampler sampler{128};

    tracer.render(sampler, image_height, image_width, 5, math::Interval{math::shadow_epsilon, math::infinity});
}