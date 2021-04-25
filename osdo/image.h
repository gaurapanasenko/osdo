#ifndef IMAGE_H
#define IMAGE_H
#include <EASTL/shared_ptr.h>

/**
 * Size of pixel in bypes.
 *
 * Name means "components" and was taken from `stb_image.h` header file library.
 */
#define COMP 4

using eastl::shared_ptr;

/**
 * @brief Channel type for image.
 * Used `unsigned char` because our channel values are in range [0, 255]
 * so has 8-bit per channel.
 */
typedef unsigned char channel_t;

/**
 * @brief Pixel type.
 * One pixel has `COMP` channels and channel defined by `channel_t` type.
 * Used 8-bit chanel with values that in range [0, 255].
 * Pixel has 4 channels (red, green, blue and alpha) so takes 4 bytes.
 */
typedef channel_t pixel_t[COMP];

/**
 * @brief Stores pixels, width and height.
 */
class Image
{
public:
    /**
     * @brief Constant pixels array.
     * Stores smart pointer to `height * width` pixels of an image.
     */
    shared_ptr<const pixel_t[]> data;
    /**
     * @brief Width of an image.
     */
    const int width;
    /**
     * @brief Height of an image.
     */
    const int height;

    /**
     * @brief Image construcor.
     * Stores reference to pixels, height and width of an image.
     * @param data Smart reference to array of pixels,
     * size must be `height * width`.
     * @param width Width of an image.
     * @param height Height of an image.
     */
    Image(shared_ptr<const pixel_t[]> data,
          const int width, const int height);
    /**
     * @brief Read image from file.
     * @param path Path of image file.
     * @return `Image` class, that also references to pixels.
     */
    static Image fromFile(const char *path);
};


#endif // IMAGE_H
