/**
 * @file mesh.h
 * @brief Задає клас мешу, який зберігається на відеокарті.
 */
#ifndef MESH_H
#define MESH_H

#include "osdo.h"
#include "model.h"
#include "vertex.h"

/**
 * @brief Меш, який зберігається на відеокарті.
 */
class Mesh : public Model {
protected:
    /**
     * @brief Об’єкт масиву вершин. Розшифровується "Vertex Array Object".
     */
    GLuint vao;
    /**
     * @brief Об'єкт буфера вершин. Розшифровується "Vertex Buffer Object".
     */
    GLuint vbo;
    /**
     * @brief Об'єкти буфера елементів. Розшифровується "Element Buffer Objects".
     */
    GLuint ebo;
    /**
     * @brief Кількість індексів у `ebo`.
     */
    GLint indices_size;
public:
    Mesh();
    ~Mesh() override;

    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    /**
     * @brief Завантажити у відеокарту меш куба.
     */
    void cube_update();
    /**
     * @brief Завантажити у відеокарту новий меш.
     * @param vertices масив вершин
     * @param vertices_n кількість вершин
     * @param indices масив індексів вершин
     * @param indices_n кількість індексів вершин
     */
    void update(const Vertex* vertices, size_t vertices_n,
                const GLuint* indices, size_t indices_n);

    /**
     * @brief Відображує меш.
     * @param shader Шейдер який використовуєтсья для відображення мешу.
     * @param pre_generated флаг залишений для інтерфесу, але не використовується.
     */
    void draw(Shader &shader, bool pre_generated) override;
    /**
     * @brief Відображує меш у певному режимі.
     * Див. [glDrawElements][glDrawElements].
     * [glDrawElements]: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawElements.xhtml#parameters
     * @param mode режим відображення.
     */
    void draw_mode(GLenum mode);
};

#endif
