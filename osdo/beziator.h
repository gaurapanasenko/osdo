/**
 * @file beziator.h
 * @brief Клас який зберігає та оброблює модель утвореню через поверхні Безьє.
 */
#ifndef BEZIATOR_H
#define BEZIATOR_H

#include <EASTL/string.h>
#include "osdo.h"
#include "mesh.h"

using eastl::string;

/**
 * @brief Набір індексів на вершини, що утворюють поверхню 4x4.
 */
typedef GLuint surfacei_t[4][4];

/**
 * @brief Клас який зберігає та оброблює модель утвореню через поверхні Безьє.
 */
class Beziator : public Mesh {
public:
    /**
     * @brief Тип позначаючий вказівнкик на массив з поверхнями Безьє.
     */
    typedef surfacei_t* surfaces_vector;
protected:
    /**
     * @brief Шлях до файлу у якому зберігається модель.
     */
    const string path;
    /**
     * @brief Згенерований за допомогою CPU меш моделі.
     */
    Mesh mesh;
    //Mesh frame;
    //Mesh normals;
    /**
     * @brief Масив вершин/вузлів моделі.
     */
    vector<Vertex> vertices;
    /**
     * @brief Масив індексів, що утворюють поверхні Безье.
     *
     * Індекси розташовані у масиві по 16 элементів, які утворюють поверхню
     * з контрольними точками 4x4.
     * Масив легко інтерпретуєтсья у `surfaces_vector`:
     *
     *     surfacei_t *surfaces = reinterpret_cast<surfacei_t*>(indices.data());
     */
    vector<GLuint> indices;
public:
    /**
     * @brief Конструктор до Beziator, який зберігає шлях до файлу з моделлю.
     *
     * Обов'язково потібно запустити метод `Beziator::init` для того щоб
     * завантажити модель у пам'ять.
     * @param path Шлях до файлу у якому зберігається модель.
     */
    Beziator(const string& path);
    ~Beziator() override;

    /**
     * @brief Завантажує модель у пам'ять.
     * @return Статус, чи успішно була завантажена модель.
     */
    bool init();

    /**
     * @brief Відображує модель.
     *
     * За допомогою флагу `pre_generated` можна задати яким чином потібно
     * відображати, якщо задати `false`, то у буде використаний меш із
     * поверхнями Безье 4x4, а якщо задано `true`,
     * то відобразиться сгенерований деталізований меш моделі.
     * @param shader Шейдер який використовуєтсья для відображення моделі.
     * @param pre_generated Флаг, який позначає який з мешів відображати.
     */
    void draw(Shader &shader, bool pre_generated) override;

    /**
     * @brief Генерує деталізований меш моделі.
     *
     * Ступінь деталізаії `d` позначає скільки вершин буде створено по двом
     * осям, за заммовчанням задано 8, таким чином поверхня буде складатися з
     * 8x8=64 вершини.
     * @param d ступінь деталізації.
     */
    void generate(size_t d = 8) override;

    /**
     * @brief Зберігає модель у файл, вказаний у полі `path`.
     * @return Статус зберігання файлу.
     */
    bool save();

    /**
     * @brief Інвертує порядок індесів поверхні, щоб нормалі дивилися у протилежний бік.
     * @param i номер поверхні.
     */
    void rotate(size_t i);

    /**
     * @brief Видає список вершин моделі.
     * @return Вказівник на поле `vertices`.
     */
    vector<Vertex> *get_vertices() override;
};

#endif // BEZIATOR_H
