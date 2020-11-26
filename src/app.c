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
    glfwSetFramebufferSizeCallback(window, app_resize);
    glfwSetScrollCallback(window, app_scroll);
    glfwSetCursorPosCallback(window, app_mouse);
    glfwSetKeyCallback(window, app_key);

    app.meshes = NULL;
    app.shaders = NULL;
    app.window = window;
    utarray_new(app.objects, &object_icd);
    app.camera = CAMERA;
    app.screen_width = SCR_WIDTH;
    app.screen_width = SCR_HEIGHT;
    app.mouse_capute = false;

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // load glew
    // ---------
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        return -1;
    }

    // build and compile our shader zprogram
    // -------------------------------------
    if (!app_load_shader("lightless"))
        return -1;

    // load meshes
    // -----------
    Mesh *mesh = malloc(sizeof(Mesh));
    mesh_init(mesh);
    HASH_ADD_STR(app.meshes, name, mesh);

    // load objects
    // ------------
    Shader *shader;
    HASH_FIND_STR(app.shaders, "lightless", shader);
    if (shader == NULL)
        return -1;
    Object object;
    object = object_init(mesh, shader);
    utarray_push_back(app.objects, &object);

    scene_init(&app.scene, app.objects);
    camera_translate(&app.camera, BASIS0POS);

    return 0;
}

void app_del(void) {
    utarray_free(app.objects);

    {Shader *i, *tmp;
    HASH_ITER(hh, app.shaders, i, tmp) {
        shader_del(i);
        free(i);
    }}

    {Mesh *i, *tmp;
    HASH_ITER(hh, app.meshes, i, tmp) {
        mesh_del(i);
        free(i);
    }}

    // glfw: terminate, clearing all previously allocated GLFW resources
    // -----------------------------------------------------------------
    glfwTerminate();
}

int app_loop(void) {
    const int status = app_init();
    if (status)
        return status;

    double current_time, last_time = glfwGetTime(),
            next_time = glfwGetTime();
    int nb_frames = 0;
    Scene* scene = &app.scene;
    Shader *sh;
    HASH_FIND_STR(app.shaders, "lightless", sh);
    float resolution;

    // render loop
    // -----------
    while(!app_should_closed()) {
        glfwPollEvents();

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

        // render
        // ------
        glClearColor(0.8f, 0.9f, 0.8f, 1.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // input
        // -----
        app_process_input();

        shader_use(sh);

        // pass projection matrix to shader
        resolution = (float)app.screen_width / (float)app.screen_height;
        glm_perspective(scene->zoom, resolution,
                        0.1f, 100.0f, app.projection);
        shader_set_mat4(sh, "projection", app.projection);

        // camera/view transformation
        camera_get_mat4(&app.camera, app.mat4buf);
        shader_set_mat4(sh, "camera", app.mat4buf);

        // render the loaded models
        for (Object *i = (Object*)utarray_front(scene->objects);
             i != NULL; i = (Object*)utarray_next(scene->objects, i)) {
            object_draw(&app, i);
        }

        // glfw: swap buffers and poll IO events
        // -------------------------------------
        glfwSwapBuffers(app.window);
    }
    return 0;
}

bool app_load_shader(const char *name) {
    Shader *shader;
    shader = malloc(sizeof(Shader));
    if (!shader_init(name, shader)) {
        printf("Failed to compile shaders.\n");
        return false;
    }
    HASH_ADD_STR(app.shaders, name, shader);
    return true;
}

bool app_should_closed(void) {
    return glfwWindowShouldClose(app.window);
}

// glfw: when the window size changed this callback function executes
// ------------------------------------------------------------------
void app_resize(GLFWwindow* window, GLint width, GLint height) {
    // make sure the viewport matches the new window dimensions;
    // note that width and height will be significantly larger
    // than specified on retina displays.
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    app.screen_width = width; app.screen_height = height;
}

// glfw: when the mouse scroll wheel scrolls, this callback is called
// ------------------------------------------------------------------
void app_scroll(UNUSED GLFWwindow* window, UNUSED GLdouble xoffset,
                GLdouble yoffset) {
    GLfloat *zoom = &app.scene.zoom;
    if (*zoom >= 0.0f && *zoom <= (float)M_PI / 4)
        *zoom -= (float)yoffset;
    if (*zoom <= 0.0f)
        *zoom = 0.0f;
    if (*zoom >= (float)M_PI / 4)
        *zoom = (float)M_PI / 4;
}

void app_mouse(UNUSED GLFWwindow* window, double xpos, double ypos) {
    float xoffset = (GLfloat)xpos - app.last_x;
    float yoffset = (GLfloat)ypos - app.last_y;
    app.last_x = (GLfloat)xpos;
    app.last_y = (GLfloat)ypos;

    if (app.mouse_capute) {
        xoffset *= SENSITIVITY;
        yoffset *= SENSITIVITY;

        camera_rotate(&app.camera, xoffset, GLM_YUP);
        camera_rotate(&app.camera, yoffset, GLM_XUP);
    }
}

// glfw: when the keyboard was used, this callback is called
// ------------------------------------------------------------------
void app_key(GLFWwindow* window, int key, UNUSED int scancode,
             int action, UNUSED int mods) {
    Scene *scene = &app.scene;
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_TAB:
            scene->active++;
            break;
        case GLFW_KEY_Z:
            glfwMakeContextCurrent(window);
            scene->wireframe = !scene->wireframe;
            if (scene->wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
            scene_del(scene);
            scene_init(scene, app.objects);
            break;
        case GLFW_KEY_B:
            glfwMakeContextCurrent(window);
            app.mouse_capute = !app.mouse_capute;
            if (app.mouse_capute)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            else
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        }
    }
    if (scene->active > utarray_len(scene->objects)) scene->active = 0;
}


void app_process_input(void) {
    GLFWwindow *window = app.window;
    Scene *scene = &app.scene;
    void *object;
    Transformable *trans;
    GLfloat delta_time = (GLfloat)app.delta_time;
    if (scene->active) {
        object = (void*)utarray_eltptr(
                     scene->objects, (unsigned)scene->active - 1);
        trans = &((Object*)object)->transformable;
    } else {
        object = (void*)&app.camera;
        trans = &((Camera*)object)->transformable;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        delta_time *= 10;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        trans->translate(object, GLM_YUP, -delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        trans->translate(object, GLM_XUP, -delta_time);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        trans->translate(object, GLM_ZUP, -delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        trans->translate(object, GLM_ZUP, delta_time);

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        trans->translate(object, GLM_YUP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        trans->translate(object, GLM_XUP, delta_time);

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        trans->rotate(object, GLM_ZUP, -delta_time);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        trans->rotate(object, GLM_YUP, -delta_time);

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        trans->rotate(object, GLM_XUP, -delta_time);
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        trans->rotate(object, GLM_XUP, delta_time);

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        trans->rotate(object, GLM_ZUP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        trans->rotate(object, GLM_YUP, delta_time);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        trans->set_animation(object, GLM_ZUP, -delta_time);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        trans->set_animation(object, GLM_YUP, -delta_time);

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        trans->set_animation(object, GLM_XUP, -delta_time);
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        trans->set_animation(object, GLM_XUP, delta_time);

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        trans->set_animation(object, GLM_ZUP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        trans->set_animation(object, GLM_YUP, delta_time);
}
