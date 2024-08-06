#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "alchemy/alchemy.hpp"
#include "alchemy/tensor.hpp"

namespace ALCHEMY {
Canvas::~Canvas() { 
    glfwDestroyWindow((GLFWwindow*)_window);
    _window = nullptr;
    glfwTerminate();
}

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

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
}

}; // namespace ALCHEMY
