#ifndef SCENE_H
#define SCENE_H

#include "osdo.h"

#include "object.h"

/*#include "EASTL/vector.h"
using namespace eastl;*/

#include "vector"
using std::vector;

struct Scene {
    vector<Object> objects;
    // active element
    size_t active;

    // states
    bool wireframe;
    bool light;
    bool dirLightOn;
    bool pointLight1On;
    bool pointLight2On;
    bool spotLightOn;
    Scene(const vector<Object>& objects = {});
};

#endif // SCENE_H
