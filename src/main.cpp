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
    const int image_height = 256;
    const int image_width = 512;
    io::ppm::write_header(std::cout, image_height, image_width);

    Scene scene;
    scene.set_background_colour(Colour::light_grey);

    scene.add_material(DiffuseMaterial{Colour::light_grey});
    scene.add_geometry(UnitSphere());
    Transform T1;
    T1.set_position({0, -101, 0}).set_scale(100);
    scene.add_instance(0, 0, T1);  // Ground "plane"
    
    scene.add_material(EmissiveMaterial{Colour::orange, 10});
    scene.add_geometry(UnitSphere());
    Transform T2;
    T2.set_position({-1, 3, 0}).set_scale(0.4);
    scene.add_instance(1, 1, T2);  // Light

    scene.add_material(PlasticMaterial{Colour::blue, 0.2});
    scene.add_geometry(UnitSphere());
    Transform T3;
    T3.set_position({1, 0, 0}).set_scale(0.5);
    scene.add_instance(2, 2, T3);

    scene.add_material(DiffuseMaterial{Colour::red});
    scene.add_geometry(UnitSphere());
    Transform T4;
    T4.set_position({-2, 0, 0});
    scene.add_instance(3, 3, T4);

    scene.add_material(MetalMaterial{Colour::light_grey, 0.1});
    scene.add_geometry(UnitSphere());
    Transform T5;
    T5.set_position({2, 0, 0});
    scene.add_instance(4, 4, T5);

    const double aspect_ratio = ((double) image_width) / image_height;
    Camera camera {aspect_ratio, 1, math::degrees_to_radians(60)};
    camera.T.set_position({0, 0, 7});
    scene.add_camera(camera);

    const Tracer tracer{scene};
    const RandomSampler sampler{64};

    tracer.render(sampler, image_height, image_width, 16, math::Interval{0.001, math::infinity});
}