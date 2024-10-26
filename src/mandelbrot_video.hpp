#ifndef MANDELBROT_VIDEO_HPP
#define MANDELBROT_VIDEO_HPP

#include <opencv2/opencv.hpp>
#include <cmath>

#include <cstdio>
#include <iostream>
#include <fstream>

#include "settings.hpp"
#include "mandelbrot.hpp"
#include "mandelbrot_trajectory.hpp"

class MandelbrotVideo : public Mandelbrot, Trajectory {
    public:
        MandelbrotVideo(Settings* settings) : 
            Mandelbrot(settings),
            Trajectory(settings), 
            fps(settings->fps), 
            output_filename(settings->output_filename), 
            render(settings->render), 
            liveplotting(settings->liveplotting) {};

        void run() override;

    private:
        // const int nr_frames is set by the Trajectory baseclass
        const int fps;  
        const string output_filename;  
        const bool render;  
        const bool liveplotting;
        int get_current_max_its(int current_frame_nr);
        void log_performance(const double total_elapsed_seconds, const double total_render_time, const string& log_filename = "performance_log.csv"); // default arguments are defined in the header, do not use in the cpp file!
};

#endif