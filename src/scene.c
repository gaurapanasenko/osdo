#include "scene.h"
#include "conf.h"
#include "app.h"

Scene scene_init(App *app) {
    Scene new_scene = {
        app,
        {
            object_init(app->meshes, &app->lightless_shader),
            object_init(app->meshes + 1, &app->lightless_shader),
        },
        0, 0, 0, 1, 1, 1, 1, (float)M_PI / 4
    };
    object_translate(&new_scene.object[0], BASIS1POS);
    object_translate(&new_scene.object[1], BASIS2POS);

    object_rotate_all(&new_scene.object[0], BASIS1ROT);
    object_rotate_all(&new_scene.object[1], BASIS2ROT);

    return new_scene;
}
