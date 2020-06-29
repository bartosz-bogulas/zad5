#ifndef DRONE_GEOMETRY_HPP
#define DRONE_GEOMETRY_HPP

#include <iostream>
#include <cmath>

#include "../include/geometry.hpp"
#include "../include/events.hpp"

#define TO_RADIANS (M_PI / 180)
#define TO_DEGREES (180 / M_PI)

namespace drone::geometry {

    template <class T, size_t N>
    class vector;

    template <class T = float>
    using vector3d = vector<T, 3>;

}

namespace drone::events {

    template <class T>
    class basic_transform_event;

    template <class T, size_t N, template <class, size_t> class V>
    class basic_transform_event<V<T, N>> : public event {
    public:
        using scalar_type = T;
        using vector_type = V<T, N>;

    private:
        vector_type vector;
    };

    using transform3d_event = basic_transform_event<vector3d>;


    template <class T>
    class basic_rotate_event;

    template <class T, size_t N, template <class, size_t> class V>
    class basic_rotate_event<V<T, N>> : public basic_transform_event<V<T, N>> {};

    using rotate3d_event = basic_rotate_event<vector3d>;


    template <class T>
    class basic_rotate_around_event;

    template <class T, size_t N, template <class, size_t> class V>
    class basic_rotate_around_event<V<T, N>> : public basic_rotate_event<V<T, N>> {
    private:
        vector_type point;
    };

    using rotate_around3d_event = basic_rotate_around_event<vector3d>;


    template <class T>
    class basic_translate_event;

    template <class T, size_t N, template <class, size_t> class V>
    class basic_translate_event<V<T, N>> : public basic_transform_event<V<T, N>> {};

    using translate3d_event = basic_translate_event<vector3d>;


    template <class T>
    class basic_translate_forward_event;

    template <class T, size_t N, template <class, size_t> class V>
    class basic_translate_forward_event<V<T, N>> : public basic_transform_event<V<T, N>> {};

    using translate_forward3d_event = basic_translate_forward_event<vector3d>;


    REGISTER_EVENT(transform3d_event, event)

    REGISTER_EVENT(rotate3d_event, trasform3d_event)
    REGISTER_EVENT(rotate_around3d_event, rotate3d_event)

    REGISTER_EVENT(translate3d_event, transform3d_event)
    REGISTER_EVENT(translate_forward3d_event, transform_3d_event)
}

#endif //DRONE_GEOMETRY_HPP
