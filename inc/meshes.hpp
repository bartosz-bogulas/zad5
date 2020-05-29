#ifndef ZAD5_MESHES_HPP
#define ZAD5_MESHES_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

#include "../inc/geometry.hpp"

template <class T>
class Mesh3D {
public:
    Mesh3D() = delete;

    Mesh3D(const std::initializer_list<Vector3D<T>>& absolute_vertexes);
    explicit Mesh3D(const std::vector<Vector3D<T>>& absolute_vertexes);

    void translate_absolute(const Vector3D<T>& translation);

    void translate_relative(Vector3D<T> translation, const Vector3D<T>& rotation); // intentional copy
    void translate_relative(const Vector3D<T>& translation);

    void rotate(const Vector3D<T>& rotation);

//protected:
    Vector3D<T> absolute_origin;
    Vector3D<T> relative_origin;

    std::vector<Vector3D<T>> absolute_vertexes;
    std::vector<Vector3D<T>> relative_vertexes;

    Vector3D<T> relative_rotation;
    Vector3D<T> relative_translation;
};

template <class T>
Mesh3D<T>::Mesh3D(const std::vector<Vector3D<T>>& absolute_vertexes)
    : absolute_vertexes(absolute_vertexes), relative_vertexes(absolute_vertexes),
      relative_rotation(Vector3D<T>::zeros()),
      relative_translation(Vector3D<T>::zeros()),
      absolute_origin(Vector3D<T>::zeros())
{
    for (auto& vertex : absolute_vertexes)
        absolute_origin += vertex;

    absolute_origin /= absolute_vertexes.size();
    relative_origin = absolute_origin;
}

template <class T>
Mesh3D<T>::Mesh3D(const std::initializer_list<Vector3D<T>>& absolute_vertexes)
    : Mesh3D(std::vector<Vector3D<T>>(absolute_vertexes)) {}

template <class T>
void Mesh3D<T>::translate_absolute(const Vector3D<T>& translation) {
    relative_translation += translation;

    for (auto& vertex : relative_vertexes)
        vertex += translation;

    relative_origin += translation;
}

template <class T>
void Mesh3D<T>::translate_relative(Vector3D<T> translation, const Vector3D<T>& rotation) {
    translation.rotate_degrees(rotation);
    translate_absolute(translation);
}

template <class T>
void Mesh3D<T>::translate_relative(const Vector3D<T>& translation) {
    translate_relative(translation, relative_rotation);
}

template <class T>
void Mesh3D<T>::rotate(const Vector3D<T>& rotation) {
    relative_rotation += rotation;
    relative_vertexes = absolute_vertexes;
    relative_origin = absolute_origin;

    for (auto& vertex : relative_vertexes)
        vertex += relative_translation;

    relative_origin += relative_translation;

    for (auto& vertex : relative_vertexes)
        vertex.rotate_degrees(relative_origin, relative_rotation);
}

// TODO move magic numbers to static
// TODO use .at() instead of [], consider

template <class T>
class GNUMesh3D : public Mesh3D<T> {
public:
    GNUMesh3D() = delete;
    GNUMesh3D(std::vector<Vector3D<T>>& absolute_vertexes, std::vector<std::vector<size_t>>& vertex_order);

    static GNUMesh3D<T> read(const std::vector<std::vector<Vector3D<T>>>& polygons);
    static GNUMesh3D<T> read(std::istream& in);
    static GNUMesh3D<T> read(const std::string& path);

    void write(std::ostream& out);
    void write(const std::string& path);

//private:
    std::vector<std::vector<size_t>> vertex_order;
};

template <class T>
GNUMesh3D<T>::GNUMesh3D(std::vector<Vector3D<T>>& absolute_vertexes, std::vector<std::vector<size_t>>& vertex_order)
    : Mesh3D<T>::Mesh3D(absolute_vertexes), vertex_order(vertex_order) {} // TODO should be like that ?

template <class T>
GNUMesh3D<T> GNUMesh3D<T>::read(const std::vector<std::vector<Vector3D<T>>>& polygons) {
    std::unordered_map<Vector3D<T>, size_t> unique_vertexes;
    std::vector<std::vector<size_t>> vertex_order(polygons.size());

    size_t i = 0;
    auto p_it = polygons.begin();
    auto o_it = vertex_order.begin();

    for (; p_it != polygons.end(); p_it++, o_it++) {
        o_it->reserve(p_it->size());

        for (auto& vertex : *p_it) { // TODO have to?
            auto v_it = unique_vertexes.find(vertex);
            size_t index;

            if (v_it != unique_vertexes.end()) {
                index = v_it->second;
            } else {
                unique_vertexes.insert_or_assign(vertex, (index = i++));
            }

            o_it->push_back(index);
        }
    }

    std::vector<Vector3D<T>> absolute_vertexes(unique_vertexes.size());

    for (auto& [vertex, index] : unique_vertexes)
        absolute_vertexes.at(index) = vertex;

    return GNUMesh3D<T>(absolute_vertexes, vertex_order);
}

template <class T>
GNUMesh3D<T> GNUMesh3D<T>::read(std::istream& in) {
    std::vector<std::vector<Vector3D<T>>> polygons;
    polygons.reserve(4);

    auto p_ptr = &polygons.emplace_back();
    p_ptr->reserve(5);

    std::string line;

    while(std::getline(in, line)) {
        if (line.empty()) {
            if (p_ptr->empty())
                continue;

            p_ptr = &polygons.emplace_back();
            p_ptr->reserve(5);

        } else {
            auto v_ptr = &p_ptr->emplace_back();

            std::stringstream ss;
            ss << line;
            ss >> *v_ptr;
        }
    }

    return GNUMesh3D<T>::read(polygons);
}

template <class T>
GNUMesh3D<T> GNUMesh3D<T>::read(const std::string& path) {
    std::ifstream ifs(path);

    // TODO if (ifs.bad())

    GNUMesh3D<T> mesh = GNUMesh3D<T>::read(ifs);
    ifs.close();

    return mesh;
}

template <class T>
void GNUMesh3D<T>::write(std::ostream& out) {
    for (size_t i = 0, x = vertex_order.size(); i < x; i++) {
        for (size_t j = 0, y = vertex_order[i].size(); j < y; j++) {
            out << this->relative_vertexes[vertex_order[i][j]]; // TODO why 'this' needed?

            if (j < y - 1)
                out << "\n";
        }

        if (i < x - 1)
            out << "\n\n";
    }
}

template <class T>
void GNUMesh3D<T>::write(const std::string& path) {
    std::ofstream ofs(path, std::ios::trunc);

    GNUMesh3D<T>::write(ofs);
    ofs.close();
}


#endif //ZAD5_MESHES_HPP
