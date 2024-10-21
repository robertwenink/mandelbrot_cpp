#include <chrono>
#include <opencv2/opencv.hpp>

#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>

#include "mandelbrot_image.hpp"
// #include "mandelbrot_video.hpp"
#include "settings.hpp"


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
    // do we have intel optimisations there?
    cv::getBuildInformation();
    show_openmp();

    // Load the Twilight colormap sampled in Python
    MandelbrotImage renderer("twilight", Settings::x_resolution, Settings::y_resolution);
    renderer.run();


    // show time image
    cv::waitKey(5);  
    cv::destroyAllWindows();

    return 0;
}