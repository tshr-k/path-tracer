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

add_executable(PathTracer src/main.cpp)

add_executable(unit_tests tests/main.cpp)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
target_sources(unit_tests PRIVATE
  tests/test_aabb.cpp
  tests/test_bsdf.cpp
  tests/test_camera.cpp
  tests/test_math.cpp
  tests/test_random.cpp
  tests/test_sampler.cpp
  tests/test_unit_sphere.cpp
  tests/test_transform.cpp
)