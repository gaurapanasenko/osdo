#ifndef SCENE_H
#define SCENE_H

#include "osdo.h"

#include "object.h"
#include "context.h"

#include "EASTL/vector.h"
#include "EASTL/shared_ptr.h"
using eastl::vector;
using eastl::shared_ptr;

struct Scene {
    hash_map<string, Object> objects;

    Scene(const Context::Models& objects);

    static shared_ptr<Scene> create(const Context::Models& objects);
};

#endif // SCENE_H
