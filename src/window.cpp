#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "alchemy/alchemy.hpp"
#include "alchemy/tensor.hpp"

#include <iostream>
using namespace std;

namespace ALCHEMY {

// * Events callback

void win_key_callback(GLFWwindow* window, int key, int scancode, int action, 
                      int mods) {
    Canvas& canvas = 
        *reinterpret_cast<Canvas*>(glfwGetWindowUserPointer(window));
    if (canvas.on_key && !canvas.on_key(key, (Action)action, mods)) return;
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
            case 'Q': // Quit
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case 'R': // Reset camera
                canvas.camera.reset_view();
                break;
            case 'O': // Orthodox
                canvas.camera.ortho = !canvas.camera.ortho;
                canvas.camera.update_proj();
                break;
            case 'W': // Wire
                canvas.wireframe = !canvas.wireframe;
                break;
            case 'C': // Culling
                canvas.cull_face = !canvas.cull_face;
                break;
            case 'A':
                canvas.draw_axes = !canvas.draw_axes;
                break;
            case 'M':
                if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE) {
                    glfwRestoreWindow(window);
                } else {
                    glfwMaximizeWindow(window);
                }
                break;
            default : {}
        }
    }
}

void win_mouse_button_callback(GLFWwindow* window, int button, int action, 
                               int mods) {
    Canvas& canvas =
        *reinterpret_cast<Canvas*>(glfwGetWindowUserPointer(window));
    glfwGetCursorPos(window, &canvas._mouse_x, &canvas._mouse_y);

    if (action == GLFW_RELEASE) canvas._mouse_button = -1;
    if (canvas.on_mouse_button &&
        !canvas.on_mouse_button(button, (Action)action,
                                mods)) {
        return;
    }
    if (action == GLFW_PRESS) {
        canvas._mouse_button = button;
        canvas._mouse_mods = mods;
    }
}

void win_mouse_move_callback(GLFWwindow* window, double x, double y) {
    bool prevent_default = false;

    Canvas& canvas =
        *reinterpret_cast<Canvas*>(glfwGetWindowUserPointer(window));
    // double prex = canvas._mouse_x, prey = canvas._mouse_y;
    // canvas._mouse_x = x, canvas._mouse_y = y;
    // if (canvas.on_mouse_move && !canvas.on_mouse_move(x, y)) {
    //     return;
    // }
    // if (canvas._mouse_button != -1) {
    //     if ((canvas._mouse_button == GLFW_MOUSE_BUTTON_LEFT &&
    //          (canvas._mouse_mods & GLFW_MOD_SHIFT)) ||
    //         canvas._mouse_button == GLFW_MOUSE_BUTTON_MIDDLE) {
    //         // Pan
    //         canvas.camera.pan_with_mouse((float)(x - prex), (float)(y - prey));
    //     } else if (canvas._mouse_button == GLFW_MOUSE_BUTTON_LEFT &&
    //                (canvas._mouse_mods & GLFW_MOD_CONTROL)) {
    //         // Roll
    //         canvas.camera.roll_with_mouse((float)(x - prex), (float)(y - prey));
    //     } else if (canvas._mouse_button == GLFW_MOUSE_BUTTON_LEFT) {
    //         canvas.camera.rotate_with_mouse((float)(x - prex),
    //                                         (float)(y - prey));
    //     }
    // }
}

void win_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Canvas& canvas =
        *reinterpret_cast<Canvas*>(glfwGetWindowUserPointer(window));
    // if (canvas.on_scroll && !canvas.on_scroll(xoffset, yoffset)) {
    //     return;
    // }
    // canvas.camera.zoom_with_mouse((float)yoffset);
}

// Window resize
void win_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Canvas& canvas =
        *reinterpret_cast<Canvas*>(glfwGetWindowUserPointer(window));
    canvas.camera.aspect = (float)width / (float)height;
    canvas.camera.update_proj();
    glViewport(0, 0, width, height);
}

Canvas::~Canvas() { glfwTerminate(); }

void Canvas::show() {
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed\n";
    }

    GLFWwindow* window = glfwCreateWindow(_width, _height, title.c_str(), NULL, NULL);
    if (!window) {
        std::cerr << "GLFW window creation failed\n";
        glfwTerminate();
    }

    _window = (void*)window;

    camera.aspect = (float)_width / (float)_height;
    // camera.update_proj();
    // camera.update_view();

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        getchar();
        glfwTerminate();
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDepthFunc(GL_LESS);
    if (cull_face) glEnable(GL_CULL_FACE);

    // * Events
    glfwSetKeyCallback(window, win_key_callback);
    glfwSetMouseButtonCallback(window, win_mouse_button_callback);
    glfwSetCursorPosCallback(window, win_mouse_move_callback);
    glfwSetScrollCallback(window, win_scroll_callback);
    glfwSetFramebufferSizeCallback(window, win_framebuffer_size_callback);
    glfwSetWindowUserPointer(window, this);

    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    glfwSetWindowTitle(window, title.c_str());

    if (on_open) on_open();

    /*
    *  pre mesh
     */

    auto set_light_and_camera = [&]() {};

    _looping = true;
    while (!glfwWindowShouldClose(window)) {
        glClearColor(background[0], background[1], background[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        
        if (cull_face)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);

        // * draw axes

        // * draw mesh


        if (on_loop && on_loop()) {
            // for (auto& mesh : meshes) mesh->update(true);
            // for (auto& pc : point_clouds) pc->update(true);
            camera.update_proj();
            camera.update_view();
        }
        glfwSwapBuffers(window);
        if (loop_wait_events) {
            glfwWaitEvents();
        } else {
            glfwPollEvents();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    _looping = false;

    if (on_close) on_close();

    // for (auto& mesh : meshes) {
    //     mesh->free_bufs();
    // }
    _window = nullptr;
    glfwDestroyWindow(window);
}

}; // namespace ALCHEMY
