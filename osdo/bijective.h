/**
 * @file bijective.h
 * @brief Інтерфейс до об'єктів, що можуть можуть бути переміщені та повернуті у просторі.
 */
#ifndef BIJECTIVE_H
#define BIJECTIVE_H

#include "osdo.h"

/**
 * @brief Інтерфейс до об'єктів, що можуть можуть бути переміщені та повернуті у просторі.
 */
class Bijective {
public:
    virtual ~Bijective() {}

    /**
     * @brief Забирає поточну позицію об'єкта у просторі.
     * @param[out] position поточна позицію об'єкта
     */
    virtual void get_position(vec4 position) {}
    /**
     * @brief Задає нову позицію об'єкта у просторі.
     * @param[in] position нова позиція об'єкта у просторі
     */
    virtual void set_position(vec4 position) {}

    /**
     * @brief Забирає поточний нахил об'єкта.
     * @param[out] rotation поточний нахил  об'єкта
     */
    virtual void get_rotation(vec3 rotation) {}
    /**
     * @brief Задає новий нахил об'єкта.
     * @param[in] rotation новий нахил об'єкта
     */
    virtual void set_rotation(vec3 rotation) {}

    /**
     * @brief Забирає поточну анімацію обернення об'єкта.
     * @param[out] rotation поточна анімація обернення об'єкта
     */
    virtual void get_animation(vec3 rotation) {}
    /**
     * @brief Задає нову анімацію обернення об'єкта.
     * @param[in] rotation нова анімація обернення об'єкта.
     */
    virtual void set_animation(vec3 rotation) {}

    /**
     * @brief Забирає матрицю лінійних перетворень над об'єктом.
     * @param[out] matrix матриця лінійних перетворень
     */
    virtual void get_mat4(mat4 matrix) {}

    /**
     * @brief Переміщує об'єкт у просторі.
     *
     * Переміщує об'єкт у просторі на відстані з аргументу `distances`,
     * де кожне значення вектору позначає відстань відповідної осі.
     * @param[in] distances відстані переміщення по осям
     * @param[in] delta_time скільки часу пройшло з останнього кадру
     */
    virtual void translate(vec3 distances, float delta_time) {}
    /**
     * @brief Обертає об'єкт.
     * @param[in] coord позначає координатну вісь навколо якої обертати
     * @param[in] delta_time скільки часу пройшло з останнього кадру
     */
    virtual void rotate(enum coord_enum coord, float delta_time) {}
    /**
     * @brief Обернути об'єкт по всім осям.
     * @param[in] angles вектор кутів у радіанах на кожну вісь
     */
    virtual void rotate_all(vec3 angles) {}
    /**
     * @brief Додає швидкість анімації обертання об'єкту.
     * @param[in] angles вектор швидкостей анімацій обертання по трьом осям
     * @param[in] delta_time скільки часу пройшло з останнього кадру
     */
    virtual void add_animation(vec3 angles, float delta_time) {}
};

#endif // BIJECTIVE_H
