#include "scene.h"
#include "conf.h"
#include "EASTL/algorithm.h"
using eastl::transform;
using eastl::make_pair;

Scene::Scene(const Context::Models &objects) : objects(objects) {
}
