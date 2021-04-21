#ifndef BUFFER_H
#define BUFFER_H

#include "osdo.h"

class Buffer {
    GLuint fbo, texture, depthrenderbuffer;
public:
    Buffer();
    ~Buffer();

    bool bind(int size[2]);
    void unbind();
};

#endif // BUFFER_H
