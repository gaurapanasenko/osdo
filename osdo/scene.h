/**
 * @file scene.h
 * @brief Задає клас сцени із об'єктами.
 */
#ifndef SCENE_H
#define SCENE_H

#include "osdo.h"

#include "object.h"
#include "context.h"

/**
 * @brief Сцена із об'єктами.
 */
struct Scene {
    /**
     * @brief Об'єкти у сцені.
     */
    hash_map<string, Object> objects;

    /**
     * @brief Конструктор, що створює об'єкти у сцені за заготовленими у агрументі `objects`
     * @param objects заготовлені об'єкти для додавання у сцену
     */
    Scene(const Context::Models& objects);

    /**
     * @brief Створює сцену
     * @param objects заготовлені об'єкти для додавання у сцену
     * @return Розумний вказівник на об'єкт сцени.
     */
    static shared_ptr<Scene> create(const Context::Models& objects);
};

#endif // SCENE_H
