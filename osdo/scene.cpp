#include "scene.h"
#include "conf.h"
#include "EASTL/algorithm.h"
using eastl::transform;
using eastl::make_shared;

Scene::Scene(const Context::Models &objects) : objects(objects) {
}

shared_ptr<Scene> Scene::create(const Context::Models &objects)
{
    return make_shared<Scene>(objects);
}
