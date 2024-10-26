#include "mandelbrot_video.hpp"
#include "settings.hpp"

/*********************
 * Animation utilities
 *********************/

/**
 * Linearly increase the max iteration count based on the zoom. 
 * Higher zoom levels require deeper nr_frames.
 * @param current_frame_nr: is a linear proxy for the zoom
 */
int MandelbrotVideo::get_current_max_its(int current_frame_nr) {

    double current_frame_frac = static_cast<double>(current_frame_nr) / nr_frames;
    int min_its = 100;

    // simple linear interpolation
    // based on: current_max_its = min_its + (MAX_ITS-min_its)*(current_zoom-min_zoom)/(max_zoom-min_zoom)
    // increase the x_scale variable to have a relatively higher rate of increase at the beginning and lower at the end of simulation.
    double x_scale = 10.0;
    double y_scale = log(1 + x_scale);
    double current_max_its = min_its + (max_its - min_its) * log(1 + x_scale * current_frame_frac) / y_scale;

    // Return the floored value as an integer
    return static_cast<int>(floor(current_max_its));
};


/**********************
 * Main class functions
 **********************/


void MandelbrotVideo::run() {
    
    // timing init
    auto start_simulation = chrono::high_resolution_clock::now();
    double render_time = 0;

    // Create a VideoWriter object
    cv::VideoWriter videoWriter;

    if(render) {
        // Parameters for the video
        int codec = cv::VideoWriter::fourcc('m', 'p', '4', 'v');  // Codec for MP4 (using 'MP4V')
        
        // Frame size (width, height)
        cv::Size frameSize(nx, ny);  

        // open the write file    
        videoWriter.open(output_filename + ".mp4", codec, fps, frameSize, true); // true for isColor argument

        // Check if the VideoWriter object was initialized successfully
        if (!videoWriter.isOpened()) {
            cerr << "Could not open the video writer!" << endl;
            return;
        }

        // open a window once for liveplotting
        if (liveplotting) {
            cv::namedWindow("Mandelbrot Liveplot", cv::WINDOW_AUTOSIZE); // Initialize window
        }
    }

    // variable initialisation
    double width = get_trajectory_point(0).target_width;
    double height = get_trajectory_point(0).target_height;
    InterpolationParameters ips = interpolation_parameters[0];
    int j = 1;

    // Call the main animation looper 
    // (merge of frame_helper and frame_builder compared to the Python version)
    for (int i = 0; i < nr_frames; ++i) {  
        auto start_it = chrono::high_resolution_clock::now();
        
        // get new trajectory simulation parameters at a trajectory change (except if it is the end of simulation)!
        if (i == trajectory_change_at_frame_numbers[j] && 
            i != trajectory_change_at_frame_numbers.back()) {
            ips = interpolation_parameters[j];
            j += 1;
            cout << "Change of trajectory!\n";
        };

        double x = corrected_interpolation(ips.start.x, ips.end.x, ips.f_xy, ips.f_err);
        double y = corrected_interpolation(ips.start.y, ips.end.y, ips.f_xy, ips.f_err);
        int current_max_its = get_current_max_its(i);

        // Create a 'corrected' x and y linspace with sizes of the resolution and values within the mandelbrot domain of interest.
        vector<double> x_cor = linspace(x-width/2.0, x+width/2.0, nx);
        vector<double> y_cor = linspace(y+height/2.0, y-height/2.0, ny); // from + to -, y order is other way around compared to matplotlib

        // main mandelbrot calculation
        mandelbrot(x_cor, y_cor, current_max_its);
        X.convertTo(output_image, CV_8UC3, 255.0);

        // adjust the main parameters for the next iteration
        ips.f_xy *= ips.r_xy;
        width *= r_dim;
        height *= r_dim;

        // Write video and liveplot
        if(render) {
            auto start_render = chrono::high_resolution_clock::now();

            // Write the image to the video
            videoWriter.write(output_image);

            if(liveplotting) {
                cv::imshow("Mandelbrot Liveplot", output_image);
                cv::waitKey(1);
            }
            
            auto end_render = chrono::high_resolution_clock::now();
            render_time += chrono::duration_cast<chrono::milliseconds>(end_render - start_render).count();
        }

        // Timings and simulation progress
        auto end_it = chrono::high_resolution_clock::now();
        double elapsed = chrono::duration_cast<chrono::milliseconds>(end_it - start_it).count() / 1000.0;

        printf("%.2f%% complete, iteration took %.2fs", (static_cast<float>(i + 1) / nr_frames) * 100, elapsed);
        cout << endl; // to flush
    }

    if(render) {
        // Release the video writer
        videoWriter.release();
    }

    // End of simulation logging
    auto end_simulation = chrono::high_resolution_clock::now();
    double total_elapsed_seconds = chrono::duration_cast<chrono::milliseconds>(end_simulation - start_simulation).count() / 1000.0;

    log_performance(total_elapsed_seconds, render_time/1000.0);
    printf("Video written successfully! Simulation finished in %.2fs", total_elapsed_seconds);
    cout << endl;
}


void MandelbrotVideo::log_performance(const double total_elapsed_seconds, const double total_render_time, const string& log_filename) {
    // Calculate average time per iteration
    double average_time_per_iteration = total_elapsed_seconds / nr_frames;
    double calc_time = total_elapsed_seconds-total_render_time;
    double average_calc_time_per_iteration = calc_time / nr_frames;

    // Open log file in append mode
    ofstream log_file(log_filename, ios::app);
    if (!log_file) {
        cerr << "Error: Could not open log file." << endl;
        return;
    }

    // Check if the file is empty to add header
    ifstream check_file(log_filename);
    bool is_empty = (check_file.peek() == ifstream::traits_type::eof());
    check_file.close();
    if (is_empty) {
        log_file << "Elapsed time (s), Render time (s), Elapsed-Render time (s), Number of frames, Resolution (nx x ny), Avg time per Iteration (s), Avg calc time per Iteration (s)" << "\n";
    }

    // Write data to log file in CSV format
    log_file << total_elapsed_seconds << ", "
             << total_render_time << ", "
             << calc_time << ", "
             << nr_frames << ", "
             << nx << "x" << ny << ", "
             << average_time_per_iteration << ", "
             << average_calc_time_per_iteration << "\n";

    cout << "Performance data logged to " << log_filename << endl;

    // Close the file
    log_file.close();
}