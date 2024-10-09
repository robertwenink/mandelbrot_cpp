cmake_minimum_required(VERSION 3.16)
project(math_cpp)

set(CMAKE_CXX_STANDARD 17)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /openmp")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif ()

# general-purpose libs
find_package(spdlog REQUIRED)

# ------------------------- OpenCV image generation build --------------------
# find OpenCV
find_package(OpenCV REQUIRED)

add_executable(
        mandelbrot_zoom
        mandelbrot.cpp
        src/mandelbrot.hpp
)
# link OpenCV
target_link_libraries(mandelbrot_zoom ${OpenCV_LIBS} spdlog::spdlog)