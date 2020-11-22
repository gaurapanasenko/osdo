#include <GL/glew.h>
#include "app.h"
#include "conf.h"
#include "shader.h"

static App app;

int app_init(void) {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

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
    app.main_window = window;
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, app_resize);
    glfwSetScrollCallback(window, app_scroll);
    glfwSetKeyCallback(window, app_key);

    // load glew
    // ---------
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        return -1;
    }

    // build and compile our shader zprogram
    // -------------------------------------
    bool success = shader_init(RES_DIR"/lighting.vs",
                               RES_DIR"/lighting.fs", &app.lighting_shader);
    success |= shader_init(RES_DIR"/lightless.vs",
                           RES_DIR"/lightless.fs", &app.lightless_shader);
    if (!success) {
        printf("Failed to compile shaders.\n");
        return -1;
    }

    // load models
    // -----------
    mesh_init(&app.e);
    mesh_init(&app.p);
    mesh_init(&app.cube);

    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    for (int i = 0; i < WINDOWS_NUM; i++) {
        window_init(&app, &app.windows[i]);
    }

    return 0;
}

int app_loop(void) {
    const int status = app_init();
    if (status)
        return status;

    double current_time, last_time = glfwGetTime(),
            next_time = glfwGetTime();
    int nb_frames = 0;

    // render loop
    // -----------
    while(!app_should_closed()) {
        // per-frame time logic
        // --------------------
        current_time = glfwGetTime();
        app.delta_time = current_time - last_time;
        last_time = current_time;

        nb_frames++;
        if (current_time - next_time >= 1.0) {
            printf("%i FPS\n", nb_frames);
            nb_frames = 0;
            next_time += 1.0;
        }

        for (int i = 0; i < WINDOWS_NUM; i++) {
            window_loop(&app, &app.windows[i]);
        }
    }

    for (int i = 0; i < WINDOWS_NUM; i++) {
        window_del(&app.windows[i]);
    }

    mesh_del(&app.e);
    mesh_del(&app.p);
    mesh_del(&app.cube);

    // glfw: terminate, clearing all previously allocated GLFW resources
    // -----------------------------------------------------------------
    glfwTerminate();
    return 0;
}

bool app_should_closed(void) {
    bool should_closed = false;
    size_t size = sizeof(app.windows) / sizeof(Window);
    for (size_t i = 0; i < size; i++) {
        should_closed |= glfwWindowShouldClose(app.windows[i].window);
    }
    return should_closed;
}

Window *app_search_window(GLFWwindow *window) {
    for (size_t i = 0; i < WINDOWS_NUM; i++) {
        if (app.windows[i].window == window)
            return &app.windows[i];
    }
    return NULL;
}

// glfw: when the window size changed this callback function executes
// ------------------------------------------------------------------
void app_resize(GLFWwindow* window, GLint width, GLint height) {
    // make sure the viewport matches the new window dimensions;
    // note that width and height will be significantly larger
    // than specified on retina displays.
    Window *win = app_search_window(window);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    win->screen_width = width; win->screen_height = height;
}

// glfw: when the mouse scroll wheel scrolls, this callback is called
// ------------------------------------------------------------------
void app_scroll(GLFWwindow* window, UNUSED GLdouble xoffset,
                GLdouble yoffset) {
    Window *win = app_search_window(window);
    GLfloat *zoom = &win->scene.zoom;
    if (*zoom >= 0.0f && *zoom <= (float)M_PI / 4)
        *zoom -= (float)yoffset;
    if (*zoom <= 0.0f)
        *zoom = 0.0f;
    if (*zoom >= (float)M_PI / 4)
        *zoom = (float)M_PI / 4;
}

// glfw: when the keyboard was used, this callback is called
// ------------------------------------------------------------------
void app_key(GLFWwindow* window, int key, UNUSED int scancode,
             int action, UNUSED int mods) {
    Window *win = app_search_window(window);
    Scene *scene = &win->scene;
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_TAB:
            scene->active++;
            break;
        case GLFW_KEY_Z:
            if (scene->waterframe) {
                scene->waterframe = 0;
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            } else {
                scene->waterframe = 1;
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            break;
        case GLFW_KEY_X:
            scene->light = (scene->light)? 0 : 1;
            break;
        case GLFW_KEY_1:
            scene->dirLightOn = (scene->dirLightOn)? 0 : 1;
            break;
        case GLFW_KEY_2:
            scene->pointLight1On = (scene->pointLight1On)? 0 : 1;
            break;
        case GLFW_KEY_3:
            scene->pointLight2On = (scene->pointLight2On)? 0 : 1;
            break;
        case GLFW_KEY_4:
            scene->spotLightOn = (scene->spotLightOn)? 0 : 1;
            break;
        case GLFW_KEY_0:
            scene->dirLightOn = 0;
            scene->pointLight1On = 0;
            scene->pointLight2On = 0;
            scene->spotLightOn = 0;
            break;
        case GLFW_KEY_M:
            *scene = scene_init();
            break;
        }
    }
    if (scene->active > 2) scene->active = 0;
}
