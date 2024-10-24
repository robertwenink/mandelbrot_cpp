#ifndef MANDELBROT_VIDEO_HPP
#define MANDELBROT_VIDEO_HPP

#include <opencv2/opencv.hpp>
#include <cmath>
#include <format>

#include "mandelbrot.hpp"
#include "mandelbrot_trajectory.hpp"

class MandelbrotVideo : public Mandelbrot, Trajectory {
    public:
        MandelbrotVideo(const string colormap_name, const int x_resolution, const int y_resolution, const vector<vector<double>> trajectory_vector, const int nr_frames, const bool render, const bool liveplotting) : 
            Mandelbrot(colormap_name, x_resolution, y_resolution),
            Trajectory(trajectory_vector, nr_frames),
            nr_frames(nr_frames), render(render), liveplotting(liveplotting) {};

        void run() override;

    private:
        const int nr_frames;
        const bool render;  
        const bool liveplotting;
        int get_current_max_its(int current_frame_nr);
};

#endif