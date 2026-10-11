#include <iostream>
#include <memory>

#include "math/colour.h"
#include "render/camera.h"
#include "io/image_io.h"
#include "geometry/mesh.h"
#include "io/mesh_io.h"
#include "shading/material.h"
#include "geometry/unit_sphere.h"
#include "render/scene.h"
#include "render/sampler.h"
#include "render/tracer.h"
#include "math/math_utils.h"
#include "math/transform.h"
#include "render/frame_buffer.h"

int main() {
    // Create instances and materials
    Scene scene;
    scene.set_background_colour(Colour::black);

    auto orange_diffuse = scene.add_material(DiffuseMaterial{Colour::orange});
    auto orange_emissive = scene.add_material(EmissiveMaterial{Colour::orange, 15.0});
    auto white_emissive = scene.add_material(EmissiveMaterial{Colour::white, 7.0});
    auto purple_diffuse = scene.add_material(DiffuseMaterial{Colour::purple});
    auto cyan_plastic = scene.add_material(PlasticMaterial{Colour::cyan, 0.3});
    auto light_grey_metal = scene.add_material(MetalMaterial{Colour::light_grey, 0.3});

    auto unit_sphere = scene.add_geometry(std::make_unique<UnitSphere>());
    auto smooth_monkey = scene.add_mesh(io::obj::load_mesh("assets/smooth_monkey.obj"));

    scene.add_instance(smooth_monkey, orange_diffuse, Transform().set_scale(2).set_rotation({0, 0, math::degrees_to_radians(45)}));  // Monkey
    scene.add_instance(unit_sphere, orange_emissive, Transform().set_position({-10, 10, 0}).set_scale(4));
    scene.add_instance(unit_sphere, white_emissive, Transform().set_position({15, 15, 0}).set_scale(10));
    scene.add_instance(unit_sphere, light_grey_metal, Transform().set_position({2, 2, 2}).set_scale(0.5));
    scene.add_instance(unit_sphere, cyan_plastic, Transform().set_position({-1.5, -1.5, 2}).set_scale(0.8));
    scene.add_instance(unit_sphere, purple_diffuse, Transform().set_position({0, -104, 0}).set_scale(100));  // Ground plane

    // Set image and camera properties, and render
    const int image_height = 256;
    const int image_width = 256;

    const int tile_size = 32;
    const int num_threads = 12;
    const int max_bounce_depth = 8;
    const int samples_per_pixel = 512;

    const double aspect_ratio = ((double) image_width) / image_height;
    Camera camera {aspect_ratio, 1, math::degrees_to_radians(60)};
    camera.T.set_position({0, 0, 7});
    scene.add_camera(camera);

    const Tracer tracer{scene};
    const RandomSampler sampler{samples_per_pixel};
    FrameBuffer frame_buffer {image_width, image_height};  // Filled by tracer.render()
    tracer.render(
        sampler,
        image_width,
        image_height,
        tile_size,
        num_threads,
        max_bounce_depth,
        math::Interval{math::shadow_epsilon, math::infinity},
        frame_buffer
    );

    io::ppm::write_image(std::cout, frame_buffer);
}
