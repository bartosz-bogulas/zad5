#ifndef ZAD5_OBJECT_HPP
#define ZAD5_OBJECT_HPP

#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <unordered_map>
#include <fstream>
#include <sstream>

#include "../inc/geometry.hpp"

namespace object {
    using namespace geometry;

    template <class T>
    class static_object3d {
    public:
        using iterator = typename std::vector<vector3d<T>>::iterator;
        using const_iterator = typename std::vector<vector3d<T>>::const_iterator;

        static_object3d() = delete;
        static_object3d(const std::initializer_list<vector3d<T>>& vertexes);
        explicit static_object3d(const std::vector<vector3d<T>>& vertexes);

        iterator begin() noexcept;
        const_iterator begin() const noexcept;

        iterator end() noexcept;
        const_iterator end() const noexcept;

        const std::vector<vector3d<T>>& vertexes() const;
        const vector3d<T>& origin() const;

    protected:
        std::vector<vector3d<T>> relative_vertexes;
        vector3d<T> relative_origin;
    };

    template <class T>
    static_object3d<T>::static_object3d(const std::initializer_list<vector3d<T>>& vertexes)
        : static_object3d<T>::static_object3d(std::vector<vector3d<T>>(vertexes)) {}

    template <class T>
    static_object3d<T>::static_object3d(const std::vector<vector3d<T>>& vertexes)
        : relative_vertexes(vertexes),
          relative_origin(0)
    {
        for (auto& vertex : vertexes)
            relative_origin += vertex;

        relative_origin /= vertexes.size();
    }

    template <class T>
    typename static_object3d<T>::iterator static_object3d<T>::begin() noexcept {
        return relative_vertexes.begin();
    }

    template <class T>
    typename static_object3d<T>::const_iterator static_object3d<T>::begin() const noexcept {
        return relative_vertexes.begin();
    }

    template <class T>
    typename static_object3d<T>::iterator static_object3d<T>::end() noexcept {
        return relative_vertexes.end();
    }

    template <class T>
    typename static_object3d<T>::const_iterator static_object3d<T>::end() const noexcept {
        return relative_vertexes.end();
    }

    template <class T>
    const std::vector<vector3d<T>>& static_object3d<T>::vertexes() const {
        return relative_vertexes;
    }

    template <class T>
    const vector3d<T>& static_object3d<T>::origin() const {
        return relative_origin;
    }

    template <class T>
    class aabb3d {
    public:
        explicit aabb3d(const static_object3d<T>& object) noexcept;

        void update_bounds(const std::vector<vector3d<T>>& vertexes);
        void update_bounds(const static_object3d<T>& object);

        bool check_bounds(const aabb3d<T>& box) const;

    protected:
        std::vector<std::pair<T, T>> bounds;
    };

    template <class T>
    aabb3d<T>::aabb3d(const static_object3d<T>& object) noexcept
            : bounds(3)
    {
        update_bounds(object);
    }

    template <class T>
    void aabb3d<T>::update_bounds(const std::vector<vector3d<T>>& vertexes) {
        static constexpr T absolute_min = std::numeric_limits<T>::min(), absolute_max = std::numeric_limits<T>::max();

        for (size_t i = 0; i < bounds.size(); i++) {
            bounds[i].first  = absolute_max;
            bounds[i].second = absolute_min;
        }

        for (auto& vertex : vertexes) {
            for (size_t i = 0; i < bounds.size(); i++) {
                bounds[i].first  = std::min(bounds[i].first,  vertex[i]);
                bounds[i].second = std::max(bounds[i].second, vertex[i]);
            }
        }
    }

    template <class T>
    void aabb3d<T>::update_bounds(const static_object3d<T>& object) {
        update_bounds(object.vertexes());
    }

    template <class T>
    bool aabb3d<T>::check_bounds(const aabb3d<T>& box) const {
        for (size_t i = 0; i < bounds.size(); i++)
            if (bounds[i].first  <= box.bounds[i].second &&
                bounds[i].second >= box.bounds[i].first)
                return false;
        return true;
    }

    template <class T>
    class step3d : public basic_vector<T, 6, step3d<T>> {
    public:
        step3d() = default;
        step3d(T x_translation, T y_translation, T z_translation,
               T x_rotation, T y_rotation, T z_rotation) noexcept;
        explicit step3d(const T (&scalars)[6]) noexcept;
        explicit step3d(const vector3d<T>& translation, const vector3d<T>& rotation) noexcept;

        vector3d<T> translation() const;
        vector3d<T> rotation() const;
    };

    template <class T>
    step3d<T>::step3d(T x_translation, T y_translation, T z_translation,
                      T x_rotation, T y_rotation, T z_rotation) noexcept
        : step3d<T>::step3d({x_translation, y_translation, z_translation, x_rotation, y_rotation, z_rotation}) {}

    template <class T>
    step3d<T>::step3d(const T (&scalars)[6]) noexcept
            : basic_vector<T, 6, step3d<T>>::basic_vector(scalars) {}

    template <class T>
    step3d<T>::step3d(const vector3d<T>& translation, const vector3d<T>& rotation) noexcept
            : step3d<T>::step3d({translation[0], translation[1], translation[2], rotation[0], rotation[1], rotation[2]}) {}

    template <class T>
    vector3d<T> step3d<T>::translation() const {
        return vector3d<T>(this->scalars[0], this->scalars[1], this->scalars[2]);
    }

    template <class T>
    vector3d<T> step3d<T>::rotation() const {
        return vector3d<T>(this->scalars[3], this->scalars[4], this->scalars[5]);
    }

    template <class T>
    using substep_constructor3d = std::function<step3d<T>(size_t)>;

    template <class T>
    static substep_constructor3d<T> linear_substep_constructor(const step3d<T>& step, size_t step_count) {
        return [=](size_t) { return step / step_count; };
    }

    typedef enum : unsigned char {
        START, NONE, STOP
    } stage;

    template <class T>
    class sequence3d {
    public:
        sequence3d() = delete;
        explicit sequence3d(const step3d<T>& step, size_t step_count,
                            const substep_constructor3d<T>& next_constructor,
                            const substep_constructor3d<T>& previous_constructor) noexcept;
        explicit sequence3d(const step3d<T>& step, size_t step_count) noexcept;

        std::pair<stage, step3d<T>> next_substep();
        std::pair<stage, step3d<T>> previous_substep();

        const step3d<T>& next_step() const;
        const step3d<T>& previous_step() const;

    protected:
        const size_t start_substep;
        const size_t stop_substep;
        size_t current_substep;

        // TODO reconsider naming
        step3d<T> _next_step;
        step3d<T> _previous_step;

        substep_constructor3d<T> next_constructor;
        substep_constructor3d<T> previous_constructor;
    };

    // TODO reconsider reusing constructor
    template <class T>
    sequence3d<T>::sequence3d(const step3d<T>& step, size_t step_count,
                              const substep_constructor3d<T>& next_constructor,
                              const substep_constructor3d<T>& previous_constructor) noexcept
            : start_substep(1), stop_substep(step_count), current_substep(start_substep),
              _next_step(step), _previous_step(step * (-1)),
              next_constructor(next_constructor), previous_constructor(previous_constructor) {}

    template <class T>
    sequence3d<T>::sequence3d(const step3d<T>& step, size_t step_count) noexcept
            :start_substep(1), stop_substep(step_count), current_substep(start_substep),
              _next_step(step), _previous_step(step * (-1)),
              next_constructor(linear_substep_constructor(_next_step, step_count)),
              previous_constructor(linear_substep_constructor(_previous_step, step_count)) {}

    template <class T>
    std::pair<stage, step3d<T>> sequence3d<T>::next_substep() {
        if (current_substep >= stop_substep)
            return std::pair(STOP, _next_step);

        return std::pair(current_substep <= start_substep ? START : NONE, next_constructor(current_substep++));
    }

    template <class T>
    std::pair<stage, step3d<T>> sequence3d<T>::previous_substep() {
        if (current_substep <= start_substep)
            return std::pair(STOP, _previous_step);

        return std::pair(current_substep >= stop_substep ? START : NONE, previous_constructor(--current_substep));
    }

    template <class T>
    const step3d<T>& sequence3d<T>::next_step() const {
        return _next_step;
    }

    template <class T>
    const step3d<T>& sequence3d<T>::previous_step() const {
        return _previous_step;
    }

    template <class T>
    class dynamic_object3d : public static_object3d<T> {
    public:
        dynamic_object3d() = delete;
        dynamic_object3d(const std::initializer_list<vector3d<T>>& vertexes);
        explicit dynamic_object3d(const std::vector<vector3d<T>>& vertexes);

        void translate_absolute(const vector3d<T>& translation);

        void translate_relative(vector3d<T> translation, const vector3d<T>& rotation); // intentional copy
        void translate_relative(const vector3d<T>& translation);

        void rotate(const vector3d<T>& rotation, const vector3d<T>& point);
        void rotate(const vector3d<T>& rotation);

        void step(const step3d<T>& step);
        void update();

        const vector3d<T>& translation() const;
        const vector3d<T>& rotation() const;

    protected:
        std::vector<vector3d<T>> absolute_vertexes;
        vector3d<T> absolute_origin;

        vector3d<T> relative_translation;
        vector3d<T> relative_rotation;
    };

    template <class T>
    dynamic_object3d<T>::dynamic_object3d(const std::initializer_list<vector3d<T>>& vertexes)
        : dynamic_object3d<T>::dynamic_object3d(std::vector<vector3d<T>>(vertexes)) {}

    template <class T>
    dynamic_object3d<T>::dynamic_object3d(const std::vector<vector3d<T>>& vertexes)
         : static_object3d<T>::static_object3d(vertexes),
           absolute_vertexes(this->relative_vertexes),
           absolute_origin(this->relative_origin),
           relative_translation(0),
           relative_rotation(0) {}

    template <class T>
    void dynamic_object3d<T>::translate_absolute(const vector3d<T>& translation) {
        relative_translation += translation;

        for (auto& vertex : this->relative_vertexes)
            vertex += translation;

        this->relative_origin += translation;
    }

    template <class T>
    void dynamic_object3d<T>::translate_relative(vector3d<T> translation, const vector3d<T>& rotation) {
        translation.rotate_degrees(rotation);
        translate_absolute(translation);
    }

    template <class T>
    void dynamic_object3d<T>::translate_relative(const vector3d<T>& translation) {
        translate_relative(translation, relative_rotation);
    }

    template <class T>
    void dynamic_object3d<T>::rotate(const vector3d<T>& rotation) {
        relative_rotation += rotation;
        update();
    }

    template <class T>
    void dynamic_object3d<T>::rotate(const vector3d<T>& rotation, const vector3d<T>& point) {
        vector3d<T> new_origin = this->relative_origin;
        new_origin.rotate_degrees(point, rotation);

        vector3d<T> translation = new_origin - this->relative_origin;
        relative_translation += translation;

        rotate(rotation);
    }

    template <class T>
    void dynamic_object3d<T>::step(const object::step3d<T>& step) {
        relative_translation += step.translation();
        relative_rotation += step.rotation();
        update();
    }

    template <class T>
    void dynamic_object3d<T>::update() {
        this->relative_vertexes = absolute_vertexes;
        this->relative_origin = absolute_origin;

        for (auto& vertex : this->relative_vertexes)
            vertex += relative_translation;

        this->relative_origin += relative_translation;

        matrix3d<T> rotation = matrix3d<T>::rotation_degrees(relative_rotation);

        for (auto& vertex : this->relative_vertexes)
            vertex.rotate(this->relative_origin, rotation);
    }

    template <class T>
    const vector3d<T>& dynamic_object3d<T>::translation() const {
        return relative_translation;
    }

    template <class T>
    const vector3d<T>& dynamic_object3d<T>::rotation() const {
        return relative_rotation;
    }

    template <class T>
    class object3d : public dynamic_object3d<T> {
    public:
        object3d() = delete;
        object3d(const std::initializer_list<vector3d<T>>& vertexes);
        explicit object3d(const std::vector<vector3d<T>>& vertexes);

        void next_sequence(const sequence3d<T>& sequence);

        bool next_substep();
        bool previous_substep();

    protected:
        vector3d<T> partial_relative_translation;
        vector3d<T> partial_relative_rotation;

        // TODO consider sequence holder thing and make_unique thing, or could be just one list with pointer
        std::list<sequence3d<T>> next_sequences;
        std::list<sequence3d<T>> previous_sequences;

        std::unique_ptr<sequence3d<T>> current_sequence;
    };

    template <class T>
    object3d<T>::object3d(const std::initializer_list<vector3d<T>>& vertexes)
        : object3d<T>::object3d(std::vector<vector3d<T>>(vertexes)) {}

    template <class T>
    object3d<T>::object3d(const std::vector<vector3d<T>>& vertexes)
        : dynamic_object3d<T>::dynamic_object3d(vertexes),
          partial_relative_translation(this->relative_translation),
          partial_relative_rotation(this->relative_rotation),
          current_sequence(nullptr) {}

    template <class T>
    void object3d<T>::next_sequence(const object::sequence3d<T>& sequence) {
        next_sequences.push_back(sequence);
    }

    template <class T>
    bool object3d<T>::next_substep() {
        if (current_sequence == nullptr) {
            if (next_sequences.empty())
                return false;

            current_sequence = std::make_unique<sequence3d<T>>(next_sequences.front());
            next_sequences.pop_front();
        }

        auto [stage, substep] = current_sequence->next_substep();

        if (stage == STOP) {
            step3d<T> step = current_sequence->next_step();
            this->relative_translation = partial_relative_translation + step.translation();
            this->relative_rotation = partial_relative_rotation + step.rotation();
            this->update();

            partial_relative_translation = this->relative_translation;
            partial_relative_rotation = this->relative_rotation;

            previous_sequences.push_back(*current_sequence);
            current_sequence = nullptr;

            return !next_sequences.empty();
        }

        this->step(substep);
        return true;
    }

    template <class T>
    bool object3d<T>::previous_substep() {
        if (current_sequence == nullptr) {
            if (previous_sequences.empty())
                return false;

            current_sequence = std::make_unique<sequence3d<T>>(previous_sequences.back());
            previous_sequences.pop_back();
        }

        auto [stage, substep] = current_sequence->previous_substep();

        if (stage == STOP) {
            this->relative_translation = partial_relative_translation;
            this->relative_rotation = partial_relative_rotation;
            this->update();

            next_sequences.push_front(*current_sequence);
            current_sequence = nullptr;

            return !previous_sequences.empty();
        }

        if (stage == START) {
            step3d<T> step = current_sequence->previous_step();
            partial_relative_translation = this->relative_translation + step.translation();
            partial_relative_rotation = this->relative_rotation + step.rotation();
        }

        this->step(substep);
        return true;
    }

    template <class T>
    class basic_gnu_object3d;

    template <class T, template <class> class O>
    class basic_gnu_object3d<O<T>> : public O<T> {
    public:
        basic_gnu_object3d() = delete;

        explicit basic_gnu_object3d(const std::vector<vector3d<T>>& absolute_vertexes,
                                    const std::vector<std::vector<size_t>>& vertex_order) noexcept;

        static basic_gnu_object3d<O<T>> read(const std::vector<std::vector<vector3d<T>>>& polygons);
        static basic_gnu_object3d<O<T>> read(std::istream& in);
        static basic_gnu_object3d<O<T>> read(const std::string& path);

        void write(std::ostream& out);
        void write(const std::string& path);

    protected:
        std::vector<std::vector<size_t>> vertex_order;
    };

    template <class T, template <class> class O>
    basic_gnu_object3d<O<T>>::basic_gnu_object3d(const std::vector<geometry::vector3d<T>>& absolute_vertexes,
                                                 const std::vector<std::vector<size_t>>& vertex_order) noexcept
        : O<T>(absolute_vertexes), vertex_order(vertex_order) {}

    template <class T, template <class> class O>
    basic_gnu_object3d<O<T>> basic_gnu_object3d<O<T>>::read(const std::vector<std::vector<vector3d<T>>>& polygons) {
        std::unordered_map<vector3d<T>, size_t> unique_vertexes;
        std::vector<std::vector<size_t>> vertex_order(polygons.size());

        size_t i = 0;
        auto p_it = polygons.begin();
        auto o_it = vertex_order.begin();

        for (; p_it != polygons.end(); p_it++, o_it++) {
            o_it->reserve(p_it->size());

            for (auto& vertex : *p_it) {
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

        std::vector<vector3d<T>> absolute_vertexes(unique_vertexes.size());

        for (auto& [vertex, index] : unique_vertexes)
            absolute_vertexes.at(index) = vertex;

        return basic_gnu_object3d<O<T>>(absolute_vertexes, vertex_order);
    }

    template <class T, template <class> class O>
    basic_gnu_object3d<O<T>> basic_gnu_object3d<O<T>>::read(std::istream& in) {
        std::vector<std::vector<vector3d<T>>> polygons;
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

        return basic_gnu_object3d<O<T>>::read(polygons);
    }

    template <class T, template <class> class O>
    basic_gnu_object3d<O<T>> basic_gnu_object3d<O<T>>::read(const std::string& path) {
        std::ifstream ifs(path);
        basic_gnu_object3d<O<T>> object = basic_gnu_object3d<O<T>>::read(ifs);

        ifs.close();
        return object;
    }

    template <class T, template <class> class O>
    void basic_gnu_object3d<O<T>>::write(std::ostream& out) {
        for (size_t i = 0, x = vertex_order.size(); i < x; i++) {
            for (size_t j = 0, y = vertex_order[i].size(); j < y; j++) {
                out << this->relative_vertexes[vertex_order[i][j]];
                if (j < y - 1)
                    out << "\n";
            }
            if (i < x - 1)
                out << "\n\n";
        }
    }

    template <class T, template <class> class O>
    void basic_gnu_object3d<O<T>>::write(const std::string& path) {
        std::ofstream ofs(path, std::ios::trunc);
        basic_gnu_object3d<O<T>>::write(ofs);

        ofs.close();
    }

    template <class T>
    using gnu_static_object3d = basic_gnu_object3d<static_object3d<T>>;

    template <class T>
    using gnu_dynamic_object3d = basic_gnu_object3d<dynamic_object3d<T>>;

    template <class T>
    using gnu_object3d = basic_gnu_object3d<object3d<T>>;
}

#endif //ZAD5_OBJECT_HPP