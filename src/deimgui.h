#ifndef DEIMGUI_H
#define DEIMGUI_H

#include "window.h"

class DeImgui {
public:
    void init(Window *win);
    ~DeImgui();
    void render();
    void update();
};

#endif
