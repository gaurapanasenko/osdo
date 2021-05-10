/**
 * @file object.h
 * @brief Задає клас об'єкту моделі.
 */
#ifndef OBJECT_H
#define OBJECT_H

#include "osdo.h"

#include "bijective.h"
#include "model.h"
#include "shader.h"
#include "EASTL/shared_ptr.h"
using eastl::shared_ptr;
using eastl::make_shared;

/**
 * @brief Об'єкт моделі.
 */
class Object : public Bijective {
    /**
     * @brief Матриця лінійних перетворень.
     */
    mat4 transform;
    /**
     * @brief Позиція об'єкту у просторі
     */
    vec4 position;
    /**
     * @brief Анімація обертання по осям \f$(x, y, z, 1.0)\f$.
     */
    vec4 animation;
    /**
     * @brief Модель об'єкту.
     */
    shared_ptr<Model> model;

public:
    /**
     * @brief Конструктор, який створює об'єкт з моделі.
     * @param model Модель об'єкту.
     */
    Object(shared_ptr<Model> model = nullptr);
    ~Object() override = default;

    /**
     * @brief Забирає поточну позицію об'єкта у просторі.
     * @param[out] position поточна позицію об'єкта
     */
    void get_position(vec4 position) override;
    /**
     * @brief Задає нову позицію об'єкта у просторі.
     * @param[in] position нова позиція об'єкта у просторі
     */
    void set_position(vec4 position) override;

    /**
     * @brief Забирає поточний нахил об'єкта.
     * @param[out] rotation поточний нахил  об'єкта
     */
    void get_rotation(vec3 rotation) override;
    /**
     * @brief Задає новий нахил об'єкта.
     * @param[in] rotation новий нахил об'єкта
     */
    void set_rotation(vec3 rotation) override;

    /**
     * @brief Забирає поточну анімацію обернення об'єкта.
     * @param[out] rotation поточна анімація обернення об'єкта
     */
    void get_animation(vec3 rotation) override;
    /**
     * @brief Задає нову анімацію обернення об'єкта.
     * @param[in] rotation нова анімація обернення об'єкта.
     */
    void set_animation(vec3 rotation) override;

    /**
     * @brief Забирає матрицю лінійних перетворень над об'єктом.
     * @param[out] matrix матриця лінійних перетворень
     */
    void get_mat4(mat4 matrix) override;

    /**
     * @brief Переміщує об'єкт у просторі.
     *
     * Переміщує об'єкт у просторі на відстані з аргументу `distances`,
     * де кожне значення вектору позначає відстань відповідної осі.
     * @param[in] distances відстані переміщення по осям
     * @param[in] delta_time скільки часу пройшло з останнього кадру
     */
    void translate(vec3 distances, float delta_time) override;
    /**
     * @brief Обертає об'єкт.
     * @param[in] coord позначає координатну вісь навколо якої обертати
     * @param[in] delta_time скільки часу пройшло з останнього кадру
     */
    void rotate(enum coord_enum coord, float delta_time) override;
    /**
     * @brief Обернути об'єкт по всім осям.
     * @param[in] angles вектор кутів у радіанах на кожну вісь
     */
    void rotate_all(vec3 angles) override;
    /**
     * @brief Додає швидкість анімації обертання об'єкту.
     * @param[in] angles вектор швидкостей анімацій обертання по трьом осям
     * @param[in] delta_time скільки часу пройшло з останнього кадру
     */
    void add_animation(vec3 angles, float delta_time) override;

    /**
     * @brief Повертає модель об'єкту.
     * @return модель об'єкту
     */
    shared_ptr<Model> get_model();

    /**
     * @brief Відображує об'єкт
     * @param shader Шейдер який використовуєтсья для відображення моделі
     * @param mat4buf буфер матриці
     * @param delta_time скільки часу пройшло з останнього кадру
     * @param pre_generated флаг, який позначає яким чином відображати модель
     */
    void draw(Shader &shader, mat4 mat4buf, GLdouble delta_time,
              bool pre_generated);

    /**
     * @brief Переміщує об'єкт незалежно від часу.
     * @param distances відстані переміщення по трьом осям \f$(x, y, z)\f$
     */
    void translate_object(vec3 distances);

    /**
     * @brief Обертає об'єкт по осі незалежно від часу.
     * @param angle кут обертання у радіанах
     * @param coord ввісь обертання
     */
    void rotate_object(float angle, enum coord_enum coord);
    /**
     * @brief Обертає об'єкт по всім осям незалежно від часу.
     * @param angles вектор кутів обертання по трьом осям \f$(x, y, z)\f$
     */
    void rotate_all_object(vec3 angles);

    /**
     * @brief Обертає об'єкт по заданій анімації.
     * @param step шаг анімації
     */
    void animate(float step);

    /**
     * @brief Збільщує або зменшує об'єкт по трьом осям.
     * @param scale коеффіціенти зміни розміру по осям \f$(x, y, z)\f$
     */
    void scale(vec3 scale);
    /**
     * @brief Повертає матрицю лінійних перетворень без переміщення.
     * @return матриця лінійних перетворень
     */
    mat4* get_transform();
};

#endif // OBJECT_H
