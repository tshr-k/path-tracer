cmake_minimum_required(VERSION 3.15)
project(PathTracer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.13.0
)
FetchContent_MakeAvailable(Catch2)

# Splitting the codebase into .h/.cpp pairs means the small, hot math types (Vector, Point,
# Matrix, Colour, Ray, ONB, ...) are no longer inlined across translation units by default.
# IPO/LTO restores that cross-TU inlining at link time for Release builds, so incremental
# Debug builds stay fast while Release keeps the original header-only performance.
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

set(PATHTRACER_SOURCES
  src/main.cpp
  src/math/math_utils.cpp
  src/math/vector3.cpp
  src/math/point.cpp
  src/math/matrix4.cpp
  src/math/onb.cpp
  src/math/ray.cpp
  src/math/transform.cpp
  src/math/colour.cpp
  src/geometry/aabb.cpp
  src/geometry/mesh.cpp
  src/geometry/unit_sphere.cpp
  src/geometry/instance.cpp
  src/accel/blas.cpp
  src/accel/tlas.cpp
  src/shading/bsdf.cpp
  src/shading/material.cpp
  src/render/camera.cpp
  src/render/frame_buffer.cpp
  src/render/sampler.cpp
  src/render/scene.cpp
  src/render/tracer.cpp
  src/io/image_io.cpp
  src/io/mesh_io.cpp
)

add_executable(PathTracer ${PATHTRACER_SOURCES})
target_include_directories(PathTracer PRIVATE src)
if(ipo_supported)
  set_property(TARGET PathTracer PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

add_executable(unit_tests tests/main.cpp)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(unit_tests PRIVATE src)
target_sources(unit_tests PRIVATE
  # Library sources under test (everything except PathTracer's own main.cpp)
  src/math/math_utils.cpp
  src/math/vector3.cpp
  src/math/point.cpp
  src/math/matrix4.cpp
  src/math/onb.cpp
  src/math/ray.cpp
  src/math/transform.cpp
  src/math/colour.cpp
  src/geometry/aabb.cpp
  src/geometry/mesh.cpp
  src/geometry/unit_sphere.cpp
  src/geometry/instance.cpp
  src/accel/blas.cpp
  src/accel/tlas.cpp
  src/shading/bsdf.cpp
  src/shading/material.cpp
  src/render/camera.cpp
  src/render/frame_buffer.cpp
  src/render/sampler.cpp
  src/render/scene.cpp
  src/render/tracer.cpp
  src/io/image_io.cpp
  src/io/mesh_io.cpp

  tests/accel/test_bvh.cpp
  tests/geometry/test_aabb.cpp
  tests/geometry/test_mesh.cpp
  tests/geometry/test_unit_sphere.cpp
  tests/math/test_math.cpp
  tests/math/test_random.cpp
  tests/math/test_transform.cpp
  tests/render/test_camera.cpp
  tests/render/test_frame_buffer.cpp
  tests/render/test_sampler.cpp
  tests/shading/test_bsdf.cpp
)
if(ipo_supported)
  set_property(TARGET unit_tests PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()
