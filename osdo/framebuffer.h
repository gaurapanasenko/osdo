/**
 * @file framebuffer.h
 * @brief Буфер кадру, що використовується для рендеренгу.
 */
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "osdo.h"
#include "glbindable.h"

class Renderbuffer;
class Texture;

/**
 * @brief Буфер кадру, що використовується для рендеренгу.
 */
class Framebuffer : public GlBindable
{
    /**
     * @brief Віртуальний метод що створює буфер кадру.
     * @return індекс буфера кадру
     */
    GLuint _generate() const override;
    /**
     * @brief Віртуальний метод, що прив'язує контекст до буферу кадру.
     * @param id індекс буфера кадру
     * @param target ціль буфера кадру
     */
    virtual void _bind(const GLuint id, GLenum target) const override;
    /**
     * @brief Віртуальний метод, що задає ціль прив'язки за замовчуванням.
     * @return тип ціль прив'язки за замовчуванням
     */
    virtual GLenum _default() const override;
public:
    Framebuffer();
    ~Framebuffer() override;

    /**
     * @brief Перевіряє, чи готовий буфер до рендеренгу.
     * @param target тип буфера
     * @return статус буфера
     */
    bool check(GLenum target = GL_FRAMEBUFFER);

    /**
     * @brief Підготовлює текстуру з згладжуванням
     * @param size ширина та висота кадру
     * @param texture текстура у яку зберігають кадр з згладжуванням
     */
    void tex_2d_multisample(GLsizei size[2], const Texture& texture);

    /**
     * @brief Підготовлює звичайну текстуру
     * @param size ширина та висота кадру
     * @param texture текстура у яку зберігають кадр
     */
    void tex_2d(GLsizei size[2], const Texture& texture);

    /**
     * @brief Підготовлює рендер буфер `rb`
     * @param rb рендер буфер
     * @param target тип буфера
     */
    void renderbuffer(const Renderbuffer& rb, GLenum target) const;

    /**
     * @brief Підготовлює рендер буфер з згладжуванням для зберігання кольорів.
     * @param size ширина та висота кадру
     * @param rb рендер буфер
     */
    void rb_color_multisample(GLsizei size[2], const Renderbuffer& rb) const;

    /**
     * @brief Підготовлює глибинний рендер буфер з згладжуванням.
     * @param size ширина та висота кадру
     * @param rb рендер буфер
     */
    void rb_depth_multisample(GLsizei size[2], const Renderbuffer& rb) const;

    /**
     * @brief Підготовлює рендер буфер для зберігання кольорів.
     * @param size ширина та висота кадру
     * @param rb рендер буфер
     */
    void rb_color(GLsizei size[2], const Renderbuffer& rb) const;

    /**
     * @brief Підготовлює глибинний рендер буфер.
     * @param size ширина та висота кадру
     * @param rb рендер буфер
     */
    void rb_depth(GLsizei size[2], const Renderbuffer& rb) const;
};

#endif // FRAMEBUFFER_H
