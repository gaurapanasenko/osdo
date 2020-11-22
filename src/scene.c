#include "scene.h"
#include "shader.h"
#include "mesh.h"

static Renderer scene;

void scene_init(void) {
    Renderer new_scene = {
        SCR_WIDTH, SCR_HEIGHT,
        {
            basis_init(true),
            basis_init(false),
            basis_init(false),
        },
        SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f,
        true, 0., 0., 0, 0, 0, 1, 1, 1, 1, (float)M_PI / 4
    };
    basis_translate(&new_scene.basis[0], BASIS0POS);
    basis_translate(&new_scene.basis[1], BASIS1POS);
    basis_translate(&new_scene.basis[2], BASIS2POS);

    basis_rotate_all(&new_scene.basis[1], BASIS1ROT);
    basis_rotate_all(&new_scene.basis[2], BASIS2ROT);

    scene = new_scene;
}

void scene_reset() {
    scene_init();
}

// process all input
// -----------------
void scene_process_input(GLFWwindow *window);

// glfw: when the window size changed this callback function executes
// ------------------------------------------------------------------
void scene_resize(GLFWwindow* window, GLint width,
                  GLint height) {
    // make sure the viewport matches the new window dimensions;
    // note that width and height will be significantly larger
    // than specified on retina displays.
    glViewport(0, 0, width, height);
    scene.screen_width = width; scene.screen_height = height;
}

// glfw: when the mouse scroll wheel scrolls, this callback is called
// ------------------------------------------------------------------
void scene_scroll(GLFWwindow* window, GLdouble xoffset,
                  GLdouble yoffset) {
    if (scene.zoom >= 0.0f && scene.zoom <= (float)M_PI / 4)
        scene.zoom -= (float)yoffset;
    if (scene.zoom <= 0.0f)
        scene.zoom = 0.0f;
    if (scene.zoom >= (float)M_PI / 4)
        scene.zoom = (float)M_PI / 4;
}

// glfw: when the keyboard was used, this callback is called
// ------------------------------------------------------------------
void scene_key(GLFWwindow* window, int key, int scancode,
               int action, int mods) {
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        scene.active++;
    if (scene.active > 2) scene.active = 0;
    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        if (scene.waterframe) {
            scene.waterframe = 0;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            scene.waterframe = 1;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        scene.light = (scene.light)? 0 : 1;
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        scene.dirLightOn = (scene.dirLightOn)? 0 : 1;
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        scene.pointLight1On = (scene.pointLight1On)? 0 : 1;
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        scene.pointLight2On = (scene.pointLight2On)? 0 : 1;
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        scene.spotLightOn = (scene.spotLightOn)? 0 : 1;
    if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
        scene.dirLightOn = 0;
        scene.pointLight1On = 0;
        scene.pointLight2On = 0;
        scene.spotLightOn = 0;
    }
}

int scene_loop(GLFWwindow* window) {
    // build and compile our shader zprogram
    // -------------------------------------
    GLuint lightingShader;
    GLuint lightlessShader;
    bool success = shader_init(RES_DIR"/lighting.vs",  RES_DIR"/lighting.fs", &lightingShader);
    success |= shader_init(RES_DIR"/lightless.vs", RES_DIR"/lightless.fs", &lightlessShader);
    if (!success) {
        printf("Failed to compile shaders.\n");
        return -1;
    }
    const GLuint * sh;

    // load models
    // -----------
    //Model EModel(RES_DIR"E.bo");
    //Model PModel(RES_DIR"P.bo");
    //Model CubeModel(RES_DIR"Cube.bo");
    Mesh EMesh, PMesh, CubeMesh;
    mesh_init(&EMesh);
    mesh_init(&PMesh);
    mesh_init(&CubeMesh);

    // lamp positions
    // --------------
    vec3 lampPositions[] = {
        {5.0f, 0.0f, 5.0f},
        {-1.0f, 0.0f, 1.0f}
    };

    double lastTime = glfwGetTime();
    int nbFrames = 0;
    mat4 mat4buf, *view, camera_rotation, projection;
    GLdouble currentTime, currentFrame;
    vec4 campos, camdir;

    // render loop
    // -----------
    while(!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) {
            // printf and reset timer
            printf("%i FPS\n", nbFrames);
            nbFrames = 0;
            lastTime += 1.0;
        }

        // per-frame time logic
        // --------------------
        currentFrame = glfwGetTime();
        scene.deltaTime = currentFrame - scene.lastFrame;
        scene.lastFrame = currentFrame;

        // input
        // -----
        //processInput(window);

        // render
        // ------
        glClearColor(0.8f, 0.9f, 0.8f, 1.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = &scene.basis[0].view;
        glm_quat_mat4(scene.basis[0].rotation, camera_rotation);
        basis_get_camera_position(&scene.basis[0], campos);
        basis_get_camera_direction(&scene.basis[0], camdir);

        // activate shader
        if (scene.light) {
            sh = &lightingShader;
            shader_use(*sh);
            shader_set_vec3(*sh, "viewPos", campos);
            shader_set_vec3f(*sh, "objectColor", 0.4f, 0.8f, 0.4f);
            shader_set_float(*sh, "materialShininess", 32.0f);
            shader_set_bool(*sh, "dirLightOn", scene.dirLightOn);
            shader_set_bool(*sh, "pointLight1On", scene.pointLight1On);
            shader_set_bool(*sh, "pointLight2On", scene.pointLight2On);
            shader_set_bool(*sh, "spotLightOn", scene.spotLightOn);

            // directional light
            shader_set_vec3f(*sh, "dirLight.direction", -1.0f, -1.0f, -1.0f);
            shader_set_vec3f(*sh, "dirLight.ambient", 0.05f, 0.05f, 0.05f);
            shader_set_vec3f(*sh, "dirLight.diffuse", 0.4f, 0.4f, 0.4f);
            shader_set_vec3f(*sh, "dirLight.specular", 0.5f, 0.5f, 0.5f);
            // point light 1
            shader_set_vec3(*sh, "pointLight1.position", lampPositions[0]);
            shader_set_vec3f(*sh, "pointLight1.ambient",  0.00f, 0.00f, 0.00f);
            shader_set_vec3f(*sh, "pointLight1.diffuse",  1.0f, 1.0f, 1.0f);
            shader_set_vec3f(*sh, "pointLight1.specular", 0.0f, 0.0f, 0.0f);
            shader_set_float(*sh, "pointLight1.constant", 1.0f);
            shader_set_float(*sh, "pointLight1.linear", 0.0f);
            shader_set_float(*sh, "pointLight1.quadratic", 0.0f);
            // point light 2
            shader_set_vec3(*sh, "pointLight2.position", lampPositions[1]);
            shader_set_vec3f(*sh, "pointLight2.ambient", 0.05f, 0.05f, 0.05f);
            shader_set_vec3f(*sh, "pointLight2.diffuse", 0.8f, 0.8f, 0.8f);
            shader_set_vec3f(*sh, "pointLight2.specular", 1.0f, 1.0f, 1.0f);
            shader_set_float(*sh, "pointLight2.constant", 1.0f);
            shader_set_float(*sh, "pointLight2.linear", 0.09f);
            shader_set_float(*sh, "pointLight2.quadratic", 0.032f);
            // spotLight
            shader_set_vec3(*sh, "spotLight.position", campos);
            shader_set_vec3(*sh, "spotLight.direction", camdir);
            shader_set_vec3f(*sh, "spotLight.ambient", 0.0f, 0.0f, 0.0f);
            shader_set_vec3f(*sh, "spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            shader_set_vec3f(*sh, "spotLight.specular", 1.0f, 1.0f, 1.0f);
            shader_set_float(*sh, "spotLight.constant", 1.0f);
            shader_set_float(*sh, "spotLight.linear", 0.09f);
            shader_set_float(*sh, "spotLight.quadratic", 0.032f);
            shader_set_float(*sh, "spotLight.cutOff", (float)cos(M_RAD * 12.5));
            shader_set_float(*sh, "spotLight.outerCutOff", (float)cos(M_RAD * 15.0));
        } else {
            sh = &lightlessShader;
            shader_use(*sh);
        }

        // pass projection matrix to shader
        glm_perspective(scene.zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT,
                        0.1f, 100.0f, projection);
        shader_set_mat4(*sh, "projection", projection);

        // camera/view transformation
        shader_set_mat4(*sh, "view", *view);
        shader_set_mat4(*sh, "camera_rotation", camera_rotation);

        // render the loaded model
        basis_animate(&scene.basis[1], (GLfloat)scene.deltaTime);
        shader_set_mat4(*sh, "model", scene.basis[1].view);
        glm_quat_mat4(scene.basis[1].rotation, mat4buf);
        shader_set_mat4(*sh, "rotation", mat4buf);
        mesh_draw(&EMesh);

        // render the loaded model
        basis_animate(&scene.basis[2], (GLfloat)scene.deltaTime);
        shader_set_mat4(*sh, "model", scene.basis[2].view);
        glm_quat_mat4(scene.basis[2].rotation, mat4buf);
        shader_set_mat4(*sh, "rotation", mat4buf);
        mesh_draw(&PMesh);

        shader_use(lightlessShader);
        shader_set_mat4(lightlessShader, "projection", projection);
        shader_set_mat4(lightlessShader, "camera_rotation", camera_rotation);
        shader_set_mat4(lightlessShader, "view", *view);
        shader_set_mat4(lightlessShader, "rotation", GLM_MAT4_IDENTITY);
        glm_mat4_identity(mat4buf);
        glm_translate(mat4buf, lampPositions[0]);
        shader_set_mat4(lightlessShader, "model", mat4buf);
        //mesh_draw(&CubeMesh);
        glm_mat4_identity(mat4buf);
        glm_translate(mat4buf, lampPositions[1]);
        shader_set_mat4(lightlessShader, "model", mat4buf);
        //mesh_draw(&CubeMesh);

        // glfw: swap buffers and poll IO events
        // -------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    mesh_del(&EMesh);
    mesh_del(&PMesh);
    mesh_del(&CubeMesh);
    return 0;
}
