#include <chrono>

// for opencv install gtk: vcpkg install opencv[gtk]
#include <opencv2/opencv.hpp>

#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>

#include "settings.hpp"
#include "mandelbrot.hpp"
#include "mandelbrot_image.hpp"
#include "mandelbrot_video.hpp"


using namespace std;

void show_openmp(){
    // test if openmp is available/used
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int total_threads = omp_get_num_threads();
        #pragma omp critical
        {
            cout << "Hello from thread " << thread_id << " of " << total_threads << endl;
        }
    }
}


int main() {
    // do we have intel optimisations there, and ffmpeg enabled?
    // std::cout << "Available backends: " << cv::getBuildInformation() << std::endl;
    // openmp enabled?
    // show_openmp();

    Settings settings;
    settings.loadFromYaml("settings.yaml");

    unique_ptr<Mandelbrot> renderer;

    // Instantiate based on settings.animate
    if (settings.animate) {
        renderer = make_unique<MandelbrotVideo>(&settings);
    } else {
        renderer = make_unique<MandelbrotImage>(&settings);
    }
    
    // For video, make sure to have installed ffmpeg!  i.e. with vcpkg install ffmpeg
    renderer->run();

    // show time image
    cv::waitKey(5);  
    cv::destroyAllWindows();

    return 0;
}