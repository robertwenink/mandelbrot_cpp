#include <chrono>
#include <opencv2/opencv.hpp>

#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>

#include "src/timer.hpp"
#include "src/mandelbrot.hpp"
#include "src/settings.cpp"


using namespace std;


// Function to load the colormap from a CSV file
vector<cv::Vec3d> loadColormap(const string& filename, int colormap_size) {
    ifstream file("colormaps/" + filename);
    vector<cv::Vec3d> colormap;

    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return colormap;
    }

    // parse the file
    float r, g, b;
    for (int i = 0; i < colormap_size; ++i) {
        file >> r;
        file.ignore(1); // Skip the comma
        file >> g;
        file.ignore(1); // Skip the comma
        file >> b;
        colormap.emplace_back(b, g, r);  // OpenCV uses BGR order
    }

    return colormap;
}


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
    vector<cv::Vec3d> colormap = loadColormap("twilight_colormap.csv", 1024);

    // preallocate output vector memory.
    cv::Mat X(Settings::y_resolution, Settings::x_resolution, CV_64FC3); // row-major order

    // setup timing
    using chrono::high_resolution_clock;
    auto t_0 = high_resolution_clock::now();
    timer::Timer timer;
    spdlog::info("Begin mandelbrot set image generation");

    // Covert pixels to mandelbrot set coords
    auto x = Settings::trajectory.at(0)[0];
    auto y = Settings::trajectory.at(0)[1];
    auto width = Settings::start_width;
    auto height = Settings::start_height;
    vector<double> x_cor = mandelbrot::linspace(x-width/2.0, x+width/2.0, Settings::x_resolution);
    vector<double> y_cor = mandelbrot::linspace(y-height/2.0, y+height/2.0, Settings::y_resolution);
    timer.timeit("linspace()", t_0);

    // main calculation
    auto t_1 = high_resolution_clock::now();
    mandelbrot::mandelbrot(X, x_cor, y_cor, Settings::max_its, colormap);
    timer.timeit("mandelbrot()", t_1);    


    // png, jpg etc only support integer color chanels, so convert
    auto t_3 = high_resolution_clock::now();
    cv::Mat img_output(X.size(), CV_8UC3);
    X.convertTo(img_output, CV_8UC3, 255.0);

    // write/show the image
    (void) cv::imwrite("mandelbrot.png", img_output);
    (void) cv::imshow("mandelbrot.png", img_output);

    timer.timeit("cv::imwrite(), imshow()", t_3);
    timer.timeit("main()", t_0);
    timer.logTime();

    // show time image
    cv::waitKey(5000);  
    cv::destroyAllWindows();

    return 0;
}