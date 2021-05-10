/**
 * @file vertex.h
 * @brief Задає cтруктуру вершини.
 */
#ifndef VERTEX_H
#define VERTEX_H
#include "osdo.h"

/**
 * @brief Структура вершини, для передачі у відеокарту.
 */
struct Vertex {
    /**
     * @brief Позиція вершини у просторі.
     */
    vec4 position;
    /**
     * @brief Нормаль вершини.
     */
    vec3 normal;
    /**
     * @brief Колір вершини.
     */
    unsigned char color[4];
    /**
     * @brief Координати вершини на текстурі.
     */
    vec2 uv;
};

#endif // VERTEX_H
