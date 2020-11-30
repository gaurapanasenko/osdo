#include "scene.h"
#include "conf.h"
#include "app.h"

void scene_init(Scene *scene, UT_array *objects) {
    *scene = (Scene){
        NULL, 0, 1, 0, 1, 1, 1, 1
    };
    utarray_new(scene->objects, &object_icd);
    if (utarray_len(objects))
        utarray_inserta(scene->objects, objects, 0);
}

void scene_del(Scene *scene) {
    utarray_free(scene->objects);
}
