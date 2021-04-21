#ifndef EASYVECTOR_H
#define EASYVECTOR_H

#include <cstring>
#include <cstdlib>

namespace DE {
    template<class T>
    class vector {
        T * arr;
        size_t _size;
        void _allocate(size_t size) {
            _size = 0;
            arr = nullptr;
            if (size)
                arr = static_cast<T*>(calloc(size, sizeof(T)));
            if (arr)
                _size = size;
        }
        void _free() {
            if (arr)
                free(arr);
            _size = 0;
            arr = nullptr;
        }
        void _copy(const vector& vector) {
            _allocate(vector._size);
            if (_size)
                memcpy(arr, vector.arr, _size * sizeof(T));
        }
        void _move(vector& vector) {
            arr = vector.arr;
            _size = vector._size;
        }
    public:
        vector(size_t size = 0) {
            _allocate(size);
        }
        vector(vector&& vector) {
            _copy(vector);
        }
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
