#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include "camera.h"
#include "scene.h"
#include "context.h"
#include "conf.h"
#include "EASTL/string.h"
#include "imgui.h"
#include <EASTL/algorithm.h>
#include "buffer.h"
using eastl::max;
using eastl::string;

class SubWindow
{
    bool wireframe;
    bool light_mode;
    bool culling;
    bool transparency;
    bool camera_mode;
    Camera camera;
    Context &context;
    Scene &scene;
    Buffer buffer;

    mat4 projection, last_camera;
public:
    SubWindow(Context &context, Scene &scene);

    bool params_window();

    bool render_window(double delta_time);

    bool loop(size_t id, double delta_time);
};

#endif // SUBWINDOW_H
