/**
 * @file shader.h
 * @brief Задає клас шейдеру.
 */
#ifndef SHADER_H
#define SHADER_H

#include "osdo.h"
#include "glbindable.h"
#include "EASTL/string.h"
#include "EASTL/hash_map.h"
#include "EASTL/shared_ptr.h"
using eastl::string;
using eastl::hash_map;
using eastl::shared_ptr;

/**
 * @brief Тип шейдеру
 */
enum ShaderType {
    VERT_SHADER, /**< вершинний шейдер */
    TESC_SHADER, /**< теселяційний контрольний шейдер */
    TESE_SHADER, /**< теселяційний обчислювальний шейдер */
    GEOM_SHADER, /**< геометричний шейдер */
    FRAG_SHADER, /**< фрагмнтний шейдер */
};

/**
 * @brief Клас взаємодії з шейдером у видеокарті.
 */
class Shader : public GlBindable {
    /**
     * @brief Віртуальний метод, що прив'язує контекст OpenGL до шейдеру.
     * @param id індекс шейдеру OpenGL
     * @param target не використовується, реалізован для інтерфейсу `GlBindable::_bind`
     */
    virtual void _bind(const GLuint id, GLenum target) const override;
public:
    /**
     * @brief тип для зберігання шляхів файлів шейдеру по їх типам.
     */
    typedef hash_map<ShaderType, string> shader_map;
    Shader(const GLuint shader);
    ~Shader() override;

    /**
     * @brief Створює об'єкт шейдеру за заданими шляхами файлів шейдерів.
     * @param shaders_paths шляхи файлів шейдеру по їх типам
     * @return розумний вказівник на об'єкт шейдеру
     */
    static shared_ptr<Shader> create(const shader_map& shaders_paths);

    /**
     * @brief Задати поле шейдеру типу `bool`
     * @param name ім'я поля
     * @param value значення поля
     */
    void set_bool (const char* name, bool value);
    /**
     * @brief Задати поле шейдеру типу `int`
     * @param name ім'я поля
     * @param value значення поля
     */
    void set_int  (const char* name, int value);
    /**
     * @brief Задати поле шейдеру типу `float`
     * @param name ім'я поля
     * @param value значення поля
     */
    void set_float(const char* name, float value);
    /**
     * @brief Задати поле шейдеру типу `vec2`
     * @param name ім'я поля
     * @param value значення поля
     */
    void set_vec2 (const char* name, vec2 value);
    /**
     * @brief Задати вектор-поле шейдеру типу `vec2`
     * @param name ім'я поля
     * @param x значення першого элементу вектор-поля
     * @param y значення другого элементу вектор-поля
     */
    void set_vec2f(const char* name, float x, float y);
    /**
     * @brief Задати поле шейдеру типу `vec3`
     * @param name ім'я поля
     * @param value значення поля
     */
    void set_vec3 (const char* name, vec3 value);
    /**
     * @brief Задати вектор-поле шейдеру типу `vec3`
     * @param name ім'я поля
     * @param x значення першого элементу вектор-поля
     * @param y значення другого элементу вектор-поля
     * @param z значення третього элементу вектор-поля
     */
    void set_vec3f(const char* name, float x, float y, float z);
    /**
     * @brief Задати поле шейдеру типу `vec4`
     * @param name ім'я поля
     * @param value значення поля
     */
    void set_vec4 (const char* name, vec4 value);
    /**
     * @brief Задати вектор-поле шейдеру типу `vec4`
     * @param name ім'я поля
     * @param x значення першого элементу вектор-поля
     * @param y значення другого элементу вектор-поля
     * @param z значення третього элементу вектор-поля
     * @param w значення четвертого элементу вектор-поля
     */
    void set_vec4f(const char* name, float x, float y, float z, float w);
    /**
     * @brief Задати поле шейдеру типу `mat2`
     * @param name ім'я поля
     * @param mat значення поля
     */
    void set_mat2 (const char* name, mat2 mat);
    /**
     * @brief Задати поле шейдеру типу `mat3`
     * @param name ім'я поля
     * @param mat значення поля
     */
    void set_mat3 (const char* name, mat3 mat);
    /**
     * @brief Задати поле шейдеру типу `mat4`
     * @param name ім'я поля
     * @param mat значення поля
     */
    void set_mat4 (const char* name, mat4 mat);
};

#endif // SHADER_H
