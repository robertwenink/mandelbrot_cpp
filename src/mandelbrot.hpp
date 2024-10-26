#ifndef MANDELBROT_HPP
#define MANDELBROT_HPP

#include <fstream>
#include <iostream>

#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <omp.h>

#include "settings.hpp"

using namespace std;

class Mandelbrot {
    public:
        // fully define the constructor here, assigning attributes directly
        Mandelbrot(Settings* settings) : 
            colormap_name(settings->colormap), 
            nx(settings->x_resolution),
            ny(settings->y_resolution),
            max_its(settings->max_its),
            colormap(loadColormap()) {
                // row-major order, so y then x
                X = cv::Mat(settings->y_resolution, settings->x_resolution, CV_64FC3); 

                // png, jpg etc only support integer color chanels
                output_image = cv::Mat(settings->y_resolution, settings->x_resolution, CV_8UC3); 
            };
        ~Mandelbrot() {};

        /**
         * Polymorphism, abstract class function: 
         * mandelbrot_image and mandelbrot_video are both required to implement their own.
         * virtual: allow polymorphism and pick the instance implementation at runtime
         * = 0: require children to implement the class, making this a partially abstract class; works for any return type
         * To be overwritten as: void run() const override {..};
         */
        virtual void run() = 0;

        // main calculations
        void mandelbrot(const std::vector<double> &x_cor, const std::vector<double> &y_cor, const int max_its);

        // utilities
        vector<double> linspace(double start, double end, int num);
        cv::Vec3d interpolateColor(const cv::Vec3d& color1, const cv::Vec3d& color2, double t);

    protected:
        cv::Mat X;
        cv::Mat output_image;
        const int nx;
        const int ny;
        const int max_its;

    private:
        const string colormap_name; 
        const vector<cv::Vec3d> colormap;
        

        /** 
         * NOTE: we could make a seperate Colormap class, 
         * but applyContinuousColormap is called from within the performance critical mandelbrot function
         * so I would like to keep things simple.
         */
        vector<cv::Vec3d> loadColormap();

        // ApplycontinousColormap in two versions; apply to image matrix or apply to a single point
        void applyContinuousColormap(cv::Mat& img_color);
        cv::Vec3d applyContinuousColormap(double n_frac);
};
    
#endif