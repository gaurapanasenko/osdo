/**
 * @file renderbuffer.h
 * @brief Задає клас буфера рендеренгу (для зберігання кольорів або глибини).
 */
#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "osdo.h"
#include "glbindable.h"

/**
 * @brief Буфер рендеренгу (для зберігання кольорів або глибини)
 */
class Renderbuffer : public GlBindable
{
    /**
     * @brief Віртуальний метод що створює буфер рендеренгу.
     * @return індекс буфер рендеренгу
     */
    GLuint _generate() const override;
    /**
     * @brief Віртуальний метод, що прив'язує контекст OpenGL до буферу рендеренгу.
     * @param id індекс буфер рендеренгу
     * @param target ціль прив'язки буфера рендеренгу
     */
    virtual void _bind(const GLuint id, GLenum target) const override;
    /**
     * @brief Віртуальний метод, що задає ціль прив'язки за замовчуванням.
     * @return ціль прив'язки за замовчуванням
     */
    virtual GLenum _default() const override;
public:
    /**
     * @brief Конструктор, що створює буфер рендеренгу.
     */
    Renderbuffer() : GlBindable(_generate()) {}
    ~Renderbuffer() override;

    /**
     * @brief Створює буфер рендеренгу з згладжуванням.
     * @param size ширина та висота кадру
     * @param target ціль буферу рендеренгу
     */
    void make_multisample(GLsizei size[2], GLenum target) const;

    /**
     * @brief Створює буфер рендеренгу.
     * @param size ширина та висота кадру
     * @param target ціль буферу рендеренгу
     */
    void make(GLsizei size[2], GLenum target) const;
};

#endif // RENDERBUFFER_H
