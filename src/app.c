#include "float.h"
#include "app.h"
#include "conf.h"
#include "shader.h"
#include "model.h"

int app_init(App *app) {
    app->models = NULL;
    app->shaders = NULL;
    window_init(&app->window);
    utarray_new(app->objects, &object_icd);
    app->camera = CAMERA;

    window_set_user_pointer(&app->window, app);
    window_set_scroll_cb(&app->window, app_scroll);
    window_set_mouse_motion_cb(&app->window, app_mouse);
    window_set_char_cb(&app->window, app_char_callback);
    window_set_mouse_button_cb(&app->window, app_mouse_button_callback);
    window_set_key_cb(&app->window, app_key);

    // build and compile our shader zprogram
    // -------------------------------------
    if (!app_load_shader(app, "simple") ||
            !app_load_shader(app, "textured") ||
            !app_load_shader(app, "nuklear"))
        return -1;

    Model *model = malloc(sizeof(Model));
    model_init(model, "test");
    model_generate(model);
    HASH_ADD_STR(app->models, name, model);

    // load objects
    // ------------
    Shader *shader;
    HASH_FIND_STR(app->shaders, "simple", shader);
    if (shader == NULL)
        return -1;
    Object object;
    object_init(&object, model, shader);
    utarray_push_back(app->objects, &object);

    scene_init(&app->scene, app->objects);
    camera_translate(&app->camera, BASIS0POS);

    HASH_FIND_STR(app->shaders, "nuklear", shader);
    nk_glfw_init(&app->nkglfw, &app->window, shader);

    return 0;
}

void app_del(App *app) {
    nk_glfw_del(&app->nkglfw);
    scene_del(&app->scene);
    utarray_free(app->objects);

    {Model *i, *tmp;
    HASH_ITER(hh, app->models, i, tmp) {
        model_del(i);
        free(i);
    }}

    {Shader *i, *tmp;
    HASH_ITER(hh, app->shaders, i, tmp) {
        shader_del(i);
        free(i);
    }}
}

int app_loop(App *app) {
    Scene* scene = &app->scene;
    Shader *sh;
    HASH_FIND_STR(app->shaders, "simple", sh);
    struct nk_context *ctx = &app->nkglfw.context;
    struct nk_colorf bg;
    bg.r = 0.8f; bg.g = 0.9f; bg.b = 0.8f; bg.a = 1.0f;
    char text[128];
    void *object;
    vec4 *position;
    vec3 rotation, *animation;
    Transformable *trans;

    // render loop
    // -----------
    while(window_pre_loop(&app->window)) {
        nk_glfw_new_frame(&app->nkglfw);

        if (scene->active)
            snprintf(text, 128, "Object %zu", scene->active);
        else snprintf(text, 128, "Camera");

        if (scene->active) {
            object = (void*)utarray_eltptr(
                         scene->objects, (unsigned)scene->active - 1);
            trans = &((Object*)object)->transformable;
        } else {
            object = (void*)&app->camera;
            trans = &((Camera*)object)->transformable;
        }
        trans->get_position(object, &position);
        trans->get_animation(object, &animation);
        glm_vec3_copy(GLM_VEC3_ZERO, rotation);

        /* GUI */
        if (nk_begin(ctx, text, nk_rect(4, 4, 256, 512),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Position:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 3);
            nk_property_float(ctx, "#X:", -FLT_MAX, *position,
                              FLT_MAX, 0.1f, 0.1f);
            nk_property_float(ctx, "#Y:", -FLT_MAX, *position + 1,
                              FLT_MAX, 0.1f, 0.1f);
            nk_property_float(ctx, "#Z:", -FLT_MAX, *position + 2,
                              FLT_MAX, 0.1f, 0.1f);

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Rotation:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 3);
            nk_property_float(ctx, "#X:", -FLT_MAX, rotation,
                              FLT_MAX, 0.1f, 0.1f);
            nk_property_float(ctx, "#Y:", -FLT_MAX, rotation + 1,
                              FLT_MAX, 0.1f, 0.1f);
            nk_property_float(ctx, "#Z:", -FLT_MAX, rotation + 2,
                              FLT_MAX, 0.1f, 0.1f);
            nk_label(ctx, "Animation:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 3);
            nk_property_float(ctx, "#X:", -FLT_MAX, *animation,
                              FLT_MAX, 0.1f, 0.1f);
            nk_property_float(ctx, "#Y:", -FLT_MAX, *animation + 1,
                              FLT_MAX, 0.1f, 0.1f);
            nk_property_float(ctx, "#Z:", -FLT_MAX, *animation + 2,
                              FLT_MAX, 0.1f, 0.1f);

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

        if (scene->active) {
            Model *model = app->models;
            if (nk_begin(ctx, "Bezier mesh", nk_rect(267, 4, 256, 512),
                         NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                         NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
                nk_layout_row_dynamic(ctx, 25, 1);
                if (nk_button_label(ctx, "Regenerate"))
                    model_generate(model);
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Control points:", NK_TEXT_LEFT);
                for (size_t i = 0; i < model->points_size; i++) {
                    nk_layout_row_dynamic(ctx, 25, 3);
                    nk_property_float(ctx, "#X:", -FLT_MAX, model->points[i],
                                      FLT_MAX, 0.1f, 0.1f);
                    nk_property_float(ctx, "#Y:", -FLT_MAX, model->points[i] + 1,
                                      FLT_MAX, 0.1f, 0.1f);
                    nk_property_float(ctx, "#Z:", -FLT_MAX, model->points[i] + 2,
                                      FLT_MAX, 0.1f, 0.1f);
                }
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Surfaces:", NK_TEXT_LEFT);
                for (size_t i = 0; i < model->surfaces_size; i++) {
                    for (size_t j = 0; j < 4; j++) {
                        nk_layout_row_dynamic(ctx, 25, 4);
                        for (size_t k = 0; k < 4; k++) {
                            nk_property_int(ctx, "#", 0, (int*)(model->surfaces[i][j] + k),
                                              100, 1, 1);
                        }
                    }
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "---", NK_TEXT_LEFT);
                }
            }
            nk_end(ctx);
        }

        // render
        // ------
        glClearColor(bg.r, bg.g, bg.b, bg.a);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // input
        // -----
        app_process_input(app);

        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);

        shader_use(sh);

        // pass projection matrix to shader
        glm_perspective(45.f * (GLfloat) M_RAD,
                        window_get_resolution(&app->window),
                        0.1f, 100.0f, app->projection);
        shader_set_mat4(sh, "projection", app->projection);

        // camera/view transformation
        camera_get_mat4((void*)&app->camera, app->last_camera);
        shader_set_mat4(sh, "camera", app->last_camera);

        trans->rotate_all(object, rotation);

        // render the loaded models
        for_each_utarr (Object, i, scene->objects){
            object_draw(i, app->mat4buf,
                        window_get_delta_time(&app->window));
        }

        nk_glfw_render(&app->nkglfw);
        window_post_loop(&app->window);
    }
    return 0;
}

bool app_load_shader(App *app, const char *name) {
    Shader *shader;
    shader = malloc(sizeof(Shader));
    if (!shader_init(name, shader)) {
        printf("Failed to compile shaders.\n");
        return false;
    }
    HASH_ADD_STR(app->shaders, name, shader);
    return true;
}

void app_scroll(Window* window, GLdouble xoffset, GLdouble yoffset) {
    App *app = window_get_user_pointer(window);
    nk_gflw_scroll_callback(&app->nkglfw, xoffset, yoffset);
}

void app_mouse(Window* window, int offset[2]) {
    App *app = window_get_user_pointer(window);
    GLfloat xoffset = (GLfloat)offset[0], yoffset = (GLfloat)offset[1];

    if (window->mouse_capute) {
        xoffset *= SENSITIVITY;
        yoffset *= SENSITIVITY;

        camera_rotate(&app->camera, xoffset, GLM_YUP);
        camera_rotate(&app->camera, yoffset, GLM_XUP);
    }
}

void app_char_callback(Window* window, unsigned int codepoint) {
    App *app = window_get_user_pointer(window);
    nk_glfw_char_callback(&app->nkglfw, codepoint);
}

void app_mouse_button_callback(
        UNUSED Window *window, UNUSED enum BUTTONS button,
        UNUSED bool pressed) {
    /*model *model;
    mat4 ortho = {
        {2.0f, 0.0f, 0.0f, 0.0f},
        {0.0f,-2.0f, 0.0f, 0.0f},
        {0.0f, 0.0f,-1.0f, 0.0f},
        {-1.0f,1.0f, 0.0f, 1.0f},
    };
    ortho[0][0] *= (GLfloat)app.width;
    ortho[1][1] *= (GLfloat)app.height;
    model = (void*)utarray_eltptr(
                 app.modeles, 0);
    vec4 cp;
    mat4 model;
    object_get_mat4((void*)utarray_eltptr(app.objects, 0), model);
    glm_mat4_mul(app.last_camera, model, model);
    //glm_mat4_mul(ortho, model, model);
    for (size_t i = 0; i < model->points_size; i++) {
        glm_mat4_mulv(model, model->points[i], cp);
        printf("%f %f %f\n", cp[0], cp[1], cp[2]);
    }
    if (action == GLFW_PRESS) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            printf("%f %f\n", app.last_x, app.last_y);
            break;
        }
    }*/
}

// glfw: when the keyboard was used, this callback is called
// ------------------------------------------------------------------
void app_key(Window* window, enum KEYS key, bool pressed) {
    App *app = window_get_user_pointer(window);
    Scene *scene = &app->scene;
    if (pressed) {
        switch (key) {
        case KEY_TAB:
            scene->active++;
            break;
        case KEY_Z:
            scene->wireframe = !scene->wireframe;
            if (scene->wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case KEY_X:
            scene->light = (scene->light)? 0 : 1;
            break;
        case KEY_1:
            scene->dirLightOn = (scene->dirLightOn)? 0 : 1;
            break;
        case KEY_2:
            scene->pointLight1On = (scene->pointLight1On)? 0 : 1;
            break;
        case KEY_3:
            scene->pointLight2On = (scene->pointLight2On)? 0 : 1;
            break;
        case KEY_4:
            scene->spotLightOn = (scene->spotLightOn)? 0 : 1;
            break;
        case KEY_0:
            scene->dirLightOn = 0;
            scene->pointLight1On = 0;
            scene->pointLight2On = 0;
            scene->spotLightOn = 0;
            break;
        case KEY_M:
            scene_del(scene);
            scene_init(scene, app->objects);
            break;
        case KEY_B:
        {
            bool capute = window_is_mouse_caputed(window);
            if (capute)
                window_grab_mouse(window, false);
            else
                window_grab_mouse(window, true);
            window_set_mouse_capute(window, capute);
        }
            break;
        default:
            break;
        }
    }
    if (scene->active > utarray_len(scene->objects)) scene->active = 0;
}


void app_process_input(App *app) {
    Window *window = &app->window;
    Scene *scene = &app->scene;
    void *object;
    Transformable *trans;
    GLfloat delta_time = (GLfloat)window_get_delta_time(window);
    if (scene->active) {
        object = (void*)utarray_eltptr(
                     scene->objects, (unsigned)scene->active - 1);
        trans = &((Object*)object)->transformable;
    } else {
        object = (void*)&app->camera;
        trans = &((Camera*)object)->transformable;
    }

    if (window_is_key_pressed(window, KEY_LEFT_CONTROL))
        delta_time *= 10;

    if (window_is_key_pressed(window, KEY_Q))
        trans->translate(object, GLM_YUP, -delta_time);
    if (window_is_key_pressed(window, KEY_A))
        trans->translate(object, GLM_XUP, -delta_time);

    if (window_is_key_pressed(window, KEY_W))
        trans->translate(object, GLM_ZUP, -delta_time);
    if (window_is_key_pressed(window, KEY_S))
        trans->translate(object, GLM_ZUP, delta_time);

    if (window_is_key_pressed(window, KEY_E))
        trans->translate(object, GLM_YUP, delta_time);
    if (window_is_key_pressed(window, KEY_D))
        trans->translate(object, GLM_XUP, delta_time);

    if (window_is_key_pressed(window, KEY_U))
        trans->rotate(object, GLM_ZUP, -delta_time);
    if (window_is_key_pressed(window, KEY_J))
        trans->rotate(object, GLM_YUP, -delta_time);

    if (window_is_key_pressed(window, KEY_I))
        trans->rotate(object, GLM_XUP, -delta_time);
    if (window_is_key_pressed(window, KEY_K))
        trans->rotate(object, GLM_XUP, delta_time);

    if (window_is_key_pressed(window, KEY_O))
        trans->rotate(object, GLM_ZUP, delta_time);
    if (window_is_key_pressed(window, KEY_L))
        trans->rotate(object, GLM_YUP, delta_time);

    if (window_is_key_pressed(window, KEY_R))
        trans->set_animation(object, GLM_ZUP, -delta_time);
    if (window_is_key_pressed(window, KEY_F))
        trans->set_animation(object, GLM_YUP, -delta_time);

    if (window_is_key_pressed(window, KEY_T))
        trans->set_animation(object, GLM_XUP, -delta_time);
    if (window_is_key_pressed(window, KEY_G))
        trans->set_animation(object, GLM_XUP, delta_time);

    if (window_is_key_pressed(window, KEY_Y))
        trans->set_animation(object, GLM_ZUP, delta_time);
    if (window_is_key_pressed(window, KEY_H))
        trans->set_animation(object, GLM_YUP, delta_time);
}
