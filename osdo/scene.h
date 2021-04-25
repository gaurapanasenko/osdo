#ifndef SCENE_H
#define SCENE_H

#include "osdo.h"

#include "object.h"
#include "context.h"

struct Scene {
    hash_map<string, Object> objects;

    Scene(const Context::Models& objects);

    static shared_ptr<Scene> create(const Context::Models& objects);
};

#endif // SCENE_H
