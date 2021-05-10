/**
 * @file buffer.h
 * @brief Буфер, у якому відбувається рендеринг у текстуру.
 */
#ifndef BUFFER_H
#define BUFFER_H

#include "osdo.h"
#include "texture.h"
#include "renderbuffer.h"
#include "framebuffer.h"

/**
 * @brief Буфер, у якому відбувається рендеринг у текстуру.
 */
class Buffer {
    /**
     * @brief Текстура у яку відбувається рендеринг.
     */
    Texture tex;
    /**
     * @brief Буфер кольорів для рендеренгу.
     */
    Renderbuffer color_rb;
    /**
     * @brief Глибинний буфер для рендеренгу.
     */
    Renderbuffer depth_rb;
    /**
     * @brief Буфер утвореного кадру рендеренгу із згладжуванням.
     */
    Framebuffer ms_fb;
    /**
     * @brief Кінцевий буфер утвореного кадру.
     */
    Framebuffer fb;
public:
    /**
     * @brief Підгодовка до рендеренгу.
     * @param[in] size ширина та висота кадру.
     * @return статус успішності підготовки буферів до рендеренгу.
     */
    bool pre_render(GLsizei size[2]);
    /**
     * @brief Генерація текстури з утвореного кадру.
     * @param[in] size ширина та висота кадру.
     */
    void post_render(GLsizei size[2]);

    /**
     * @brief Забирає текстуру у яку проводився рендеринг.
     * @return текстура у яку проводився рендеринг
     */
    const Texture& get_tex();
};

#endif // BUFFER_H
