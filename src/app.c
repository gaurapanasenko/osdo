#include <GL/glew.h>
#include "app.h"
#include "conf.h"
#include "shader.h"

static App app;

static void error_callback(int e, const char *d) {
    printf("Error %d: %s\n", e, d);
}

int app_init(void) {
    // glfw: initialize and configure
    // ------------------------------
    glfwSetErrorCallback(error_callback);
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
    glfwSetCharCallback(window, app_char_callback);
    glfwSetMouseButtonCallback(window, app_mouse_button_callback);
    glfwSetCursorPosCallback(window, app_mouse);
    glfwSetKeyCallback(window, app_key);

    app.meshes = NULL;
    app.shaders = NULL;
    app.window = window;
    utarray_new(app.objects, &object_icd);
    app.camera = CAMERA;
    app.width = SCR_WIDTH;
    app.width = SCR_HEIGHT;
    app.mouse_capute = false;

    // load glew
    // ---------
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        return -1;
    }

    // build and compile our shader zprogram
    // -------------------------------------
    if (!app_load_shader("simple") || !app_load_shader("textured")
            || !app_load_shader("nuklear"))
        return -1;

    // load meshes
    // -----------
    Mesh *mesh = malloc(sizeof(Mesh));
    mesh_init(mesh, "cube");
    HASH_ADD_STR(app.meshes, name, mesh);

    // load objects
    // ------------
    Shader *shader;
    HASH_FIND_STR(app.shaders, "simple", shader);
    if (shader == NULL)
        return -1;
    Object object;
    object = object_init(mesh, shader);
    utarray_push_back(app.objects, &object);

    scene_init(&app.scene, app.objects);
    camera_translate(&app.camera, BASIS0POS);

    mesh = malloc(sizeof(Mesh));
    mesh_init(mesh, "nuklear");
    HASH_ADD_STR(app.meshes, name, mesh);
    HASH_FIND_STR(app.shaders, "nuklear", shader);
    nk_glfw_init(&app.nkglfw, &app, mesh, shader);

    return 0;
}

void app_del(void) {
    nk_glfw_del(&app.nkglfw);
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
    HASH_FIND_STR(app.shaders, "simple", sh);
    float resolution;
    struct nk_context *ctx = &app.nkglfw.context;
    struct nk_colorf bg;
    bg.r = 0.10f; bg.g = 0.18f; bg.b = 0.24f; bg.a = 1.0f;

    // render loop
    // -----------
    while(!app_should_closed()) {
        glfwPollEvents();

        glfwGetWindowSize(app.window, &app.width, &app.height);
        glfwGetFramebufferSize(app.window, &app.display_width,
                               &app.display_height);
        app.framebuffer_scale[0] =
                (float)app.display_width / (float)app.width;
        app.framebuffer_scale[1] =
                (float)app.display_height / (float)app.height;

        nk_glfw_new_frame(&app.nkglfw, &app);

        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

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

        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);

        shader_use(sh);

        // pass projection matrix to shader
        resolution = (float)app.width / (float)app.height;
        glm_perspective(45 * M_RAD, resolution, 0.1f, 100.0f,
                        app.projection);
        shader_set_mat4(sh, "projection", app.projection);

        // camera/view transformation
        camera_get_mat4(&app.camera, app.mat4buf);
        shader_set_mat4(sh, "camera", app.mat4buf);

        // render the loaded models
        for (Object *i = (Object*)utarray_front(scene->objects);
             i != NULL; i = (Object*)utarray_next(scene->objects, i)) {
            object_draw(i, app.mat4buf, app.delta_time);
        }

        nk_glfw_render(&app.nkglfw, &app, NK_ANTI_ALIASING_ON,
                       MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

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
    app.width = width; app.height = height;
}

// glfw: when the mouse scroll wheel scrolls, this callback is called
// ------------------------------------------------------------------
void app_scroll(UNUSED GLFWwindow* window, UNUSED GLdouble xoffset,
                GLdouble yoffset) {
    nk_gflw_scroll_callback(&app.nkglfw, xoffset, yoffset);
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

void app_char_callback(
        UNUSED GLFWwindow* window, unsigned int codepoint) {
    nk_glfw_char_callback(&app.nkglfw, codepoint);
}

void app_mouse_button_callback(
        UNUSED GLFWwindow *window, int button, int action, int mods) {
    nk_glfw_mouse_button_callback(
                &app.nkglfw, &app, button, action, mods);
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
