#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "scene.h"

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "osdo",
                                          NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, scene_resize);
    glfwSetScrollCallback(window, scene_scroll);
    glfwSetKeyCallback(window, scene_key);

    // load glew
    // ---------
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    scene_init();

    int status = scene_loop(window);

    // glfw: terminate, clearing all previously allocated GLFW resources
    // -----------------------------------------------------------------
    glfwTerminate();
    return status;
}
