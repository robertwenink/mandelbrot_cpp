#include "mandelbrot_video.hpp"

class MandelbrotImage : public Mandelbrot {
    
    void run() override {
        // Parameters for the video
        std::string filename = "output_video.mp4";
        int codec = cv::VideoWriter::fourcc('M', 'P', '4', 'V');  // Codec for MP4 (using 'MP4V')
        double fps = 30.0;  // Frames per second
        cv::Size frameSize(640, 480);  // Frame size (width, height)

        // Create a VideoWriter object
        cv::VideoWriter videoWriter(filename, codec, fps, frameSize, true);

        // Check if the VideoWriter object was initialized successfully
        if (!videoWriter.isOpened()) {
            std::cerr << "Could not open the video writer!" << std::endl;
            return;
        }

        // Simulate an iterative process where frames (cv::Mat) are generated
        for (int i = 0; i < 100; ++i) {  // Create 100 frames
            // Create a blank image (e.g., white background)
            cv::Mat frame = cv::Mat::zeros(frameSize, CV_8UC3);

            // Draw something on the image (for example, a moving circle)
            int radius = 50;
            cv::Point center(100 + i * 2, 240);  // Moving the circle horizontally
            cv::Scalar color(0, 255, 0);  // Green color

            cv::circle(frame, center, radius, color, -1);  // Draw filled circle

            // Write the frame to the video
            videoWriter.write(frame);
        }

        // Release the video writer
        videoWriter.release();

        std::cout << "Video written successfully!" << std::endl;
    }
};