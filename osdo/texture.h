/**
 * @file texture.h
 * @brief Задає клас текстури.
 */
#ifndef TEXTURE_H
#define TEXTURE_H

#include "osdo.h"
#include "glbindable.h"

class Image;

/**
 * @brief Клас текстури, що зберігаэться у відеокарті.
 */
class Texture : public GlBindable
{
    /**
     * @brief Віртуальний метод що створює текстуру.
     * @return індекс текстури
     */
    GLuint _generate() const override;
    /**
     * @brief Віртуальний метод, що прив'язує контекст OpenGL до текстури.
     * @param id індекс текстури
     * @param target ціль прив'язки текстури
     */
    virtual void _bind(const GLuint id, GLenum target) const override;
    /**
     * @brief Віртуальний метод, що задає ціль прив'язки за замовчуванням.
     * @return ціль прив'язки за замовчуванням
     */
    virtual GLenum _default() const override;
public:
    Texture() : GlBindable(_generate()) {}
    ~Texture() override;

    /**
     * @brief Завантажує зображення у текстуру.
     * @param image зображення
     */
    void update(const Image& image) const;

    /**
     * @brief Створює текстуру з згладжуванням.
     * @param size ширина та висота кадру
     */
    void make_2d_multisample(GLsizei size[2]) const;

    /**
     * @brief Створює текстуру.
     * @param size ширина та висота кадру
     */
    void make_2d(GLsizei size[2]) const;
};

#endif // TEXTURE_H
