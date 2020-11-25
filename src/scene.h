#ifndef SCENE_H
#define SCENE_H
#include <GL/gl.h>

#include "object.h"

typedef struct App App;
typedef struct Scene {
    App *app;
    Object object[2];

    // active element
    int active;

    // states
    bool wireframe;
    bool light;
    bool dirLightOn;
    bool pointLight1On;
    bool pointLight2On;
    bool spotLightOn;

    GLfloat zoom;
} Scene;

Scene scene_init(App *app);

#endif // SCENE_H
