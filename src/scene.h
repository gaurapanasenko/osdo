#ifndef SCENE_H
#define SCENE_H
#include <GL/gl.h>

#include "basis.h"

typedef struct Scene {
    // camera
    Basis basis[3];
    bool firstMouse;

    // active element
    int active;

    // states
    bool waterframe;
    bool light;
    bool dirLightOn;
    bool pointLight1On;
    bool pointLight2On;
    bool spotLightOn;

    GLfloat zoom;
} Scene;

Scene scene_init(void);

#endif // SCENE_H
