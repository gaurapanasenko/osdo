/**
 * @file model.h
 * @brief Задає інтерфейс моделі, яку можна відобразити.
 */
#ifndef MODEL_H
#define MODEL_H

#include <EASTL/vector.h>
#include "osdo.h"

struct Vertex;
class Shader;
using eastl::vector;

/**
 * @brief Інтерфейс до деякої моделі, яку можна відобразити.
 */
class Model {
public:
    virtual ~Model();
    /**
     * @brief Відображує модель.
     * @param shader Шейдер який використовуєтсья для відображення моделі.
     * @param pre_generated флаг, який позначає яким чином відображати модель.
     */
    virtual void draw(Shader &shader, bool pre_generated = false);
    /**
     * @brief Генерує деталізований меш моделі.
     * Див. `Beziator::generate`
     * @param d ступінь деталізації.
     */
    virtual void generate(size_t d = 8);
    /**
     * @brief Видає список вершин моделі.
     * @return Вказівник на поле `vertices`.
     */
    virtual vector<Vertex> *get_vertices();
    /**
     * @brief Створює вікно редагування моделі.
     */
    virtual void edit_panel();
};

#endif // MODEL_H
