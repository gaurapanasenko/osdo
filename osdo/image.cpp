#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <EASTL/algorithm.h>
#include "image.h"

using eastl::min;
using eastl::max;

Image::Image(shared_ptr<const pixel_t[]> data,
             const int width, const int height)
    : data(data), width(width), height(height)
{
}

Image Image::fromFile(const char *path)
{
    int width = 0, height = 0;
    pixel_t (*data)[] = (pixel_t (*)[])stbi_load(path, &width, &height, nullptr, COMP);
    if (data) {
        shared_ptr<pixel_t[]> ptr(data, stbi_image_free);
        return {ptr, width, height};
    }
    return {nullptr, 0, 0};
}
