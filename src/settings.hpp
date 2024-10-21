#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>
#include <vector>
#include <cmath>

using namespace std;

namespace Settings {
    //////////////////////////////////////////////////////////////////////////////
    ////////////////////////// Settings ////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

    int x_resolution = 1920;
    int y_resolution = 1080;
    int nr_frames = 400;
    int max_its = 2000;

    // CPU or GPU
    bool GPU = true;

    bool ANIMATE = true; // Make a zoom animation?
    bool RENDER = true; // Render the images, or just do the calculations?

    // options only used if animate and render are true
    bool liveplotting = false; // We can animate and render, but not draw, saves time.
    bool output_filename = "mandelbrot";

    ///////////////////////////////////////////////////////
    ////////////// Trajectory definition //////////////////
    ///////////////////////////////////////////////////////
    // NOTE Define a trajectory of multiple points, 
    // e.g. to zoom with a centre focus on some structure 
    // and then continue zooming on something near

    // On a trajectory change, factor to shift 
    // the location quicker than the zoom for a smoother simulation.
    // Take a minimum of ~1.2
    float smnoothing_power = 1.25;

    // Standard start sizes to capture the mandelbrot
    double start_height = 3.0;
    double start_width = 3.0 * (static_cast<double>(x_resolution)/y_resolution);

    // NOTE 4096**4 / 10 is about the deepest we can go with float64 precision
    vector<vector<double>> trajectory = {{-0.5,0,1},        // formulated as [x,y,zoom], this one is always required
                                         {0.3602404434377, -0.6413130610647635, pow(4096, 4) / 25.0}}; 
}

#endif