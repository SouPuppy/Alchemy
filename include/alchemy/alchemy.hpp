#pragma once

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include <algorithm>
#include <memory>

#include <GLFW/glfw3.h>

#include "alchemy/common.hpp"
#include "alchemy/tensor.hpp"

#include <bitset>

using Color     = TENSOR::Matrix<double, 1, 3>;

using Vec3D     = TENSOR::Matrix<float, 1, 3>;
using Norm3D    = TENSOR::Matrix<float, 1, 3>;
using Point3D   = TENSOR::Matrix<float, 1, 3>;
using Triangle  = TENSOR::Matrix<Point3D, 1, 3>;

using Vertex    = Point3D;
using Face      = TENSOR::Matrix<int, 1, 3>;

using AABB      = TENSOR::Matrix<Point3D, 1, 2>;

namespace ALCHEMY { 

struct Node {
    int l, r;
    int parent;
    bool isLeaf() { return l == 0 && r == 0; }
};

struct OctNode {
    OctNode* son[8];
    AABB aabb;
    OctNode() { for (int i = 0; i < 8; i++) son[i] = nullptr; }
};

struct OCTree {
    OctNode* root;
    OCTree() { root = new OctNode(); }
    void add(const std::vector<int>& idx, const AABB& aabb) {
        OctNode* p = root;
        for (int i = 0; i < idx.size(); i++) {
            int index = idx[i];
            
            if (p->son[index] == nullptr) {
                p->son[index] = new OctNode();
            }
            p = p->son[index];
        }
        p->aabb = aabb;
    }
};

struct Object {
    // * Basics
    std::vector<Vertex> vertices;
    std::vector<Face>   faces;

    Object() = default;
    Object(const std::string& path);
    Triangle get_triangle(int idx);
    Triangle get_triangle(Face& f);

    OCTree octree;

    // * Linear BVH
    // ! using vector is oversimplification
    Point3D limit;  // to resize Points into [0,1].
    std::vector<int>  Sort_idx; // sorted index.
    std::vector<uint> mortonCode;    // Sorted_mortonCode align with faces.
    std::vector<AABB> AABBs;    // Sorted_mortonCode align with faces.

    // * == [0,N-2] node == [N-1,2N-1] leaf ==
    std::vector<uint> Sorted_mortonCode;    // Sorted_mortonCode align with faces.
    std::vector<AABB> Sorted_AABBs;         // AABBs for all nodes.
    std::vector<Node> node;                 // nodes in BVH
    // ! No CUDA Hack
    std::vector<bool> visit;
    #define id(x)  Sort_idx[x]


    void build_LinearBVH();
    void build_OCTree_with_BVH();

    void get_limit();
    // sort faces by MortonCode
    void debug() {
        int N = faces.size();
        for (int i = 0; i < node.size(); i++) {
            std::cout << "Node [" << i << "] " << " \0"[i > 9];
            // std:: cout << std::bitset<30>(Sorted_mortonCode[i]) << "\n";
            // Sorted_AABBs[i][0].print();
            // Sorted_AABBs[i][1].print();
            std::cout << "\n"
            << "L: " << node[i].l << "\n"
            << "R: " << node[i].r << "\n"
            << "F: " << node[i].parent << "\n\n";
        }
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
