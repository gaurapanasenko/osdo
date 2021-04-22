#ifndef DEIMGUI_H
#define DEIMGUI_H

#include "window.h"

class DeImgui {
    Window *win;

    bool show_demo_window;
public:
    void init(Window *win);
    ~DeImgui();

    DeImgui() = default;
    DeImgui(const DeImgui&) = delete;
    DeImgui(DeImgui&&) = delete;

    DeImgui& operator=(const DeImgui&) = delete;
    DeImgui& operator=(DeImgui&&) = delete;

    void render();
    void update();
};

#endif
