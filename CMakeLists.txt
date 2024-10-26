cmake_minimum_required(VERSION 3.16)
project(Mandelbrot_cpp)

set(CMAKE_CXX_STANDARD 17)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /openmp /JMC") # JMC = Just My Code
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif ()

# Load libs
find_package(spdlog REQUIRED)
find_package(OpenCV REQUIRED)
# find_package(FFMPEG REQUIRED) (I installed opencv with opencv[ffmpeg] !)
find_package(yaml-cpp REQUIRED)

# Add the src and colormaps directory to the include path
include_directories(src colormaps)

add_executable(
        mandelbrot_render
        mandelbrot_main.cpp
        src/mandelbrot.cpp
        src/mandelbrot_video.cpp
        src/mandelbrot_trajectory.cpp
)

# link OpenCV, ffmpeg (for videowriter), gtk (for opencv gui)
target_link_libraries(mandelbrot_render PRIVATE ${FFMPEG_LIBRARIES} ${OpenCV_LIBS} spdlog::spdlog yaml-cpp::yaml-cpp)
target_include_directories(mandelbrot_render PRIVATE ${FFMPEG_INCLUDE_DIRS})