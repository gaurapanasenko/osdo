#include "window.h"
#include "app.h"
#include "conf.h"
#include "shader.h"

int window_init(App *app, Window *win) {
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "osdo",
                                          NULL, app->main_window);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, app_resize);
    glfwSetScrollCallback(window, app_scroll);
    glfwSetKeyCallback(window, app_key);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


    *win = (Window){SCR_WIDTH, SCR_HEIGHT,
           GLMESH_INIT, GLMESH_INIT, GLMESH_INIT,
           //0, 0,
           scene_init(), window,
           GLM_MAT4_IDENTITY_INIT, NULL, GLM_MAT4_IDENTITY_INIT,
           GLM_MAT4_IDENTITY_INIT, 0.f, GLM_VEC4_BLACK_INIT,
           GLM_VEC4_BLACK_INIT};

    // load models
    // -----------
    win->e = mesh_bind(&app->e);
    win->p = mesh_bind(&app->p);
    win->cube = mesh_bind(&app->cube);

    return 0;
}

void window_del(Window *win) {
    mesh_unbind(&win->e);
    mesh_unbind(&win->p);
    mesh_unbind(&win->cube);
}

int window_loop(App *app, Window *win) {
    const GLuint * sh;
    Scene* scene = &win->scene;

    glfwMakeContextCurrent(win->window);

    // render
    // ------
    glClearColor(0.8f, 0.9f, 0.8f, 1.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // input
    // -----
    window_process_input(app, win);

    win->camera_view = &scene->basis[0].view;
    glm_quat_mat4(scene->basis[0].rotation, win->camera_rotation);
    basis_get_camera_position(
                &scene->basis[0], win->camera_position);
    basis_get_camera_direction(
                &scene->basis[0], win->camera_look_direction);

    // activate shader
    if (scene->light) {
        sh = &app->lighting_shader;
        shader_use(*sh);
        shader_set_vec3(*sh, "viewPos", win->camera_position);
        shader_set_vec3f(*sh, "objectColor", 0.4f, 0.8f, 0.4f);
        shader_set_float(*sh, "materialShininess", 32.0f);
        shader_set_bool(*sh, "dirLightOn", scene->dirLightOn);
        shader_set_bool(*sh, "pointLight1On", scene->pointLight1On);
        shader_set_bool(*sh, "pointLight2On", scene->pointLight2On);
        shader_set_bool(*sh, "spotLightOn", scene->spotLightOn);

        // directional light
        shader_set_vec3f(*sh, "dirLight.direction", -1.0f, -1.0f, -1.0f);
        shader_set_vec3f(*sh, "dirLight.ambient", 0.05f, 0.05f, 0.05f);
        shader_set_vec3f(*sh, "dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader_set_vec3f(*sh, "dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        shader_set_vec3(*sh, "pointLight1.position", LAMP_POSITIONS[0]);
        shader_set_vec3f(*sh, "pointLight1.ambient",  0.00f, 0.00f, 0.00f);
        shader_set_vec3f(*sh, "pointLight1.diffuse",  1.0f, 1.0f, 1.0f);
        shader_set_vec3f(*sh, "pointLight1.specular", 0.0f, 0.0f, 0.0f);
        shader_set_float(*sh, "pointLight1.constant", 1.0f);
        shader_set_float(*sh, "pointLight1.linear", 0.0f);
        shader_set_float(*sh, "pointLight1.quadratic", 0.0f);
        // point light 2
        shader_set_vec3(*sh, "pointLight2.position", LAMP_POSITIONS[1]);
        shader_set_vec3f(*sh, "pointLight2.ambient", 0.05f, 0.05f, 0.05f);
        shader_set_vec3f(*sh, "pointLight2.diffuse", 0.8f, 0.8f, 0.8f);
        shader_set_vec3f(*sh, "pointLight2.specular", 1.0f, 1.0f, 1.0f);
        shader_set_float(*sh, "pointLight2.constant", 1.0f);
        shader_set_float(*sh, "pointLight2.linear", 0.09f);
        shader_set_float(*sh, "pointLight2.quadratic", 0.032f);
        // spotLight
        shader_set_vec3(*sh, "spotLight.position", win->camera_position);
        shader_set_vec3(*sh, "spotLight.direction",
                        win->camera_look_direction);
        shader_set_vec3f(*sh, "spotLight.ambient", 0.0f, 0.0f, 0.0f);
        shader_set_vec3f(*sh, "spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        shader_set_vec3f(*sh, "spotLight.specular", 1.0f, 1.0f, 1.0f);
        shader_set_float(*sh, "spotLight.constant", 1.0f);
        shader_set_float(*sh, "spotLight.linear", 0.09f);
        shader_set_float(*sh, "spotLight.quadratic", 0.032f);
        shader_set_float(*sh, "spotLight.cutOff",
                         (float)cos(M_RAD * 12.5));
        shader_set_float(*sh, "spotLight.outerCutOff",
                         (float)cos(M_RAD * 15.0));
    } else {
        sh = &app->lightless_shader;
        shader_use(*sh);
    }

    // pass projection matrix to shader
    win->resolution = (float)win->screen_width / (float)win->screen_height;
    glm_perspective(scene->zoom, win->resolution,
                    0.1f, 100.0f, win->projection);
    shader_set_mat4(*sh, "projection", win->projection);

    // camera/view transformation
    shader_set_mat4(*sh, "view", *win->camera_view);
    shader_set_mat4(*sh, "camera_rotation", win->camera_rotation);

    // render the loaded model
    basis_animate(&scene->basis[1], (GLfloat)app->delta_time);
    shader_set_mat4(*sh, "model", scene->basis[1].view);
    glm_quat_mat4(scene->basis[1].rotation, win->mat4buf);
    shader_set_mat4(*sh, "rotation", win->mat4buf);
    mesh_draw(&win->e);

    // render the loaded model
    basis_animate(&scene->basis[2], (GLfloat)app->delta_time);
    shader_set_mat4(*sh, "model", scene->basis[2].view);
    glm_quat_mat4(scene->basis[2].rotation, win->mat4buf);
    shader_set_mat4(*sh, "rotation", win->mat4buf);
    mesh_draw(&win->p);

    shader_use(app->lightless_shader);
    shader_set_mat4(app->lightless_shader,
                    "projection", win->projection);
    shader_set_mat4(app->lightless_shader,
                    "camera_rotation", win->camera_rotation);
    shader_set_mat4(app->lightless_shader,
                    "view", *win->camera_view);
    shader_set_mat4(app->lightless_shader,
                    "rotation", GLM_MAT4_IDENTITY);

    glm_mat4_identity(win->mat4buf);
    glm_translate(win->mat4buf, LAMP_POSITIONS[0]);
    shader_set_mat4(app->lightless_shader, "model", win->mat4buf);
    //mesh_draw(&CubeMesh);
    glm_mat4_identity(win->mat4buf);
    glm_translate(win->mat4buf, LAMP_POSITIONS[1]);
    shader_set_mat4(app->lightless_shader, "model", win->mat4buf);
    //mesh_draw(&CubeMesh);

    // glfw: swap buffers and poll IO events
    // -------------------------------------
    glfwSwapBuffers(win->window);

    glfwPollEvents();

    return 0;
}

void window_process_input(App *app, Window *win) {
    GLFWwindow *window = win->window;
    Scene *scene = &win->scene;
    Basis *basis = &scene->basis[scene->active];
    vec3 buf;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    GLfloat s = ((ctrl)? 10.0f : 1.0f) * (GLfloat)app->delta_time,
            a = (scene->active ? -1.f : 1.f) * BASIS_MOVE_SPEED;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glm_vec3_muladds(GLM_YUP, a * s, buf);
        basis_translate(basis, buf);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm_vec3_muladds(GLM_XUP, a * s, buf);
        basis_translate(basis, buf);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm_vec3_muladds(GLM_ZUP, a * s, buf);
        basis_translate(basis, buf);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm_vec3_muladds(GLM_ZUP, -a * s, buf);
        basis_translate(basis, buf);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        glm_vec3_muladds(GLM_YUP, -a * s, buf);
        basis_translate(basis, buf);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm_vec3_muladds(GLM_XUP, -a * s, buf);
        basis_translate(basis, buf);
    }

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        basis_rotate_speed(basis, -s, GLM_ZUP);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        basis_rotate_speed(basis, -s, GLM_YUP);

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        basis_rotate_speed(basis, -s, GLM_XUP);
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        basis_rotate_speed(basis, s, GLM_XUP);

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        basis_rotate_speed(basis, s, GLM_ZUP);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        basis_rotate_speed(basis, s, GLM_YUP);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        basis->animation[2] -= s;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        basis->animation[1] -= s;

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        basis->animation[0] -= s;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        basis->animation[0] += s;

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        basis->animation[2] += s;
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        basis->animation[1] += s;
}
