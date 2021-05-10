/**
 * @file easyvector.h
 * @brief Вектор що не змінює свій розмір.
 */
#ifndef EASYVECTOR_H
#define EASYVECTOR_H

#include <cstring>
#include <cstdlib>

/**
 * @brief Неймспейс бібліотеки `osdo`
 */
namespace OSDO {
/**
 * @brief Вектор що не змінює свій розмір.
 */
    template<class T>
    class vector {
        /**
         * @brief Масив із елементами типу `T`
         */
        T * arr;
        /**
         * @brief Поточний розмір масиву.
         */
        size_t _size;
        /**
         * @brief Виділяє масив розміру `size`
         * @param size розмір масиву
         */
        void _allocate(size_t size) {
            _size = 0;
            arr = nullptr;
            if (size)
                arr = static_cast<T*>(calloc(size, sizeof(T)));
            if (arr)
                _size = size;
        }
        /**
         * @brief Звільнює пам'ять від масиву.
         */
        void _free() {
            if (arr)
                free(arr);
            _size = 0;
            arr = nullptr;
        }
        /**
         * @brief Копіює масив.
         * @param vector інший масив
         */
        void _copy(const vector& vector) {
            _allocate(vector._size);
            if (_size)
                memcpy(arr, vector.arr, _size * sizeof(T));
        }
        /**
         * @brief Переміщює данні з іншого масиву.
         * @param vector інший масив
         */
        void _move(vector& vector) {
            arr = vector.arr;
            _size = vector._size;
        }
    public:
        /**
         * @brief Конструктор з параметром розміру масиву
         * @param size розмір массиву
         */
        vector(size_t size = 0) {
            _allocate(size);
        }
        /**
         * @brief Конструктор переносу
         * @param vector інший масив
         */
        vector(vector&& vector) {
            _copy(vector);
        }
        /**
         * @brief Конструктор копіювання
         * @param vector інший масив
         */
        vector(const vector& vector) {
            _copy(vector);
        }
        ~vector() {_free();}
        vector &operator=(vector&& vector) {
            _free();
            _copy(vector);
            return *this;
        }
        vector &operator=(const vector& vector) {
            _free();
            _copy(vector);
            return *this;
        }
        T &operator[](size_t i) {
            return arr[i];
        }
        size_t size() const {
            return _size;
        }
        T * data() {
            return arr;
        }
        const T * data() const {
            return arr;
        }
        void clear() {
            *this = vector(0);
        }
    };
}

#endif // EASYVECTOR_H
