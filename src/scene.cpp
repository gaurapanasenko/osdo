#include "scene.h"
#include "conf.h"

Scene::Scene(const vector<Object>& objects)
    : objects(objects),
      active(0),
      wireframe(1),
      light(1),
      dirLightOn(0),
      pointLight1On(1),
      pointLight2On(1),
      spotLightOn(1)
{
}
