#include "mandelbrot_trajectory.hpp"
#include "settings.hpp"

/**
 * Defining the ratios at which each frame consecutively changes in size; defined using the complete simulation range
 */
double Trajectory::get_r_dim() {
    TrajectoryPoint tp_start = get_trajectory_point(0);
    TrajectoryPoint tp_end = get_trajectory_point(-1);

    // nr_frames -1, for the number of intervals
    return pow((tp_end.target_heigth/tp_start.target_heigth),(1/(nr_frames-1)));
};    


/**
 * Calculate at which iterations exactly we will be at the start of a new trajectory and so change the trajectory index.
 * Sets the class attribute trajectory_change_at_frame_number
 */
void Trajectory::set_trajectory_change_at_frame_numbers() {
    // fac_temp = the series of r_dim^i
    double fac_temp = 1;
    
    int j = 0;
    for(int i=0; i<nr_frames; i++) {
        // fac is the inverse zoom here, and with respect to the start of simulation, not per trajectory as done later: 
        // start width or height multiplied with it will retrieve the width or height at zoom level xx
        fac_temp*= r_dim;

        // if we have just crossed the zoomlevel of the current trajectory target, move to the next trajectory target
        if(fac_temp <= 1/(trajectory_vector[j][2])) {
            // indicate at which framenumber we will change the trajectory
            trajectory_change_at_frame_numbers.push_back(i);
            j += 1;
        };
        
        // if we are going to the last trajectory now, just break and add the last manually
        if(j == (trajectory_vector.size()-1)) {
            break; 
        };
    };
    trajectory_change_at_frame_numbers.push_back(nr_frames);
};
       


/**
 * Function to convert trajectory point from [x,y,zoom] -> [x,y,width,height],    
 * width and height based on the initial screen size and the zoom.
 */
Trajectory::TrajectoryPoint Trajectory::get_trajectory_point(int i) {
    TrajectoryPoint trajectory_point;
    vector<double> xyzoom; 

    // -1, are we doing Python?!
    if (i == -1){
        xyzoom = trajectory_vector[trajectory_vector.size() - 1];
    } else {
        xyzoom = trajectory_vector[i];
    }
    
    trajectory_point.x = xyzoom[0];
    trajectory_point.y = xyzoom[1];
    trajectory_point.target_width = Settings::start_width / xyzoom[2];
    trajectory_point.target_heigth = Settings::start_height / xyzoom[2];
    
    return trajectory_point;
};


/**
 * Set the parameters used by the 'corrected interpolation' function.
 * End and start of each trajectory (x0 and x1 for the interpolation) per x and y 
 * are captured by using the corresponding TrajectoryPoint.
 * 
 * Main parameters for this corrected interpolation as set here are:
 * - r_xy: the ratio used to converge the xy location from the one frame to the next
 * - f_xy: uncorrected interpolation factor between the start and end of a trajectory (for the x and y location)
 * - f_err:  constant, equal to 1-r_xy^N, or
 *           the error wrt to interpolation factor 0 after a series of N iterations
 */
void Trajectory::set_interpolation_parameters() {
    // XY_SMOOTHING_POWER: let the location converge faster than the screensize (which is calculated with just r_dim); this looks smoother.
    // i.e. r_xy < r_dim so we converge faster to 0
    double r_xy = std::pow(r_dim, xy_smoothing_power);

    for (size_t i = 0; i < trajectory_change_at_frame_numbers.size() - 1; i++) {
        InterpolationParameters intpars;
        intpars.r_xy = r_xy;

        // set start and endpoints of the current trajectory
        TrajectoryPoint tp_i = get_trajectory_point(i);
        TrajectoryPoint tp_ip1 = get_trajectory_point(i + 1);
        intpars.start.x = tp_i.x;
        intpars.start.y = tp_i.y;
        intpars.end.x = tp_ip1.x;
        intpars.end.y = tp_ip1.y;

        // trajectory_change_at_frame_numbers: frame number at which we switch to the next trajecory target x, y and zoom
        int traj_nr_frames = trajectory_change_at_frame_numbers[i + 1] - trajectory_change_at_frame_numbers[i];

        // Below: with the smoothing_power applied (the effect of r_xy is stronger than globally needed), 
        // we need something that compensates for that in the interpolation, 
        // such that the endpoint in x and y correspond again compared to using the 'normal' r_dim at a trajectory switch.
        // Normally, we interpolate between x0 and x1 *of a trajectory* using an inverse interpolation factor fac in [1,0].
        // Remember that we defined traj_nr_frames using r_dim, so 1 and 1*r_dim^traj_nr_frames should exactly match this [1,0]
        // since r_xy is based on r_dim, the same holds for r_xy 
        // (Note that even without the smoothing_power we would require a correction!).

        // Lets represent the mismatch at the end of the (sub)series using 'f_err', this would mean we have f_uncorrected in [1, f_err].
        // where f_err = 1 * r_xy^N - 0 = r_xy^N. 
        // At the interpolation, we will normalize f_xy to the [0,1] range and use: 
        // f_corr = f_err - f_xy / (f_err - 1)

        double rN = std::pow(r_xy, traj_nr_frames);
        intpars.f_err = rN / (1 - rN);
        
        // reset the base interpolation factor for each new trajectory.
        intpars.f_xy = 1;

        // push the parameters per trajectory
        interpolation_parameters.push_back(intpars);
    }
};


/**
 * Interpolation function that takes correction factor f into account, 
 * correcting for the difference between self.r_dim and the smoothed self.r_dim**SMOOTHING_POWER
 * 
 * x0, x1: start and end values between which the interpolation takes place. 
 * f_xy: starts at 1, gets multiplied by r_xy after each iteration.
 * f_err:  constant, equal to 1-r_xy^N, or
 *         the error wrt to interpolation factor 0 after a series of N iterations
 */
double Trajectory::corrected_interpolation(double x0, double x1, double f_xy, double f_err) {
    double f_corr = (f_err - f_xy) / (f_err - 1);
    return x0 * f_corr + x1 * (1 - f_corr);
};