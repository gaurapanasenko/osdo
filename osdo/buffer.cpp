#include "buffer.h"
#include "glbinder.h"

//static GLenum DRAW_BUFFERS[] = {GL_COLOR_ATTACHMENT0};

bool Buffer::pre_render(GLsizei size[2]) {
    this->ms_fb.bind();

    this->ms_fb.rb_color_multisample(size, this->color_rb);
    this->ms_fb.rb_depth_multisample(size, this->depth_rb);

    if (!this->ms_fb.check()) {
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, size[0], size[1]);
    return true;
}

void Buffer::post_render(GLsizei size[2]) {
    this->ms_fb.unbind();

    {
        GlBinder b = this->fb.binder();
        this->fb.tex_2d(size, this->tex);
    }
    GlBinder b1 = this->ms_fb.binder(GL_READ_FRAMEBUFFER),
            b2 = this->fb.binder(GL_DRAW_FRAMEBUFFER);

    glBlitFramebuffer(0, 0, size[0], size[1], 0, 0, size[0], size[1],
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

const Texture &Buffer::get_tex() {return tex;}
