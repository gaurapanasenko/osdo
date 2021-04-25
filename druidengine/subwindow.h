#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include "camera.h"
#include "context.h"
#include "scene.h"
#include "buffer.h"
#include "window.h"

class SubWindow
{
    bool wireframe;
    bool light_mode;
    bool culling;
    bool transparency;
    bool camera_mode;
    bool textured;
    bool pre_generated;
    int level[2];
    Camera camera;
    Context &context;
    shared_ptr<Scene> scene;
    Buffer buffer;
    Window &window;

    mat4 projection, last_camera;
public:
    SubWindow(Window &window, Context &context, shared_ptr<Scene> scene);

    bool params_window();

    bool render_window(double delta_time);

    bool loop(size_t id, double delta_time);
    shared_ptr<Scene>& get_scene();
};

#endif // SUBWINDOW_H
