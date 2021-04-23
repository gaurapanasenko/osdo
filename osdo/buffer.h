#ifndef BUFFER_H
#define BUFFER_H

#include "osdo.h"
#include "texture.h"
#include "renderbuffer.h"
#include "framebuffer.h"

class Buffer {
    Texture tex;
    Renderbuffer color_rb, depth_rb;
    Framebuffer ms_fb, fb;
public:
    bool pre_render(GLsizei size[2]);
    void post_render(GLsizei size[2]);

    const Texture& get_tex();
};

#endif // BUFFER_H
