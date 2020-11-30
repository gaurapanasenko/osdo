#ifndef SCENE_H
#define SCENE_H

#include "osdo.h"

#include "object.h"

typedef struct Scene {
    UT_array *objects;

    // active element
    size_t active;

    // states
    int wireframe;
    bool light;
    bool dirLightOn;
    bool pointLight1On;
    bool pointLight2On;
    bool spotLightOn;
} Scene;

void scene_init(Scene *scene, UT_array *objects);

void scene_del(Scene *scene);

#endif // SCENE_H
