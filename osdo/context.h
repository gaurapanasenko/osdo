/**
 * @file context.h
 * @brief Контекст, який зберігає усі завантажені у пам'ять ресурси.
 */
#ifndef CONTEXT_H
#define CONTEXT_H

#include "osdo.h"

#include "object.h"
#include "EASTL/hash_map.h"
#include "EASTL/string.h"
#include "EASTL/shared_ptr.h"
using eastl::hash_map;
using eastl::string;
using eastl::shared_ptr;
using eastl::pair;
using eastl::make_shared;

class Shader;
class Texture;

/**
 * @brief Контекст, який зберігає усі завантажені у пам'ять ресурси.
 */
struct Context
{
    /**
     * @brief Тип для зберігання моделей.
     */
    typedef hash_map<string, Object> Models;
    /**
     * @brief Тип для зберігання текстур.
     */
    typedef hash_map<string, shared_ptr<Texture>> Textures;
    /**
     * @brief Завантажені моделі.
     */
    Models models;
    /**
     * @brief Зкомпіловані шейдери.
     */
    hash_map<string, shared_ptr<Shader>> shaders;
    /**
     * @brief Завантажені текстури.
     */
    Textures textures;

    /**
     * @brief Вибрана модель для редагування.
     */
    Models::iterator active;
    /**
     * @brief Вибрана текстура для відображення.
     */
    Textures::iterator active_texture;

public:
    Context();

    /**
     * @brief перехід до наступної моделі для редагування.
     * @return ітератор моделі
     */
    Models::iterator &next_active();

    /**
     * @brief Завантажує текстуру у пам'ять.
     * @param[in] path шлях до файлу з текстурою
     */
    void load_texture(const char *path);

    /**
     * @brief Завантажує та компілює шейдер.
     * @param name назва шейдеру
     * @param shaders массив до файлів шейдеру
     * @return статус успішності завантаження та компіляції шейдеру
     */
    bool load_shader(const char *name, const Shader::shader_map& shaders);

    /**
     * @brief Завантажує модель з поверхнями Безье
     * @param path шлях до файлу з моделлю
     * @return статус успішності завантаження моделі
     */
    bool load_model(const string& path);
};

#endif // CONTEXT_H
