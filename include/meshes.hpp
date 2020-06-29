#ifndef DRONE_MESHES_HPP
#define DRONE_MESHES_HPP

#include <iostream>

#include "../include/geometry.hpp"

namespace drone::meshes {

    template <class T>
    class basic_mesh;

    template <class T, size_t N, template <class, size_t> class V>
    class basic_mesh<V<T, N>> {
    public:
        using scalar_type = T;
        using vector_type = V<scalar_type, N>;


    protected:
        std::vector<vector_type> _vertexes;
        vector_type _origin;

        // egdes, faces
    };

    template <class T>
    using mesh3d = basic_mesh<geometry::vector3d<T>>;
}

#endif //DRONE_MESHES_HPP
