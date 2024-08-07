#pragma once

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>

#include <GLFW/glfw3.h>

#include "alchemy/tensor.hpp"

using Vertex    = TENSOR::Matrix<float, 1, 3>;
using Triangle  = TENSOR::Matrix<int  , 1, 3>;

using Color     = TENSOR::Matrix<double, 1, 3>;

using Vec3D     = TENSOR::Matrix<float, 1, 3>;
using Norm3D    = TENSOR::Matrix<float, 1, 3>;
using Point3D   = TENSOR::Matrix<float, 1, 3>;

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

struct Camera {

    // Get camera position
    inline Point3D get_pos() const { return pos; }
    // Update view matrix, call after changing any view parameter
    void update_view();
    void update_proj();
    // Reset view matrix
    void reset_view();
    void reset_proj();
    // * Camera matrices
    TENSOR::Matrix<float, 4, 4> view; 
    TENSOR::Matrix<float, 4, 4> proj;

    // * Handlers
    void rotate_with_mouse(float xoffset, float yoffset);
    void roll_with_mouse(float xoffset, float yoffset);
    void pan_with_mouse(float xoffset, float yoffset);
    void zoom_with_mouse(float amount);

    // Camera mouse control options
    float pan_speed = .0015f, rotate_speed = .008f, scroll_factor = 1.1f;

    // * Projection parameters
    bool ortho = false;
    float FOV, aspect;
    float NCP, FCP;

    // * View parameters

    Norm3D center_of_rot;
    Norm3D world_up, camera_up;
    
    float dist_to_center;
    // Euler angles, for mouse control
    float yaw, pitch, roll;

    Vec3D pos, right;
};

enum class Action {
    release, press, repeat
};

struct Canvas {
    // * Objects
    std::vector<std::unique_ptr<Object> > objects;
    // * Lighting
    Point3D light_pos;
    // * Camera
    Camera camera;
    
    // * Aesthetics
    std::string title = "untitled";

    // Background color
    Color background = Color(0.0f, 0.0f, 0.0f);
    // * Render params
    // Axes? (a)
    bool draw_axes = true;
    // Wireframe mode? (w)
    bool wireframe = false;
    // Backface culling? (c)
    bool cull_face = true;
    // Whether to wait for event on loop
    // true: loops on user input (glfwWaitEvents), saves power and computation
    // false: loops continuously (glfwPollEvents), useful for e.g. animation
    bool loop_wait_events = true;

    // * Events callbacks
    // Called after GL cnotext init
    std::function<void()> on_open;
    // Called when window is about to close
    std::function<void()> on_close;
    // Called per iter of render loop, before on_gui
    // return true if mesh/point cloud/camera data has been updated, false
    // otherwise
    std::function<bool()> on_loop;


    std::function<bool(int, Action, int)> on_key;
    std::function<bool(int, Action, int)> on_mouse_button;
    // Called on mouse move: args(x, y) return false to prevent default
    std::function<bool(double, double)> on_mouse_move;
    // Called on mouse scroll: args(xoffset, yoffset) return false to prevent default
    std::function<bool(double, double)> on_scroll;

    // * other
    int     _width = 800, _height = 600;
    double  _mouse_x, _mouse_y;
    int     _mouse_button = -1, _mouse_mods;

    // ADNANCED: Pointer to GLFW window object
    void* _window = nullptr;

    // True only during the render loop (show())
    bool _looping = false;

    Canvas() {}
    ~Canvas();
    
    void show();

};

}; // namespace ALCHEMY
