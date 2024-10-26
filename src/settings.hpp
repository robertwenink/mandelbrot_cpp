#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

using namespace std;

class Settings {
    public:
        ///////////////////////////////
        /////////// Settings //////////
        ///////////////////////////////

        // Define default values
        int x_resolution = 1920;
        int y_resolution = 1080;
        int nr_frames = 400;
        int max_its = 2000;

        bool gpu = true;
        bool animate = true;
        bool render = true;
        bool liveplotting = true;

        string colormap = "twilight";

        string output_filename = "mandelbrot";
        int fps = 30;

        float xy_smoothing_power = 1.25;

        double start_height = 3.0;
        double start_width = start_height * (static_cast<double>(x_resolution) / y_resolution);

        vector<vector<double>> trajectory_vector = {
            {-0.5, 0, 1}, 
            {0.3602404434377, -0.6413130610647635, pow(4096, 4) / 25.0}
        };

        // Load settings from a YAML file, fallback to defaults if not provided
        void loadFromYaml(const string& filepath) {
            try {
                YAML::Node config = YAML::LoadFile(filepath);

                // Resolution
                x_resolution = config["x_resolution"] ? config["x_resolution"].as<int>() : x_resolution;
                y_resolution = config["y_resolution"] ? config["y_resolution"].as<int>() : y_resolution;
                nr_frames = config["nr_frames"] ? config["nr_frames"].as<int>() : nr_frames;
                max_its = config["max_its"] ? config["max_its"].as<int>() : max_its;

                // Flags
                gpu = config["gpu"] ? config["gpu"].as<bool>() : gpu;
                animate = config["animate"] ? config["animate"].as<bool>() : animate;
                render = config["render"] ? config["render"].as<bool>() : render;
                liveplotting = config["liveplotting"] ? config["liveplotting"].as<bool>() : liveplotting;

                // Filename and fps
                output_filename = config["output_filename"] ? config["output_filename"].as<string>() : output_filename;
                fps = config["fps"] ? config["fps"].as<int>() : fps;

                // Smoothing and zoom properties
                xy_smoothing_power = config["xy_smoothing_power"] ? config["xy_smoothing_power"].as<float>() : xy_smoothing_power;
                start_height = config["start_height"] ? config["start_height"].as<double>() : start_height;
                start_width = start_height * (static_cast<double>(x_resolution) / y_resolution);

                // Trajectory
                if (config["trajectory"]) {
                    trajectory_vector.clear();
                    for (const auto& point : config["trajectory"]) {
                        trajectory_vector.push_back({point[0].as<double>(), point[1].as<double>(), point[2].as<double>()});
                    }
                }
            } catch (const YAML::Exception& e) {
                cerr << "Error loading settings from file: " << e.what() << "\nUsing default settings." << endl;
            }
        }
};

#endif
