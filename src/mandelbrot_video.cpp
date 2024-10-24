#include "mandelbrot_video.hpp"
#include "settings.hpp"

/*********************
 * Animation utilities
 *********************/

/**
 * Linearly increase the max iteration count based on the zoom. 
 * Higher zoom levels require deeper iterations.
 * @param current_frame_nr: is a linear proxy for the zoom
 */
int MandelbrotVideo::get_current_max_its(int current_frame_nr) {

    double current_frame_frac = static_cast<double>(current_frame_nr) / nr_frames;
    int min_its = 100;

    // simple linear interpolation
    // based on: current_max_its = min_its + (MAX_ITS-min_its)*(current_zoom-min_zoom)/(max_zoom-min_zoom)
    // increase the x_scale variable to have a relatively higher rate of increase at the beginning and lower at the end of simulation.
    double x_scale = 10.0;
    double y_scale = std::log(1 + x_scale);
    double current_max_its = min_its + (Settings::max_its - min_its) * std::log(1 + x_scale * current_frame_frac) / y_scale;

    // Return the floored value as an integer
    return static_cast<int>(std::floor(current_max_its));
};


/**********************
 * Main class functions
 **********************/


void MandelbrotVideo::run() {

    // Parameters for the video
    int codec = cv::VideoWriter::fourcc('M', 'P', '4', 'V');  // Codec for MP4 (using 'MP4V')
    
    // Frame size (width, height)
    cv::Size frameSize(nx, ny);  

    // Create a VideoWriter object
    cv::VideoWriter videoWriter(Settings::output_filename + ".mp4", codec, Settings::fps, frameSize, true); // true for isColor argument

    // Check if the VideoWriter object was initialized successfully
    if (!videoWriter.isOpened()) {
        std::cerr << "Could not open the video writer!" << std::endl;
        return;
    }

    // variable initialisation
    double width = get_trajectory_point(0).target_width;
    double height = get_trajectory_point(0).target_heigth;
    InterpolationParameters ips;
    int j = 0;

    // Call the main animation looper 
    // (merge of frame_helper and frame_builder compared to the Python version)
    for (int i = 0; i < nr_frames; ++i) {  
        
        // get new trajectory simulation parameters at a trajectory change!
        if (i == trajectory_change_at_frame_numbers[j] && 
            i != trajectory_change_at_frame_numbers[-1]) {
            ips = interpolation_parameters[j];
            j += 1;
            cout << "Change of trajectory!\n";
        };

        double x = corrected_interpolation(ips.start.x, ips.end.x, ips.f_xy, ips.f_err);
        double y = corrected_interpolation(ips.start.y, ips.end.y, ips.f_xy, ips.f_err);
        int current_max_its = get_current_max_its(i);

        // Create a 'corrected' x and y linspace with sizes of the resolution and values within the mandelbrot domain of interest.
        vector<double> x_cor = linspace(x-width/2.0, x+width/2.0, nx);
        vector<double> y_cor = linspace(y-height/2.0, y+height/2.0, ny);

        // main mandelbrot calculation
        X = mandelbrot(x_cor, y_cor, current_max_its);
        X.convertTo(output_image, CV_8UC3, 255.0);

        // Write the image to the video
        videoWriter.write(output_image);

        // adjust the main parameters for the next iteration
        ips.f_xy *= ips.r_xy;
        width *= r_dim;
        height *= r_dim;

        // Print animation progress
        cout << format("{:.2f}", (static_cast<double>(i + 1) / nr_frames) * 100) << "%% complete" << std::endl;
    }

    // Release the video writer
    videoWriter.release();

    std::cout << "Video written successfully!" << std::endl;
}