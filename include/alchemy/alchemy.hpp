#pragma once

#include <thread>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GLFW/glfw3.h>

#include "alchemy/tensor.hpp"

using Vertex    = TENSOR::Matrix<float, 1, 3>;
using Triangle  = TENSOR::Matrix<int  , 1, 3>;

using Point3D   = TENSOR::Matrix<float, 1, 3>;
using Color     = TENSOR::Vector<float, 3>;

namespace ALCHEMY {

// only face and vertex
struct Object {
    std::vector<Vertex>     vertex;
    std::vector<Triangle>   face;

    Object() = default;
    Object(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string line;
        while (std::getline(file, line)) {
            // cout << line << endl;
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::istringstream input_line(line);
            char type;
            input_line >> type;

            switch (type) {
                case 'v': {
                    float x, y, z;
                    input_line >> x >> y >> z;
                    vertex.push_back(Vertex(x, y, z));
                    break;
                }
                case 'f': {
                    int a, b, c;
                    input_line >> a >> b >> c;
                    face.push_back(Triangle(a, b, c));
                    break;
                }
                default: {
                    // std::cerr << "Unknown line type: " << type << std::endl;
                    break;
                }
            }
        }
        std::cout << "Successfully read in: " << path << "\n"; 
        std::cout << "VERTEX [" << vertex.size() << "]  | FACE [" << face.size() << "]\n ";
    }

    void debug() {

    }
};

struct Light {

};

struct Frustum {
    // Field Of View
    float FOV;
    // Near Clip Plane
    float NCP;
    // Far Clip Plane
    float FCP;
};

struct Camera {
    Frustum frustum;
};

struct Canvas {
    // Objects
    std::vector<std::unique_ptr<Object> > objects;
    // Lighting
    Point3D light_pos;
    // Camera
    Camera camera;
    
    // Aesthetics
    std::string title = "untitled";

    // Background color
    Color background;

    Canvas();
    ~Canvas();
    
    void show();

};

}; // namespace ALCHEMY
