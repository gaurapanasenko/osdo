#include "float.h"
#include "app.h"
#include "conf.h"
#include "shader.h"
#include "beziator.h"

int app_init(App *app) {
    app->models = NULL;
    app->shaders = NULL;
    app->interactive_mode = false;
    window_init(&app->window);
    utarray_new(app->objects, &object_icd);
    app->camera = CAMERA;
    memset(app->trans, 0, sizeof(app->trans));

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
            !app_load_shader(app, "lighting") ||
            !app_load_shader(app, "nuklear") ||
            !app_load_shader(app, "editmode"))
        return -1;

    Shader *shader;
    HASH_FIND_STR(app->shaders, "editmode", shader);
    if (shader == NULL)
        return -1;

    {
        model_t model_ch;
        Model *model;
        Object object;

        model_ch.beziator = beziator_create("test", shader);
        model = model_create("test", model_ch, &beziator_type);
        beziator_generate(model_ch.beziator);
        object_init(&object, model, shader);
        HASH_ADD_STR(app->models, name, model);
        utarray_push_back(app->objects, &object);

        model_ch.mesh = mesh_create();
        mesh_cube_update(model_ch.mesh);
        model = model_create("cube", model_ch, &mesh_type);
        object_init(&object, model, shader);
        HASH_ADD_STR(app->models, name, model);
        utarray_push_back(app->objects, &object);
    }

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

    {
        Model *i, *tmp;
        HASH_ITER(hh, app->models, i, tmp)
            model_free(i);
    }

    {
        Shader *i, *tmp;
        HASH_ITER(hh, app->shaders, i, tmp)
            shader_free(i);
    }
}

int app_loop(App *app) {
    Scene* scene = &app->scene;
    Shader *sh, *sh2;
    HASH_FIND_STR(app->shaders, "editmode", sh);
    HASH_FIND_STR(app->shaders, "simple", sh2);
    struct nk_context *ctx = &app->nkglfw.context;
    struct nk_colorf bg;
    bg.r = 0.8f; bg.g = 0.9f; bg.b = 0.8f; bg.a = 1.0f;
    char text[128];
    vec4 *position, direction;
    vec3 rotation, *animation;
    Bijective bijective;

    // render loop
    // -----------
    while(window_alive(&app->window)) {
        nk_glfw_begin_input(&app->nkglfw);
        window_pre_loop(&app->window);
        nk_glfw_end_input(&app->nkglfw);

        if (scene->active)
            snprintf(text, 128, "Object %zu", scene->active);
        else snprintf(text, 128, "Camera");

        if (scene->active) {
            bijective.bijective.object = (void*)utarray_eltptr(
                         scene->objects, (unsigned)scene->active - 1);
            bijective.type = &object_bijective;
        } else {
            bijective.bijective.camera = &app->camera;
            bijective.type = &camera_bijective;
        }
        bijective_get_position(bijective, &position);
        bijective_get_animation(bijective, &animation);
        glm_vec3_copy(GLM_VEC3_ZERO, rotation);

        /* GUI */
        if (nk_begin(ctx, "OSDO", nk_rect(4, 4, 256, 512),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_checkbox_label(ctx, "Wireframe",
                              &app->scene.wireframe);
            nk_label(ctx, "Active element:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_label(ctx, text, NK_TEXT_LEFT);
            scene->active = (size_t)(nk_propertyi(
                                ctx, "#", 0, (int)scene->active,
                                (int)utarray_len(scene->objects), 1, 0.1f));
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
            Beziator *beziator = app->models->model.beziator;
            if (nk_begin(ctx, "Bezier mesh", nk_rect(267, 4, 256, 512),
                         NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                         NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
                nk_layout_row_dynamic(ctx, 25, 1);
                if (nk_button_label(ctx, "Regenerate"))
                    beziator_generate(beziator);
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Control points:", NK_TEXT_LEFT);
                for (size_t i = 0; i < beziator->points_size; i++) {
                    nk_layout_row_dynamic(ctx, 25, 3);
                    nk_property_float(ctx, "#X:", -FLT_MAX, beziator->points[i],
                                      FLT_MAX, 0.1f, 0.1f);
                    nk_property_float(ctx, "#Y:", -FLT_MAX, beziator->points[i] + 1,
                                      FLT_MAX, 0.1f, 0.1f);
                    nk_property_float(ctx, "#Z:", -FLT_MAX, beziator->points[i] + 2,
                                      FLT_MAX, 0.1f, 0.1f);
                }
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Surfaces:", NK_TEXT_LEFT);
                for (size_t i = 0; i < beziator->surfaces_size; i++) {
                    for (size_t j = 0; j < 4; j++) {
                        nk_layout_row_dynamic(ctx, 25, 4);
                        for (size_t k = 0; k < 4; k++) {
                            nk_property_int(ctx, "#", 0, (int*)(beziator->surfaces[i][j] + k),
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
                        0.01f, 100.0f, app->projection);
        shader_set_mat4(sh, "projection", app->projection);
        shader_set_float(sh, "materialShininess", 32.0f);
        shader_set_vec3(sh, "objectColor", (vec3){0,1,0});

        // directional light
        camera_get_direction(&app->camera, direction);
        shader_set_vec3(sh, "dirLight.direction", direction);
        shader_set_vec3f(sh, "dirLight.ambient", 0.0f, 0.0f, 0.0f);
        shader_set_vec3f(sh, "dirLight.diffuse", 0.6f, 0.6f, 0.6f);
        shader_set_vec3f(sh, "dirLight.specular", 0.f, 0.f, 0.f);

        // camera/view transformation
        camera_get_mat4((void*)&app->camera, app->last_camera);
        shader_set_vec3(sh, "viewPos", app->camera.position);
        shader_set_vec3f(sh, "objectColor", 0.4f, 0.8f, 0.4f);
        shader_set_float(sh, "materialShininess", 32.0f);
        shader_set_mat4(sh, "camera", app->last_camera);
        shader_set_vec2(sh, "vp", (vec2){(float)app->window.size[0], (float)app->window.size[1]});

        shader_set_float(sh, "alpha", 1.0);

        bijective_rotate_all(bijective, rotation);

        // Culling unneded back faced
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glEnable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //glEnable(GL_ALPHA_TEST);
        //glAlphaFunc(GL_GREATER, 0.0f);

        glPointSize(10);
        if (!scene->wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // render the loaded models
        for_each_utarr (Object, i, scene->objects){
            object_draw(i, app->mat4buf,
                        window_get_delta_time(&app->window));
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        nk_glfw_render(&app->nkglfw);
        window_post_loop(&app->window);
    }
    return 0;
}

bool app_load_shader(App *app, const char *name) {
    Shader *shader = shader_create(name);
    if (shader == NULL) {
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

void app_mouse(Window* window, vec2 pos, vec2 offset) {
    App *app = window_get_user_pointer(window);
    GLfloat xoffset = (GLfloat)offset[0], yoffset = (GLfloat)offset[1];

    if (app->interactive_mode) {
        xoffset *= SENSITIVITY;
        yoffset *= SENSITIVITY;

        camera_rotate(&app->camera, xoffset, GLM_YUP);
        camera_rotate(&app->camera, yoffset, GLM_XUP);
    }
    nk_glfw_mouse_callback(&app->nkglfw, pos, offset);
}

void app_char_callback(Window* window, unsigned int codepoint) {
    App *app = window_get_user_pointer(window);
    nk_glfw_char_callback(&app->nkglfw, codepoint);
}

void app_mouse_button_callback(
        Window *window, UNUSED enum BUTTONS button,
        UNUSED bool pressed) {
    App *app = window_get_user_pointer(window);
    /*Object *object = (void*)utarray_eltptr(app->objects, 0);
    Model *model = object->model;
    int *size = window_get_size(&app->window);
    int *cursor = window_get_cursor(&app->window);
    vec4 cp;
    mat4 m;
    object_get_mat4(object, m);
    glm_mat4_mul(app->last_camera, m, m);
    glm_mat4_mul(app->projection, m, m);
    for (size_t i = 0; i < model->points_size; i++) {
        glm_mat4_mulv(m, model->points[i], cp);
        printf("%f %f %f\n", cp[0] / cp[2], cp[1] / cp[2], cp[2]);
    }
    printf("Tap: %f %f\n", (float)cursor[0] * 2.0 / (float)size[0] - 1.0,
        (float)cursor[1] * 2.0 / (float)size[1] - 1.0);*/
    nk_glfw_mouse_button_callback(&app->nkglfw, button, pressed);
}

// glfw: when the keyboard was used, this callback is called
// ------------------------------------------------------------------
void app_key(Window* window, enum KEYS key, bool pressed) {
    App *app = window_get_user_pointer(window);
    Scene *scene = &app->scene;
    int t = pressed ? 1 : -1;
    if (pressed) {
        switch (key) {
        case KEY_TAB:
            scene->active++;
            break;
        case KEY_Z:
            scene->wireframe = !scene->wireframe;
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
            app->interactive_mode = !app->interactive_mode;
            if (!app->interactive_mode)
                window_grab_mouse(window, false);
            else
                window_grab_mouse(window, true);
        }
            break;
        default:
            break;
        }
    }
    switch (key) {
    case KEY_Q: app->trans[TRANSLATE][Y] -= t; break;
    case KEY_A: app->trans[TRANSLATE][X] -= t; break;
    case KEY_W: app->trans[TRANSLATE][Z] -= t; break;
    case KEY_S: app->trans[TRANSLATE][Z] += t; break;
    case KEY_E: app->trans[TRANSLATE][Y] += t; break;
    case KEY_D: app->trans[TRANSLATE][X] += t; break;
    case KEY_U: app->trans[ROTATE   ][Z] += t; break;
    case KEY_J: app->trans[ROTATE   ][Y] -= t; break;
    case KEY_I: app->trans[ROTATE   ][X] -= t; break;
    case KEY_K: app->trans[ROTATE   ][X] += t; break;
    case KEY_O: app->trans[ROTATE   ][Z] -= t; break;
    case KEY_L: app->trans[ROTATE   ][Y] += t; break;
    case KEY_R: app->trans[ANIMATE  ][Z] += t; break;
    case KEY_F: app->trans[ANIMATE  ][Y] -= t; break;
    case KEY_T: app->trans[ANIMATE  ][X] -= t; break;
    case KEY_G: app->trans[ANIMATE  ][X] += t; break;
    case KEY_Y: app->trans[ANIMATE  ][Z] -= t; break;
    case KEY_H: app->trans[ANIMATE  ][Y] += t; break;
    default:
        break;

    }
    if (scene->active > utarray_len(scene->objects)) scene->active = 0;
    nk_glfw_key_callback(&app->nkglfw, key, pressed);
}

static mat3 m3i = GLM_MAT3_IDENTITY_INIT;

void app_process_input(App *app) {
    Window *window = &app->window;
    Scene *scene = &app->scene;
    Bijective bijective;
    float t;
    GLfloat delta_time = (GLfloat)window_get_delta_time(window);
    if (scene->active) {
        bijective.bijective.object = (void*)utarray_eltptr(
                     scene->objects, (unsigned)scene->active - 1);
        bijective.type = &object_bijective;
    } else {
        bijective.bijective.camera = &app->camera;
        bijective.type = &camera_bijective;
    }

    if (window_is_key_pressed(window, KEY_LEFT_CONTROL))
        delta_time *= 10;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (app->trans[i][j]) {
                t = (float)app->trans[i][j] * delta_time;
                switch (i) {
                case TRANSLATE:
                    bijective_translate(bijective, m3i[j], t);
                    break;
                case ROTATE:
                    bijective_rotate(bijective, m3i[j], t);
                    break;
                case ANIMATE:
                    bijective_set_animation(bijective, m3i[j], t);
                    break;
                }
            }
        }
    }
}
