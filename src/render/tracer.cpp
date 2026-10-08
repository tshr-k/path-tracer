#include "tracer.h"

#include <algorithm>
#include <chrono>
#include <format>
#include <iostream>
#include <mutex>
#include <thread>
#include <variant>
#include <vector>

#include "scene.h"
#include "sampler.h"
#include "io/image_io.h"
#include "math/colour.h"
#include "math/onb.h"
#include "math/ray.h"
#include "math/vector3.h"
#include "shading/bsdf.h"
#include "render/frame_buffer.h"

Colour Tracer::ray_colour(const Sampler& sampler, const Ray& ray, int bounce_depth, const math::Interval t_range, Colour& throughput) const {
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

Tracer::Tracer(const Scene& scene) : scene_{scene} {}

void Tracer::render_pixel(const Sampler& sampler, int row, int col, int image_width, int image_height, int max_bounce_depth, const math::Interval &t_range, FrameBuffer& frame_buffer) const {
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
    frame_buffer(row, col) = Colour::clamp(c);
}

void Tracer::render_tile(const Tile& tile, const Sampler& sampler, int image_width, int image_height, int max_bounce_depth, const math::Interval &t_range, FrameBuffer& frame_buffer) const {
    for (int row = tile.row_0; row < tile.row_1; row++) {
        for (int col = tile.col_0; col < tile.col_1; col++) {
            render_pixel(sampler, row, col, image_width, image_height, max_bounce_depth, t_range, frame_buffer);
        }
    }
}

void Tracer::render(const Sampler& sampler, int image_width, int image_height, int tile_size, int num_threads, int max_bounce_depth, const math::Interval t_range, FrameBuffer& frame_buffer) const {
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

    // Add tiles to a "queue"
    std::vector<Tile> tiles;
    for (int row = 0; row < image_height; row += tile_size) {
        for (int col = 0; col < image_width; col += tile_size) {
            tiles.emplace_back(
                row,
                col,
                std::min(row + tile_size, image_height),
                std::min(col + tile_size, image_width)
            );
        }
    }

    const int num_tiles = tiles.size();
    std::atomic<std::size_t> next_tile = 0;
    std::mutex clog_mutex;

    // A worker consumes and completes render_tile jobs until all are exhuasted
    auto worker = [&]() {
        while (true) {
            std::size_t i = next_tile.fetch_add(1);
            if (i >= tiles.size()) {
                break;
            }

            render_tile(tiles[i], sampler, image_width, image_height, max_bounce_depth, t_range, frame_buffer);

            std::lock_guard<std::mutex> lock {clog_mutex};
            std::clog << std::format(
                "Tile {} / {} finished.\n",
                i + 1,
                num_tiles
            );
        }
    };

    // Launch as many threads as are supported or requested
    std::vector<std::thread> threads;
    num_threads = std::min(static_cast<unsigned>(num_threads), std::thread::hardware_concurrency());
    std::clog << "Using " << num_threads << " threads.\n";
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(worker);
    }

    // Wait for all the threads to finish
    for (std::thread& t : threads) {
        t.join();
    }

    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end_time - start_time;
    std::clog << "\nRendering finished in " << elapsed_seconds.count() << " seconds.\n";
}
