#include "osdo.h"

void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
    return malloc(size);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) {
    return malloc(size);
}

vec3 BASIS0POS = { 0.0f, 0.0f,-32.0f};
vec3 BASIS1POS = {-8.0f, 0.0f,  0.0f};
vec3 BASIS2POS = { 8.0f, 0.0f,  0.0f};

vec3 BASIS1ROT = { 0.0f, 0.0f,  0.2f};
vec3 BASIS2ROT = { 0.0f, 0.0f, -0.2f};
