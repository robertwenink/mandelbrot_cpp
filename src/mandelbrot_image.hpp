#include "mandelbrot.hpp"
#include "timer.hpp"
#include "settings.hpp"


class MandelbrotImage : public Mandelbrot {
    public:
        // directly inherit the constructor from the parent class
        using Mandelbrot::Mandelbrot; 
        
        void run() override {

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

            vector<double> x_cor = linspace(x-width/2.0, x+width/2.0, nx);
            vector<double> y_cor = linspace(y-height/2.0, y+height/2.0, ny);
            timer.timeit("linspace()", t_0);

            // main calculation
            auto t_1 = high_resolution_clock::now();
            mandelbrot(x_cor, y_cor, Settings::max_its);
            timer.timeit("mandelbrot()", t_1);    

            // png, jpg etc only support integer color chanels, so convert
            auto t_3 = high_resolution_clock::now();

            X.convertTo(output_image, CV_8UC3, 255.0);

            // write/show the image
            (void) cv::imwrite("mandelbrot.png", output_image);
            (void) cv::imshow("mandelbrot.png", output_image);

            timer.timeit("cv::imwrite(), imshow()", t_3);
            timer.timeit("main()", t_0);
            timer.logTime();
        };

};