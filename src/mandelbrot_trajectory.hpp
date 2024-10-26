#ifndef MANDELBROT_TRAJECTORY_HPP
#define MANDELBROT_TRAJECTORY_HPP

#include <vector>

#include "settings.hpp"

using namespace std;

class Trajectory {
    public:
        Trajectory(Settings* settings) :
            trajectory_vector(settings->trajectory_vector), 
            nr_frames(settings->nr_frames), 
            start_height(settings->start_height), 
            start_width(settings->start_width), 
            xy_smoothing_power(settings->xy_smoothing_power) {
                set_r_dim();
                
                set_trajectory_change_at_frame_numbers();

                // I leave this to be set outside the constructor, such that we can change f_xy
                set_interpolation_parameters();
            };

        const int nr_frames;
        const double xy_smoothing_power;
    
    protected:
        // to be accessed by the 'befriended' MandelbrotVideo
        double start_height;
        double start_width;
        
        double r_dim;       
        
        struct xy {
            double x;
            double y;
        };

        struct InterpolationParameters {
            xy start;
            xy end;
            double r_xy; 
            double f_xy; 
            double f_err;
        };

        // struct to handle the return 'vector' but also keep meaningful naming
        struct TrajectoryPoint {
            double x;
            double y;
            double target_width;
            double target_height;
        };

        vector<InterpolationParameters> interpolation_parameters;
        vector<int> trajectory_change_at_frame_numbers;

        double corrected_interpolation(double x0, double x1, double f_xy, double f_err);
        TrajectoryPoint get_trajectory_point(size_t i);

    private:
        const vector<vector<double>> trajectory_vector;

        void set_r_dim();    

        void set_trajectory_change_at_frame_numbers();

        void set_interpolation_parameters();
};

#endif