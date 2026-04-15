#pragma once

#include <chrono>
#include <iostream>
#include <variant>

#include "scene.h"
#include "sampler.h"
#include "image_io.h"
#include "vector3.h"
#include "colour.h"
#include "ray.h"
#include "onb.h"
#include "math_utils.h"

class Tracer {
private:
    const Scene& scene_;

    Colour ray_colour(const Sampler& sampler, const Ray& ray, int bounce_depth, const math::Interval t_range, Colour& throughput) const {
        if (bounce_depth <= 0) {  // No more bounces allowed
            return Colour{0, 0, 0};
        }
        
        HitRecord hit_record;
        if (scene_.hit(ray, hit_record, t_range.min)) {
            const MaterialVariant& material = scene_.materials()[hit_record.material_index];
            const Colour emitted = std::visit([](const auto& m) { return m.emission(); }, material);
            const auto bsdf = std::visit([](const auto& m) { return m.create_bsdf(); }, material);
            if (!bsdf) {  // No scattering, purely emissive
                return emitted;
            }

            const ONB onb{hit_record.normal};
            const UnitVector w_o_local = unit_vector(onb.global_to_local(-ray.direction()));
            
            double p;            
            const UnitVector w_i_local = bsdf->sample(w_o_local, p);
            if (p <= 1e-6) {
                return Colour{0, 0, 0};
            }

            const Ray scattered_ray{hit_record.point, onb.local_to_global(w_i_local)};
            const Colour f = bsdf->evaluate(w_i_local, w_o_local);
            const double cos_theta = std::max(0.0, w_i_local.z());
            
            throughput *= f * cos_theta / p;
            double survival_p = std::min(0.95, throughput.max_component());
            if (sampler.sample_1d() > survival_p) { 
                return emitted;  // Russian roulette termination
            }
            throughput /= survival_p;  // Compensate for terminated rays

            const Colour incoming_radiance = ray_colour(sampler, scattered_ray, bounce_depth - 1, t_range, throughput);

            return emitted + (throughput * incoming_radiance);
        }

        return scene_.background_colour();  // Sky colour
    }

public:
    constexpr Tracer(const Scene& scene) : scene_{scene} {}

    void render(const Sampler& sampler, int image_height, int image_width, int max_bounce_depth, const math::Interval t_range) const {       
        std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
        
        for (int row = 0; row < image_height; row++) {
            std::clog << "Rendering row " << (row + 1) << " / " << image_height << '\r';
            for (int col = 0; col < image_width; col++) {
                Colour c{0, 0, 0};
                for (int s = 0; s < sampler.samples_per_pixel(); s++) {
                    sampler.start_pixel(row, col);
                    
                    const auto [offset_u, offset_v] = sampler.sample_2d();
                    const double u = (col + offset_u) / image_width;
                    const double v = (row + offset_v) / image_height;

                    const Ray ray = scene_.active_camera().cast_ray(u, v);

                    Colour throughput{1, 1, 1};
                    c += ray_colour(sampler, ray, max_bounce_depth, t_range, throughput);

                    sampler.next_sample();
                }
                c /= sampler.samples_per_pixel();
                io::ppm::write_pixel(std::cout, Colour::clamp(c));
            }
        }

        std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end_time - start_time;
        std::clog << "\nRendering finished in " << elapsed_seconds.count() << " seconds.\n";
    }
};