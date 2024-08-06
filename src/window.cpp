#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "alchemy/alchemy.hpp"
#include "alchemy/tensor.hpp"

namespace ALCHEMY {
Canvas::~Canvas() { glfwTerminate(); }

void Canvas::show() {
    GLFWwindow* window =
        glfwCreateWindow(_width, _height, "meshview", NULL, NULL);
    if (!window) {
        glfwTerminate();
        std::cerr << "GLFW window creation failed\n";
        return;
    }
    _window = (void*)window;

    // camera.aspect = (float)_width / (float)_height;
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
    // if (cull_face) glEnable(GL_CULL_FACE);
}

}; // namespace ALCHEMY
