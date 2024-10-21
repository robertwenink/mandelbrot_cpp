#include "mandelbrot.hpp"
    

/**
 * Escape time algorithm; optimised variant.
 * See https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set
 * 
 * Here x_corr are the real values, y_corr the imaginary in terms of the mandelbrot fractal
 */
void Mandelbrot::mandelbrot(const std::vector<double> &x_cor, const std::vector<double> &y_cor, const int max_its) {
    const double bailout = 4.0;
    const double log2_inv = 1.0 / log(2.0); // Precompute / log(2)

    // Parallelize the outer loops with OpenMP
    #pragma omp parallel for
    for (int idx = 0; idx < nx * ny; ++idx) {
        // Because the MSVC compiler does not support OpenMP 3 and collapse(2) yet we rewrite to manual indexing. Slightly faster.
        //  for (int i = 0; i < nx; i++) {
        //      for (int j = 0; j < ny; j++) {

        int i = idx / ny;  // Flattened index for i
        int j = idx % ny;  // Flattened index for j

        double x = 0.0, y = 0.0;
        double x2 = 0.0, y2 = 0.0;
        int n = 0;

        while (x2 + y2 <= bailout && n < max_its) {
            y = 2.0 * x * y + y_cor[j];
            x = x2 - y2 + x_cor[i];
            x2 = x * x;
            y2 = y * y;
            n++;
        }

        if (n < max_its) {
            double log_zn = log(x2 + y2) / 2; // in ln |z| because |z| of a complex number is just sqrt(x^2 + y^2) without its cross components
            double nu = log(log_zn) * log2_inv;
            double n_frac = n + 1 - nu;

            // Take the modulus for cyclic coloring
            X.at<cv::Vec3d>(j, i) = applyContinuousColormap(fmod(n_frac, 255.0));
        } else {
            // Set to black for those pixels in the set
            X.at<cv::Vec3d>(j, i) = {0.0, 0.0, 0.0};
        }
    }
}



/**
 * Function to load the colormap from a CSV file created using the following (reduced) Python script, from matplotlib:
 * 
 * import numpy as np
 * import matplotlib.pyplot as plt
 * n = 1024
 * cmap = plt.get_cmap('twilight', n)
 * colors = cmap(np.linspace(0, 1, n))[:, :3] 
 * np.savetxt('twilight.csv', colors, delimiter=',')
*/ 
vector<cv::Vec3d> Mandelbrot::loadColormap() {
    int colormap_size = 1024;

    // TOOD make path a variable in settings or similar
    ifstream file("colormaps/" + colormap_name + ".csv");
    vector<cv::Vec3d> local_colormap;

    if (!file.is_open()) {
        cerr << "No .csv file for the " << colormap_name << " colormap exists!" << endl;
        return local_colormap;
    }

    // parse the file
    float r, g, b;
    for (int i = 0; i < colormap_size; ++i) {
        file >> r;
        file.ignore(1); // Skip the comma
        file >> g;
        file.ignore(1); // Skip the comma
        file >> b;
        local_colormap.emplace_back(b, g, r);  // OpenCV uses BGR order
    }

    return local_colormap;
}


/**
 * Function to apply continuous colormap with interpolation
 * Needed because cv::COLORMAP_TWILIGHT is not continuous!
 * 
 * Variant that maps a whole matrix X with fractional pixel iteration values at once.
*/ 
void Mandelbrot::applyContinuousColormap(cv::Mat& img_color) {

    int num_colors = static_cast<int>(colormap.size()) - 1;  // -1 since we interpolate between adjacent colors
    
    // Loop through each pixel
    #pragma omp parallel for
    for (int j = 0; j < X.rows; ++j) {
        for (int i = 0; i < X.cols; ++i) {
            // Get normalized value (0 to 1)
            double value = X.at<double>(j, i) / 255.0;

            // Scale value to [0, num_colors], to find the neighboring indices in the colormap
            double scaled_value = value * num_colors;
            int index1 = static_cast<int>(floor(scaled_value));  // Lower index
            int index2 = min(index1 + 1, num_colors);  // Upper index

            // Fractional part for interpolation
            double t = scaled_value - index1;

            // Interpolate between color1 and color2 based on 't'
            cv::Vec3d color1 = colormap[index1];
            cv::Vec3d color2 = colormap[index2];
            cv::Vec3d interpolated_color = interpolateColor(color1, color2, t);

            // Set the pixel color
            img_color.at<cv::Vec3d>(j, i) = interpolated_color;
        }
    }
}

/**
 * Function to apply continuous colormap with interpolation
 * Needed because cv::COLORMAP_TWILIGHT is not continuous!
 * 
 * Variant that calculates the color per fractional iteration value.
*/ 
cv::Vec3d Mandelbrot::applyContinuousColormap(double n_frac) {
    int num_colors = static_cast<int>(colormap.size()) - 1;  // -1 since we interpolate between adjacent colors
    
    double value = n_frac / 255.0;

    // Scale value to [0, num_colors], to find the neighboring indices in the colormap
    double scaled_value = value * num_colors;
    int index1 = static_cast<int>(floor(scaled_value));  // Lower index
    int index2 = min(index1 + 1, num_colors);  // Upper index

    // Fractional part for interpolation
    double t = scaled_value - index1;

    // Interpolate between color1 and color2 based on 't'
    cv::Vec3d color1 = colormap[index1];
    cv::Vec3d color2 = colormap[index2];
    cv::Vec3d interpolated_color = interpolateColor(color1, color2, t);

    return interpolated_color;
}


/**
 * Create a linspace like np.linspace
 */
vector<double> Mandelbrot::linspace(double start, double end, int num) {
    std::vector<double> result;

    // Return empty vector if invalid amount of numbers are requested
    if (num <= 0) {
        return result; 
    }
    
    // Return the start value if only one number is requested
    if (num == 1) {
        result.push_back(start); 
        return result;
    }

    // Else, pre-allocate memory for `num` elements to avoid reallocation by push_back
    result.resize(num);  

    // and divide
    double step = (end - start) / (num - 1);

    for (int i = 0; i < num; i++) {
        result.at(i) = start + i * step;
    }

    return result;
}

/**
 * Function to interpolate between two colors
 */
cv::Vec3d Mandelbrot::interpolateColor(const cv::Vec3d& color1, const cv::Vec3d& color2, double t) {
    return color1 * (1.0f - t) + color2 * t;
}

