#ifndef SCENE_H
#define SCENE_H

#include "osdo.h"

#include "object.h"
#include "context.h"

#include "EASTL/vector.h"
using eastl::vector;

struct Scene {
    hash_map<string, Object> objects;

    Scene(const Context::Models& objects);
};

#endif // SCENE_H
