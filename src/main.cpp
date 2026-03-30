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
    const int image_width = 1024;
    io::ppm::write_header(std::cout, image_height, image_width);

    Scene scene;
    scene.set_background_colour(Colour::light_grey);

    scene.add_material(std::make_unique<DiffuseMaterial>(Colour::light_grey));
    scene.add_geometry(std::make_unique<UnitSphere>());
    scene.add_instance(scene.geometries().back().get(), scene.materials().back().get(), Transform());
    scene.world().back()->T.set_position({0, -101, 0}).set_scale(100);  // Ground "plane"
    
    scene.add_material(std::make_unique<EmissiveMaterial>(Colour::orange, 10));
    scene.add_geometry(std::make_unique<UnitSphere>());
    scene.add_instance(scene.geometries().back().get(), scene.materials().back().get(), Transform());
    scene.world().back()->T.set_position({-1, 3, 0}).set_scale(0.4);  // Light

    scene.add_material(std::make_unique<PlasticMaterial>(Colour::blue, 0.2));
    scene.add_geometry(std::make_unique<UnitSphere>());
    scene.add_instance(scene.geometries().back().get(), scene.materials().back().get(), Transform());

    scene.add_material(std::make_unique<DiffuseMaterial>(Colour::red));
    scene.add_geometry(std::make_unique<UnitSphere>());
    scene.add_instance(scene.geometries().back().get(), scene.materials().back().get(), Transform());
    scene.world().back()->T.set_position({-2, 0, 0});

    scene.add_material(std::make_unique<MetalMaterial>(Colour::light_grey, 0.1));
    scene.add_geometry(std::make_unique<UnitSphere>());
    scene.add_instance(scene.geometries().back().get(), scene.materials().back().get(), Transform());
    scene.world().back()->T.set_position({2, 0, 0});

    const double aspect_ratio = ((double) image_width) / image_height;
    Camera camera {aspect_ratio, 1, math::degrees_to_radians(60)};
    camera.T.set_position({0, 0, 7});
    scene.add_camera(std::make_unique<Camera>(camera));

    const Tracer tracer{scene};
    const RandomSampler sampler{64};
    tracer.render(sampler, image_height, image_width, 16, math::Interval{0.001, math::infinity});
}