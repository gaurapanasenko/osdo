#include "buffer.h"

Buffer::Buffer() {
    glGenFramebuffers(1, &this->fbo);
    glGenTextures(1, &this->texture);
    glGenRenderbuffers(1, &this->depthrenderbuffer);
}

bool Buffer::bind(int size[2]) {
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size[0], size[1], 0, GL_RGB,
            GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->texture, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, this->depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size[0], size[1]);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, this->depthrenderbuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    glViewport(0, 0, size[0], size[1]);
    return true;
}

void Buffer::unbind() {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Buffer::~Buffer() {
    glDeleteRenderbuffers(1, &this->depthrenderbuffer);
    glDeleteTextures(1, &this->texture);
    glDeleteFramebuffers(1, &this->fbo);
}
