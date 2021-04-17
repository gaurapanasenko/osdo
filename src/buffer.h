#ifndef BUFFER_H
#define BUFFER_H

#include "osdo.h"

typedef struct Buffer {
    GLuint fbo, texture, depthrenderbuffer;
} Buffer;

bool buffer_init(Buffer *this);
Buffer *buffer_create(void);

bool buffer_bind(Buffer *this, int size[2]);
void buffer_unbind(Buffer *this);

void buffer_del(Buffer *this);
void buffer_free(Buffer *this);

#endif // BUFFER_H
