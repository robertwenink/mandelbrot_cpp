#ifndef MANDELBROT_IMAGE_HPP
#define MANDELBROT_IMAGE_HPP

#include "settings.hpp"
#include "mandelbrot.hpp"
#include "timer.hpp"

using namespace std;

class MandelbrotImage : public Mandelbrot {
    public:
        MandelbrotImage(Settings* settings) : 
            Mandelbrot(settings),
            x(settings->trajectory_vector.at(0)[0]),
            y(settings->trajectory_vector.at(0)[1]),
            width(settings->start_width),
            height(settings->start_height) 
            {};

        void run() override {

            // setup timing
            using chrono::high_resolution_clock;
            auto t_0 = high_resolution_clock::now();
            timer::Timer timer;
            spdlog::info("Begin mandelbrot set image generation");

            // Covert pixels to mandelbrot set coords
            vector<double> x_cor = linspace(x-width/2.0, x+width/2.0, nx);
            vector<double> y_cor = linspace(y+height/2.0, y-height/2.0, ny); // from + to -, y order is other way around compared to matplotlib
            timer.timeit("linspace()", t_0);

            // main calculation
            auto t_1 = high_resolution_clock::now();
            mandelbrot(x_cor, y_cor, max_its);
            timer.timeit("mandelbrot()", t_1);    

            // png, jpg etc only support integer color chanels, so convert
            auto t_3 = high_resolution_clock::now();

            X.convertTo(output_image, CV_8UC3, 255.0);

            // write/show the image
            cv::imwrite("mandelbrot.png", output_image);
            cv::imshow("mandelbrot.png", output_image);

            timer.timeit("cv::imwrite(), imshow()", t_3);
            timer.timeit("main()", t_0);
            timer.logTime();
        };

    private:
        double x;
        double y;
        double width;
        double height;
};

#endif