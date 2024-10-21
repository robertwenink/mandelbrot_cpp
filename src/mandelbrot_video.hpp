#ifndef MANDELBROT_VIDEO_HPP
#define MANDELBROT_VIDEO_HPP

#include <opencv2/opencv.hpp>

#include "mandelbrot.hpp"

class MandelbrotVideo : public Mandelbrot {
    public:
        MandelbrotVideo(const string colormap_name, const int x_resolution, const int y_resolution, const int nr_frames, const bool render, const bool liveplotting) : 
            Mandelbrot(colormap_name, x_resolution, y_resolution),
            nr_frames(nr_frames), render(render), liveplotting(liveplotting) {
                set_r_dim();
                set_trajectory_change_at_frame_number_list();
            }

        void run() override;

    private:
        const int nr_frames;
        const bool render; 
        const bool liveplotting;

        void set_r_dim();
        void set_trajectory_change_at_frame_number_list();
};


#endif