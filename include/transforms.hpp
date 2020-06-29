#ifndef DRONE_TRANSFORMS_HPP
#define DRONE_TRANSFORMS_HPP

#include <iostream>

#include "../include/geometry.hpp"

namespace drone::transforms {

    template <class T>
    class basic_transform;

    template <class T, size_t N, template <class, size_t> class V>
    class basic_transform<V<T, N>> {
    public:
        using scalar_type = T;
        using vector_type = V<T, N>;

        void translate_absolute(const vector_type& translation);
        void translate_relative(vector_type translation, const vector_type& rotation);
        void translate_relative(vector_type translation);

        void rotate_absolute(const vector_type& rotation);
        void rotate_absolute(const vector_type& rotation, const vector_type& point);
        void rotate_relative(const vector_type& rotation, const vector_type& point);

    protected:
        vector_type _translation;
        vector_type _rotation;
    };

    template <class T, size_t N, template <class, size_t> class V>
    void basic_transform<V<T, N>>::translate_absolute(const vector_type& translation) {
        _translation += translation;
    }

    template <class T, size_t N, template <class, size_t> class V>
    void basic_transform<V<T, N>>::translate_relative(vector_type translation, const vector_type& rotation) {
        translation.rotate_degrees(rotation);
        translate_absolute(translation);
    }

    template <class T, size_t N, template <class, size_t> class V>
    void basic_transform<V<T, N>>::translate_relative(vector_type translation) {
        translate_relative(std::move(translation), _rotation);
    }

    template <class T, size_t N, template <class, size_t> class V>
    void basic_transform<V<T, N>>::rotate_absolute(const vector_type& rotation) {
        _rotation += rotation;
    }

    template <class T, size_t N, template <class, size_t> class V>
    void basic_transform<V<T, N>>::rotate_absolute(const vector_type& rotation, const vector_type& point) {
        _translation.rotate_degrees(rotation, point - _translation);
    }

    template <class T, size_t N, template <class, size_t> class V>
    void basic_transform<V<T, N>>::rotate_relative(const vector_type& rotation, const vector_type& point) {
        rotate_absolute(rotation, point);
        rotate_absolute(rotation);
    }

    template <class T>
    using transform3d = basic_transform<geometry::vector3d<T>>;
}

#endif //DRONE_TRANSFORMS_HPP
