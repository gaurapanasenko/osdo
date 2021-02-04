#ifndef DEIMGUI_H
#define DEIMGUI_H

#include "window.h"

typedef struct DeImgui {
    void* ctx;
    void* io;
} DeImgui;

void deimgui_init(DeImgui* deimgui, Window *win);
void deimgui_del(DeImgui* deimgui);
void deimgui_render(DeImgui* deimgui);

void deimgui_update(DeImgui* deimgui);

#endif
