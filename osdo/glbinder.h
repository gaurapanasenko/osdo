/**
 * @file glbinder.h
 * @brief Клас який прив'язує контексту до деякого об'єкту OpenGL.
 */
#ifndef GLBINDER_H
#define GLBINDER_H

#include "osdo.h"

class GlBindable;

/**
 * @brief Клас який прив'язує контексту до деякого об'єкту OpenGL.
 */
class GlBinder
{
    /**
     * @brief Об'єкт OpenGL, який прив'язують.
     */
    const GlBindable& bindable;
    /**
     * @brief Ціль прив'язки.
     */
    const GLenum target;
public:
    /**
     * @brief Конструктор із параметром
     * @param bindable об'єкт OpenGL, який прив'язують
     * @param target ціль прив'язки
     */
    GlBinder(const GlBindable& bindable, GLenum target);
    ~GlBinder();
};

#endif // GLBINDER_H
