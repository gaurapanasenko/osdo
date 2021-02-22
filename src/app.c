#include "float.h"
#include "app.h"
#include "conf.h"
#include "shader.h"
#include "beziator.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

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

        model_ch.beziator = beziator_create("car", shader);
        model = model_create("teapot", model_ch, &beziator_type);
        beziator_generate(model_ch.beziator);
        object_init(&object, model, shader);
        object_translate(&object, (vec3){-5, 5, 0});
        object_scale(&object, (vec3){4, 4, 4});
        object_rotate(&object, M_PI_F / 1.1f, X);
        HASH_ADD_STR(app->models, name, model);
        utarray_push_back(app->objects, &object);

        /*model_ch.mesh = mesh_create();
        mesh_cube_update(model_ch.mesh);
        model = model_create("cube", model_ch, &mesh_type);
        object_init(&object, model, shader);
        HASH_ADD_STR(app->models, name, model);
        utarray_push_back(app->objects, &object);*/
    }

    scene_init(&app->scene, app->objects);
    camera_translate(&app->camera, BASIS0POS);

    HASH_FIND_STR(app->shaders, "nuklear", shader);
    //nk_glfw_init(&app->nkglfw, &app->window, shader);
    deimgui_init(&app->deimgui, &app->window);

    return 0;
}

void app_del(App *app) {
    //nk_glfw_del(&app->nkglfw);
    deimgui_del(&app->deimgui);
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
    //struct nk_context *ctx = &app->nkglfw.context;
    //struct nk_colorf bg;
    //bg.r = 0.8f; bg.g = 0.9f; bg.b = 0.8f; bg.a = 1.0f;
    char text[128];
    vec4 *position, direction;
    vec3 rotation, *animation;
    Bijective *bijective;
    bool light = false;
    vec4 *points = NULL;
    size_t points_size = 0;

    GLuint texture, texture2;
    glGenTextures(1, &texture);
    glGenTextures(1, &texture2);

    GLuint fbo, fbo_ms, texture_ms, depth_rbo, color_rbo;
    GLuint fbo2, fbo_ms2, texture_ms2, depth_rbo2, color_rbo2;
    glEnable(GL_MULTISAMPLE);
    glGenFramebuffers(1, &fbo);
    glGenFramebuffers(1, &fbo_ms);
    glGenTextures(1, &texture_ms);
    glGenRenderbuffers(1, &depth_rbo);
    glGenRenderbuffers(1, &color_rbo);
    glGenFramebuffers(1, &fbo2);
    glGenFramebuffers(1, &fbo_ms2);
    glGenTextures(1, &texture_ms2);
    glGenRenderbuffers(1, &depth_rbo2);
    glGenRenderbuffers(1, &color_rbo2);

    // render loop
    // -----------
    while(window_alive(&app->window)) {
        //nk_glfw_begin_input(&app->nkglfw);
        window_pre_loop(&app->window);
        //nk_glfw_end_input(&app->nkglfw);
        deimgui_update(&app->deimgui);

        if (scene->active)
            snprintf(text, 128, "Object %zu", scene->active);
        else snprintf(text, 128, "Camera");

        if (scene->active) {
            bijective = (void*)utarray_eltptr(
                         scene->objects, (unsigned)scene->active - 1);
        } else {
            bijective = (void*)&app->camera;
        }
        bijective_get_position(bijective, &position);
        bijective_get_animation(bijective, &animation);
        glm_vec3_copy(GLM_VEC3_ZERO, rotation);

        /* GUI */
        /*if (nk_begin(ctx, "OSDO", nk_rect(4, 4, 256, 512),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_checkbox_label(ctx, "Wireframe",
                              &app->scene.wireframe);
            nk_checkbox_label(ctx, "Light",
                              &light);
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
        nk_end(ctx);*/

        /*if (scene->active) {
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
        }*/

        // render
        // ------
        glClearColor(1,1,1,1);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // input
        // -----
        app_process_input(app);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        igSetNextWindowPos((ImVec2){4, 4}, ImGuiCond_FirstUseEver, (ImVec2){0,0});
        igSetNextWindowSize((ImVec2){300, 512}, ImGuiCond_FirstUseEver);

        igBegin("OSDO", NULL, 0);
        igCheckbox("Wireframe", &app->scene.wireframe);
        igCheckbox("Light", &light);
        igText("Active element: %s", text);
        {
            const ImU64 u64_zero = 0;
            const ImU64 length = utarray_len(scene->objects);
            igSliderScalar("", ImGuiDataType_U64, &scene->active, &u64_zero, &length, "%i", 0);
        }
        igInputFloat3("Potision", *position, "%g", 0);
        igInputFloat3("Rotation", rotation, "%g", 0);
        igInputFloat3("Animation", *animation, "%g", 0);
        igEnd();

        if (scene->active) {
            Beziator *beziator = app->models->model.beziator;
            Object *obj = (void*)utarray_front(scene->objects);

            mat4 matr = GLM_MAT4_IDENTITY_INIT;
            //glm_scale(matr, (vec3){(float)app->window.size[0], (float)app->window.size[1], 1});
            glm_mat4_mul(matr, app->projection, matr);
            glm_mat4_mul(matr, app->last_camera, matr);
            glm_mat4_mul(matr, obj->transform, matr);
            vec4 p;
            int active_elem = -1;
            float min_d = FLT_MAX, cur_d;

            for (size_t i = 0; i < beziator->points_size; i++) {
                glm_mat4_mulv(matr, beziator->points[i], p);
                cur_d = glm_vec3_distance2(GLM_VEC3_ZERO, p);
                if (min_d > cur_d) {
                    min_d = cur_d;
                    active_elem = (int)i;
                }
            }

            igSetNextWindowPos((ImVec2){4, 4}, ImGuiCond_Always, (ImVec2){0,0});
            igBeginTooltip();
            igText("%i", active_elem);
            igEndTooltip();

            igSetNextWindowPos((ImVec2){308, 4}, ImGuiCond_FirstUseEver, (ImVec2){0,0});
            igSetNextWindowSize((ImVec2){300, 600}, ImGuiCond_Always);
            igBegin("Bezier mesh", NULL, 0);
            if (igButton("Regenerate", (ImVec2){0, 0}))
                beziator_generate(beziator);
            if (igButton("Save", (ImVec2){0, 0}))
                beziator_save(beziator);
            igBeginChildStr("Control points", (ImVec2){igGetWindowContentRegionWidth(), 260}, true, 0);
            bool changed = false;
            for (int i = 0; i < (int)beziator->points_size; i++) {
                igPushIDInt(i);
                changed |= igInputFloat3("##empty", beziator->points[i], "%g", 0);
                igSameLine(0, 5);
                igText("%i", i);
                igPopID();
            }
            igEndChild();
            igBeginChildStr("Surfaces", (ImVec2){igGetWindowContentRegionWidth(), 260}, true, 0);
            for (int i = 0; i < (int)beziator->surfaces_size; i++) {
                igPushIDInt(i);
                igText("%i", i);
                if (igButton("Invert light normals", (ImVec2){0, 0})) {
                    beziator_rotate(beziator, i);
                    beziator_generate(beziator);
                }
                changed |= igInputInt4("##first", beziator->surfaces[i][0], 0);
                changed |= igInputInt4("##second", beziator->surfaces[i][1], 0);
                changed |= igInputInt4("##third", beziator->surfaces[i][2], 0);
                changed |= igInputInt4("##fourth", beziator->surfaces[i][3], 0);
                igPopID();
            }
            igEndChild();
            if (changed)
                beziator_generate(beziator);
            igEnd();
        }

        igBegin("Win", false, 0);
        ImVec2 vMin, vMax, size, win_pos;
        igGetWindowPos(&win_pos);
        igGetWindowContentRegionMin(&vMin);
        igGetWindowContentRegionMax(&vMax);
        size.x = max(vMax.x-vMin.x, 5); size.y = max(vMax.y-vMin.y, 5);

        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo_ms);

            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_ms);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, (GLsizei)size.x, (GLsizei)size.y, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_ms, 0);

            /*glBindRenderbuffer(GL_RENDERBUFFER, color_rbo);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, (GLsizei)size.x, (GLsizei)size.y);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_rbo);*/

            glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, (GLsizei)size.x, (GLsizei)size.y);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
                glEnable(GL_DEPTH_TEST);
                glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
                glClearColor(1, 1, 1, 1);
                glClearDepth(1.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glActiveTexture(GL_TEXTURE0);

                shader_use(sh);

                // pass projection matrix to shader
                glm_perspective(45.f * (GLfloat) M_RAD, size.x / size.y,
                                0.01f, 100.0f, app->projection);
                //glm_mat4_identity(app->projection);
                shader_set_mat4(sh, "projection", app->projection);
                shader_set_float(sh, "materialShininess", 32.0f);
                shader_set_vec3(sh, "objectColor", (vec3){0,1,0});

                // directional light
                camera_get_direction(&app->camera, direction);
                if (!light)
                    shader_set_vec3(sh, "dirLight.direction", (vec3){0,0,-0.5});
                else
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
                //glEnable(GL_CULL_FACE);
                //glCullFace(GL_BACK);
                //glFrontFace(GL_CCW);

                //glEnable(GL_BLEND);
                //glBlendEquation(GL_FUNC_ADD);
                //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                //glEnable(GL_ALPHA_TEST);
                //glAlphaFunc(GL_GREATER, 0.0f);

                if (!scene->wireframe)
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                // render the loaded models
                for_each_utarr (Object, i, scene->objects){
                    object_draw(i, app->mat4buf,
                                window_get_delta_time(&app->window));
                }
            } else {
                printf("Error\n");
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)size.x, (GLsizei)size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_ms);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                printf("error\n");
            glDrawBuffer(GL_BACK);
            glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }


        {
            ImVec2 cursorBegin;
            igGetCursorPos(&cursorBegin);
            igImage(((void*)(intptr_t)texture), size, (ImVec2){0, 0}, (ImVec2){1, 1}, (ImVec4){1,1,1,1}, (ImVec4){0,0,0,0});

            igSetCursorPos(cursorBegin);
            igInvisibleButton("##empty", size, ImGuiButtonFlags_AllowItemOverlap);
            const bool is_hovered = igIsItemHovered(0);
            const bool is_active = igIsItemActive();
            if (is_hovered) {
                ImGuiIO *io = igGetIO();
                if (is_active) {
                    vec3 mouse_pos = {io->MouseDelta.y / size.y * 2, -io->MouseDelta.x / size.x * 2, 0};
                    camera_rotate_all(&app->camera, mouse_pos);
                }
                camera_translate_bijective(&app->camera, (vec3){0, 0, -io->MouseWheel}, 1);
            }
            ImDrawList *dl = igGetWindowDrawList();
            if (app->scene.active) {
                Beziator *beziator = app->models->model.beziator;
                Object *obj = (void*)utarray_front(scene->objects);
                object_get_mat4(obj, app->mat4buf);
                mat4 matr = GLM_MAT4_IDENTITY_INIT;
                glm_mat4_mul(app->mat4buf, matr, matr);
                glm_mat4_mul(app->last_camera, matr, matr);
                glm_mat4_mul(app->projection, matr, matr);
                float minz = 0, maxz = 0;
                if (points != NULL && points_size != beziator->points_size) {
                    points_size = beziator->points_size;
                    points = (vec4*)realloc(points, points_size * sizeof(vec4));
                }
                if (points == NULL) {
                    points_size = beziator->points_size;
                    points = malloc(points_size * sizeof(vec4));
                }

                for (size_t i = 0; i < beziator->points_size; i++) {
                    //glm_mat4_mulv(matr, beziator->points[i], p);
                    glm_project(beziator->points[i], matr, (vec4){0, 0, size.x, size.y}, points[i]);
                    if (!i) {
                        minz = points[i][2];
                        maxz = points[i][2];
                    } else {
                        if (minz > points[i][2]) minz = points[i][2];
                        if (maxz < points[i][2]) maxz = points[i][2];
                    }
                }
                for (size_t i = 0; i < points_size; i++) {
                    points[i][2] = (maxz - points[i][2]) / (maxz - minz) * 0.6f + 0.4f;
                }
                ImVec2 pos, sz;
                char buf[16];
                for (size_t i = 0; i < points_size; i++) {
                    pos.x = points[i][0] + win_pos.x + cursorBegin.x;
                    pos.y = points[i][1] + win_pos.y + cursorBegin.y;
                    igPushStyleColorVec4(ImGuiCol_Text, (ImVec4){0,0,0,points[i][2]});
                    igSetCursorPos((ImVec2){points[i][0] + cursorBegin.x, points[i][1] + cursorBegin.y});
                    sprintf(buf, "%zu", i);
                    igCalcTextSize(&sz, buf, NULL, false, 0);
                    ImDrawList_AddRectFilled(dl, (ImVec2){pos.x-2,pos.y-2}, (ImVec2){pos.x+ sz.x+2, pos.y+ sz.y+2}, igGetColorU32Vec4((ImVec4){0,1,0,points[i][2]}), 4, ImDrawCornerFlags_All);
                    igText(buf);
                    igPopStyleColor(1);
                }
            }
        }

        igEnd();

        if (scene->active) {
            Beziator *beziator = app->models->model.beziator;
            Object *obj = (void*)utarray_front(scene->objects);

            igSetNextWindowSize((ImVec2){512, 512}, ImGuiCond_FirstUseEver);

            igBegin("Win2", false, 0);
            ImVec2 vMin, vMax, size, win_pos;
            igGetWindowPos(&win_pos);
            igGetWindowContentRegionMin(&vMin);
            igGetWindowContentRegionMax(&vMax);
            size.x = max(vMax.x-vMin.x, 5); size.y = max(vMax.y-vMin.y, 5);

            {
                glBindFramebuffer(GL_FRAMEBUFFER, fbo_ms2);

                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_ms2);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, (GLsizei)size.x, (GLsizei)size.y, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_ms2, 0);

                glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo2);
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, (GLsizei)size.x, (GLsizei)size.y);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_rbo2);

                if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
                    glEnable(GL_DEPTH_TEST);
                    glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
                    glClearColor(1, 1, 1, 1);
                    glClearDepth(1.0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glActiveTexture(GL_TEXTURE0);

                    shader_use(sh);

                    glm_perspective(45.f * (GLfloat) M_RAD, size.x / size.y,
                                    0.01f, 100.0f, app->projection);
                    shader_set_mat4(sh, "projection", app->projection);
                    shader_set_float(sh, "materialShininess", 32.0f);
                    shader_set_vec3(sh, "objectColor", (vec3){0,1,0});

                    // directional light
                    shader_set_vec3(sh, "dirLight.direction", (vec3){0,0,-1});
                    shader_set_vec3f(sh, "dirLight.ambient", 0.0f, 0.0f, 0.0f);
                    shader_set_vec3f(sh, "dirLight.diffuse", 0.6f, 0.6f, 0.6f);
                    shader_set_vec3f(sh, "dirLight.specular", 0.f, 0.f, 0.f);

                    // camera/view transformation
                    mat4 mat4buf = GLM_MAT4_IDENTITY_INIT;
                    glm_translate_z(mat4buf, -10);
                    shader_set_vec3(sh, "viewPos", (vec3){0,0,-10});
                    shader_set_vec3f(sh, "objectColor", 0.4f, 0.8f, 0.4f);
                    shader_set_float(sh, "materialShininess", 32.0f);
                    shader_set_mat4(sh, "camera", mat4buf);

                    shader_set_float(sh, "alpha", 1.0);

                    // render the loaded models
                    for_each_utarr (Object, i, scene->objects){
                        object_draw(i, app->mat4buf,
                                    window_get_delta_time(&app->window));
                    }
                } else {
                    printf("Error\n");
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                glBindFramebuffer(GL_FRAMEBUFFER, fbo2);

                glBindTexture(GL_TEXTURE_2D, texture2);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)size.x, (GLsizei)size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture2, 0);
                glBindTexture(GL_TEXTURE_2D, 0);

                glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_ms2);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo2);
                if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    printf("error\n");
                glDrawBuffer(GL_BACK);
                glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            ImVec2 cursorBegin;
            igGetCursorPos(&cursorBegin);
            igImage(((void*)(intptr_t)texture2), size, (ImVec2){0, 0}, (ImVec2){1, 1}, (ImVec4){1,1,1,1}, (ImVec4){0,0,0,0});

            igSetCursorPos(cursorBegin);
            igInvisibleButton("##empty", size, ImGuiButtonFlags_AllowItemOverlap);
            const bool is_hovered = igIsItemHovered(0);
            const bool is_active = igIsItemActive();
            if (is_hovered) {
                ImGuiIO *io = igGetIO();
                if (is_active) {
                    object_rotate_bijective(obj, X, -io->MouseDelta.y / size.y * 2);
                    object_rotate_bijective(obj, Y, io->MouseDelta.x / size.x * 2);
                }
                //camera_translate_bijective(&app->camera, (vec3){0, 0, -io->MouseWheel}, 1);
            }
            ImDrawList *dl = igGetWindowDrawList();
            object_get_mat4(obj, app->mat4buf);
            mat4 matr = GLM_MAT4_IDENTITY_INIT;
            glm_mat4_mul(app->mat4buf, matr, matr);
            mat4 mat4buf = GLM_MAT4_IDENTITY_INIT;
            glm_translate_z(mat4buf, -10);
            glm_mat4_mul(mat4buf, matr, matr);
            glm_mat4_mul(app->projection, matr, matr);
            float minz = 0, maxz = 0;

            for (size_t i = 0; i < beziator->points_size; i++) {
                //glm_mat4_mulv(matr, beziator->points[i], p);
                glm_project(beziator->points[i], matr, (vec4){0, 0, size.x, size.y}, points[i]);
                if (!i) {
                    minz = points[i][2];
                    maxz = points[i][2];
                } else {
                    if (minz > points[i][2]) minz = points[i][2];
                    if (maxz < points[i][2]) maxz = points[i][2];
                }
            }
            for (size_t i = 0; i < points_size; i++) {
                points[i][2] = (maxz - points[i][2]) / (maxz - minz) * 0.6f + 0.4f;
            }
            ImVec2 pos, sz;
            char buf[16];

            for (size_t i = 0; i < points_size; i++) {
                pos.x = points[i][0] + win_pos.x + cursorBegin.x;
                pos.y = points[i][1] + win_pos.y + cursorBegin.y;
                igPushStyleColorVec4(ImGuiCol_Text, (ImVec4){0,0,0,points[i][2]});
                igSetCursorPos((ImVec2){points[i][0] + cursorBegin.x, points[i][1] + cursorBegin.y});
                sprintf(buf, "%zu", i);
                igCalcTextSize(&sz, buf, NULL, false, 0);
                ImDrawList_AddRectFilled(dl, (ImVec2){pos.x-2,pos.y-2}, (ImVec2){pos.x+ sz.x+2, pos.y+ sz.y+2}, igGetColorU32Vec4((ImVec4){0,1,0,points[i][2]}), 4, ImDrawCornerFlags_All);
                igText(buf);
                igPopStyleColor(1);
            }
            igEnd();
        }

        //nk_glfw_render(&app->nkglfw);
        deimgui_render(&app->deimgui);
        window_post_loop(&app->window);
    }
    glDeleteRenderbuffers(1, &color_rbo);
    glDeleteRenderbuffers(1, &depth_rbo);
    glDeleteTextures(1, &texture_ms);
    glDeleteFramebuffers(1, &fbo_ms);
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &color_rbo2);
    glDeleteRenderbuffers(1, &depth_rbo2);
    glDeleteTextures(1, &texture_ms2);
    glDeleteFramebuffers(1, &fbo_ms2);
    glDeleteFramebuffers(1, &fbo2);
    glDeleteTextures(1, &texture2);
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

void app_scroll(UNUSED Window* window, UNUSED GLdouble xoffset, UNUSED GLdouble yoffset) {
    //App *app = window_get_user_pointer(window);
    //nk_gflw_scroll_callback(&app->nkglfw, xoffset, yoffset);
}

void app_mouse(Window* window, UNUSED vec2 pos, vec2 offset) {
    App *app = window_get_user_pointer(window);

    if (app->interactive_mode) {
        vec2 offset_sens = GLM_VEC2_ZERO_INIT;
        glm_vec2_muladds(offset, -SENSITIVITY, offset_sens);
        //offset_sens[0] *= -1;

        camera_rotate(&app->camera, offset_sens[0], Y);
        camera_rotate(&app->camera, -offset_sens[1], X);
    }
    //nk_glfw_mouse_callback(&app->nkglfw, pos, offset);
}

void app_char_callback(UNUSED Window* window, UNUSED unsigned int codepoint) {
    //App *app = window_get_user_pointer(window);
    //nk_glfw_char_callback(&app->nkglfw, codepoint);
}

void app_mouse_button_callback(
        UNUSED Window *window, UNUSED enum BUTTONS button,
        UNUSED bool pressed) {
    //App *app = window_get_user_pointer(window);
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
    //nk_glfw_mouse_button_callback(&app->nkglfw, button, pressed);
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
    //nk_glfw_key_callback(&app->nkglfw, key, pressed);
}

static mat3 m3i = GLM_MAT3_IDENTITY_INIT;

void app_process_input(App *app) {
    Window *window = &app->window;
    Scene *scene = &app->scene;
    Bijective *bijective;
    float t;
    GLfloat delta_time = (GLfloat)window_get_delta_time(window);
    if (scene->active) {
        bijective = (void*)utarray_eltptr(
                     scene->objects, (unsigned)scene->active - 1);
    } else {
        bijective = (void*)&app->camera;
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
                    bijective_rotate(bijective, (enum coord_enum)j, t);
                    break;
                case ANIMATE:
                    bijective_set_animation(bijective, m3i[j], t);
                    break;
                }
            }
        }
    }
}
