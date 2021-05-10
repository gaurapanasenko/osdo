/**
 * @file camera.h
 * @brief Клас камери, якою можна маніпулювати у сцені.
 */
#ifndef CAMERA_H
#define CAMERA_H

#include "osdo.h"
#include "bijective.h"

/**
 * @brief Вектор, який використовуэться для обчислення напрямку перегляду камери.
 */
#define CAMERA_DIRECTION_INIT {0.0f, 0.0f, -1.0f, 0.0f}

/**
 * @brief Клас камери, якою можна маніпулювати у сцені.
 */
class Camera : public Bijective {
    /**
     * @brief Матриця лінійних перетворень у якій зберігається обернення камери.
     */
    mat4 rotation;
    /**
     * @brief Вектор позіції камери \f$(x, y, z, 1.0)\f$
     */
    vec4 position;
    /**
     * @brief Вектор, який реалізования для інтерфейсу `Bijective`
     */
    vec4 animation;
public:
    Camera();

    /**
     * @brief Забирає поточну позицію камери у просторі.
     * @param[out] position поточна позицію камери
     */
    void get_position(vec4 position) override;
    /**
     * @brief Задає нову позицію камери у просторі.
     * @param[in] position нова позиція камери у просторі
     */
    void set_position(vec4 position) override;

    /**
     * @brief Забирає поточний нахил камери.
     * @param[out] rotation поточний нахил камери
     */
    void get_rotation(vec3 rotation) override;
    /**
     * @brief Задає новий нахил камери.
     * @param[in] rotation новий нахил камери
     */
    void set_rotation(vec3 rotation) override;

    /**
     * @brief Метод реалізований для інтерфейсу `Bijective`
     */
    void get_animation(vec3 animation) override;
    /**
     * @brief Метод реалізований для інтерфейсу `Bijective`
     */
    void set_animation(vec3 animation) override;

    /**
     * @brief Забирає матрицю лінійних перетворень над камерою.
     * @param[out] matrix матриця лінійних перетворень
     */
    void get_mat4(mat4 matrix) override;

    /**
     * @brief Переміщує камеру у просторі.
     * @param[in] distances відстані переміщення по осям
     * @param[in] delta_time скільки часу пройшло з останнього кадру
     */
    void translate(vec3 distances, float delta_time) override;
    /**
     * @brief Обертає камеру.
     * @param[in] coord позначає координатну вісь навколо якої обертати
     * @param[in] delta_time скільки часу пройшло з останнього кадру
     */
    void rotate(enum coord_enum coord, float delta_time) override;
    /**
     * @brief Обернути камеру по всім осям.
     * @param[in] angles вектор кутів у радіанах на кожну вісь
     */
    void rotate_all(vec3 angles) override;
    /**
     * @brief Метод реалізований для інтерфейсу `Bijective`
     */
    void add_animation(vec3 angles, float delta_time) override;

    /**
     * @brief Обчислює напрямок перегляду камери.
     * @param[out] dest напрямок перегляду камери
     */
    void get_direction(vec4 dest);
    /**
     * @brief Забирає матрицю обертання камери.
     * @param[out] dest матриця обертання камери.
     */
    void get_rotation_mat4(mat4 dest);
    /**
     * @brief Забирає інвертовану матрицю обертання камери.
     * @param[out] dest інвертована матриця обертання камери.
     */
    void get_rotation_inv_mat4(mat4 dest);

    /**
     * @brief Переміщує камеру незалежно від часу.
     * @param distances відстані переміщення по трьом осям \f$(x, y, z)\f$
     */
    void translate_camera(vec3 distances);

    /**
     * @brief Обертає камеру по осі незалежно від часу.
     * @param angle кут обертання у радіанах
     * @param coord ввісь обертання
     */
    void rotate_camera(float angle, enum coord_enum coord);
    /**
     * @brief Обертає камеру по всім осям незалежно від часу.
     * @param angles вектор кутів обертання по трьом осям \f$(x, y, z)\f$
     */
    void rotate_all_camera(vec3 angles);

    /**
     * @brief Обертає камеру по орбіті навколо центру коодинат.
     * @param angles вектор кутів у радіанах обертання по трьом осям \f$(x, y, z)\f$
     */
    void rotate_all_inverse(vec3 angles);

};

#endif // CAMERA_H
