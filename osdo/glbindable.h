/**
 * @file glbindable.h
 * @brief Абстрактний клас, який виконує роль генерації та прив'язки об'єктів OpenGL.
 */
#ifndef GLBINDABLE_H
#define GLBINDABLE_H

#include "osdo.h"

class GlBinder;

/**
 * @brief Абстрактний клас, який виконує роль генерації та прив'язки об'єктів OpenGL.
 */
class GlBindable
{
    /**
     * @brief Індекс об'єкту OpenGL.
     */
    const GLuint id;
protected:
    /**
     * @brief Віртуальний метод що створює об'єкт.
     * @return індекс об'єкту OpenGL
     */
    virtual GLuint _generate() const;
    /**
     * @brief Віртуальний метод, що прив'язує контекст OpenGL до об'єкту.
     * @param id індекс об'єкту OpenGL
     * @param target ціль прив'язки об'єкту
     */
    virtual void _bind(const GLuint id, GLenum target) const;
    /**
     * @brief Віртуальний метод, що задає ціль прив'язки за замовчуванням.
     * @return ціль прив'язки за замовчуванням
     */
    virtual GLenum _default() const;
protected:
    GlBindable();
    /**
     * @brief Конструктор з параметром.
     * @param id індекс об'єкту OpenGL
     */
    GlBindable(const GLuint id);
public:
    virtual ~GlBindable();

    GlBindable(const GlBindable&) = delete;
    GlBindable(GlBindable&&) = delete;
    GlBindable& operator=(const GlBindable&) = delete;
    GlBindable& operator=(GlBindable&&) = delete;

    /**
     * @brief Повертає індекс об'єкту OpenGL.
     * @return індекс об'єкту OpenGL
     */
    const GLuint& get_id() const;
    /**
     * @brief Повертає індекс об'єкту OpenGL у типі `void*`
     * @return індекс об'єкту OpenGL
     */
    void* get_vid() const;

    /**
     * @brief Прив'язує контекст до об'єкту із ціллю за замовчуванням.
     */
    void bind() const;
    /**
     * @brief Прив'язує контекст до об'єкту із ціллю `target`
     * @param target ціль прив'язки об'єкту
     */
    void bind(GLenum target) const;
    /**
     * @brief Відв'язує контекст до об'єкту із ціллю за замовчуванням.
     */
    void unbind() const;
    /**
     * @brief Відв'язує контекст до об'єкту із ціллю `target`
     * @param target ціль прив'язки об'єкту
     */
    void unbind(GLenum target) const;

    /**
     * @brief Повертає об'єкт прив'язки контексту із ціллю за замовчуванням
     * @return об'єкт прив'язки контексту із ціллю за замовчуванням
     */
    GlBinder binder() const;
    /**
     * @brief Повертає об'єкт прив'язки контексту із ціллю `target`
     * @param target ціль прив'язки об'єкту
     * @return об'єкт прив'язки контексту із ціллю `target`
     */
    GlBinder binder(GLenum target) const;
};

#endif // GLBINDABLE_H
