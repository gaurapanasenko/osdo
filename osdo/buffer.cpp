#include "buffer.h"
#include "stdio.h"

bool Buffer::pre_render(GLsizei size[2]) {
    this->ms_fb.bind();

    this->ms_fb.tex_2d_multisample(size, this->ms_tex);
    this->ms_fb.rb_depth_multisample(size, this->depth_rb);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

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

    glBlitFramebuffer(0, 0, size[0], size[1], 0, 0, size[0], size[1], GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
