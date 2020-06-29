#ifndef DRONE_UTILITY_HPP
#define DRONE_UTILITY_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

#include <iterator>
#include <type_traits>

namespace drone::utility {

    template <class I>
    class flat_iterator {
    public:
        using outer_iterator = I;
        using inner_iterator = typename std::conditional<std::is_const<typename outer_iterator::value_type>::value,
                typename outer_iterator::value_type::const_iterator,
                typename outer_iterator::value_type::iterator>::type;

        using iterator_category = std::forward_iterator_tag;

        using value_type = typename inner_iterator::value_type;
        using difference_type = typename inner_iterator::difference_type;
        using pointer = typename inner_iterator::pointer;
        using reference = typename inner_iterator::reference;

        flat_iterator() = delete;
        explicit flat_iterator(outer_iterator outer_it) noexcept;
        explicit flat_iterator(outer_iterator outer_begin_it, outer_iterator outer_end_it) noexcept;

        reference operator*() const;
        pointer operator->() const;

        flat_iterator<I>& operator++();
        const flat_iterator<I> operator++(int);

        bool operator==(const flat_iterator<I>& rhs) const;
        bool operator!=(const flat_iterator<I>& rhs) const;

    private:
        void skip();

        outer_iterator outer_it;
        outer_iterator outer_end_it;

        inner_iterator inner_it;
    };

    template <class I>
    flat_iterator<I>::flat_iterator(outer_iterator outer_it) noexcept
        : outer_it(outer_it), outer_end_it(outer_it) {}

    template <class I>
    flat_iterator<I>::flat_iterator(outer_iterator outer_begin_it, outer_iterator outer_end_it) noexcept
        : outer_it(outer_begin_it), outer_end_it(outer_end_it)
    {
        inner_it = outer_it->begin();
        skip();
    }

    template <class I>
    typename flat_iterator<I>::reference flat_iterator<I>::operator*() const {
        return *inner_it;
    }

    template <class I>
    typename flat_iterator<I>::pointer flat_iterator<I>::operator->() const {
        return &*inner_it;
    }

    template <class I>
    flat_iterator<I>& flat_iterator<I>::operator++() {
        ++inner_it;
        skip();

        return *this;
    }

    template <class I>
    const flat_iterator<I> flat_iterator<I>::operator++(int) {
        flat_iterator<I> iterator(*this);
        operator++();

        return iterator;
    }

    template <class I>
    bool flat_iterator<I>::operator==(const flat_iterator<I>& rhs) const {
        if (outer_it != rhs.outer_it)
            return false;

        return outer_it == outer_end_it ||
               rhs.outer_it == rhs.outer_end_it ||
               inner_it == rhs.inner_it;
    }

    template <class I>
    bool flat_iterator<I>::operator!=(const flat_iterator<I>& rhs) const {
        return !operator==(rhs);
    }

    template <class I>
    void flat_iterator<I>::skip() {
        while (inner_it == outer_it->end() && ++outer_it != outer_end_it)
            inner_it = outer_it->begin();
    }


    template <class T, template <class, class...> class C>
    using chain_part_iterator = typename C<T>::iterator;

    template <class T, template <class, class...> class C>
    using const_chain_part_iterator = typename C<T>::const_iterator;


    template <class T, template <class, class...> class C>
    class chain_part {
    protected:
        template <class R>
        using container_template = C<R>;

        template <class R>
        using pointer_template = std::shared_ptr<R>;

    public:
        using value_type = typename container_template<T>::value_type;
        using reference = typename container_template<T>::reference;
        using const_reference = typename container_template<T>::const_reference;

        using iterator = chain_part_iterator<T, C>;
        using const_iterator = const_chain_part_iterator<T, C>;

        friend iterator;
        friend const_iterator;

        chain_part() noexcept;
        chain_part(const chain_part<T, C>& part) noexcept;
        chain_part(chain_part<T, C>&& part) noexcept;

        void push(const value_type& value);
        void push(value_type&& value);

        template <class... AS>
        void emplace(AS&&... arguments);

        iterator begin() noexcept;
        const_iterator begin() const noexcept;

        iterator end() noexcept;
        const_iterator end() const noexcept;

    private:
        pointer_template<container_template<value_type>> data_pointer;
    };

    template <class T, template <class, class...> class C>
    chain_part<T, C>::chain_part() noexcept
        : data_pointer(pointer_template<container_template<value_type>>(new container_template<value_type>())) {}

    template <class T, template <class, class...> class C>
    chain_part<T, C>::chain_part(const chain_part<T, C>& part) noexcept
        : data_pointer(part.data_pointer) {}

    template <class T, template <class, class...> class C>
    chain_part<T, C>::chain_part(chain_part<T, C>&& part) noexcept
        : data_pointer(std::move(part.data_pointer)) {}

    template <class T, template <class, class...> class C>
    void chain_part<T, C>::push(const value_type& value) {
        data_pointer->push_back(value);
    }

    template <class T, template <class, class...> class C>
    void chain_part<T, C>::push(value_type&& value) {
        data_pointer->push_back(std::move(value));
    }

    template <class T, template <class, class...> class C>
    template <class... AS>
    void chain_part<T, C>::emplace(AS&&... arguments) {
        data_pointer->emplace_back(std::forward<AS>(arguments)...);
    }

    template <class T, template <class, class...> class C>
    typename chain_part<T, C>::iterator chain_part<T, C>::begin() noexcept {
        return data_pointer->begin();
    }

    template <class T, template <class, class...> class C>
    typename chain_part<T, C>::const_iterator chain_part<T, C>::begin() const noexcept {
        return data_pointer->begin();
    }

    template <class T, template <class, class...> class C>
    typename chain_part<T, C>::iterator chain_part<T, C>::end() noexcept {
        return data_pointer->end();
    }

    template <class T, template <class, class...> class C>
    typename chain_part<T, C>::const_iterator chain_part<T, C>::end() const noexcept {
        return data_pointer->end();
    }


    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    using chain_iterator = flat_iterator<typename C1<chain_part<T, C2>>::iterator>;

    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    using const_chain_iterator = flat_iterator<typename C1<chain_part<T, C2>>::const_iterator>;


    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    class basic_chain {
    protected:
        template <class R>
        using container_template = C1<R>;

        template <class R>
        using part_template = chain_part<R, C2>;

    public:
        using value_type = typename container_template<T>::value_type;
        using reference = typename container_template<T>::reference;
        using const_reference = typename container_template<T>::const_reference;

        using iterator = chain_iterator<T, C1, C2>;
        using const_iterator = const_chain_iterator<T, C1, C2>;

        friend iterator;
        friend const_iterator;

        basic_chain() noexcept;
        basic_chain(const basic_chain<T, C1, C2>& chain) noexcept;
        basic_chain(basic_chain<T, C1, C2>&& chain) noexcept;

        void push(const value_type& value);
        void push(value_type&& value);

        template <class... AS>
        void emplace(AS&&... arguments);

        iterator begin() noexcept;
        const_iterator begin() const noexcept;

        iterator end() noexcept;
        const_iterator end() const noexcept;

        static basic_chain<T, C1, C2> join(const basic_chain<T, C1, C2>& chain);

    private:
        container_template<part_template<value_type>> parts;
    };

    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    basic_chain<T, C1, C2> basic_chain<T, C1, C2>::join(const basic_chain<T, C1, C2>& chain) {
        basic_chain<T, C1, C2> joined_chain(chain);
        joined_chain.parts.emplace_back();

        return joined_chain;
    }

    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    basic_chain<T, C1, C2>::basic_chain() noexcept {
        parts.emplace_back();
    }

    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    basic_chain<T, C1, C2>::basic_chain(const basic_chain<T, C1, C2>& chain) noexcept
        : parts(chain.parts) {}

    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    basic_chain<T, C1, C2>::basic_chain(basic_chain<T, C1, C2>&& chain) noexcept
        : parts(std::move(chain.parts)) {}

    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    void basic_chain<T, C1, C2>::push(const value_type& value) {
        parts.back().push(value);
    }

    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    void basic_chain<T, C1, C2>::push(value_type&& value) {
        parts.back().push(std::move(value));
    }

    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    template <class... AS>
    void basic_chain<T, C1, C2>::emplace(AS&&... arguments) {
        parts.back().emplace(std::forward<AS>(arguments)...);
    }

    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    typename basic_chain<T, C1, C2>::iterator basic_chain<T, C1, C2>::begin() noexcept {
        return iterator(parts.begin(), parts.end());
    }

    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    typename basic_chain<T, C1, C2>::const_iterator basic_chain<T, C1, C2>::begin() const noexcept {
        return const_iterator(parts.begin(), parts.end());
    }

    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    typename basic_chain<T, C1, C2>::iterator basic_chain<T, C1, C2>::end() noexcept {
        return iterator(parts.end());
    }

    template <class T, template <class, class...> class C1, template <class, class...> class C2>
    typename basic_chain<T, C1, C2>::const_iterator basic_chain<T, C1, C2>::end() const noexcept {
        return iterator(parts.end());
    }

    template <class T, template <class, class...> class C = std::vector>
    using chain = basic_chain<T, C, C>;


    template <class... TS, class T>
    static void _join(std::stringstream& ss, const TS&... rest, const T& last) {
        ((ss << rest << ", "), ...);
        ss << last;
    }

    template <class... TS>
    static std::string join(const TS&... parts) {
        std::stringstream ss;
        //_join(ss, parts...);
        return ss.str();
    }


    class context_exception : public std::exception {
    public:
        explicit context_exception(std::string context) noexcept;

        template <class... TS>
        explicit context_exception(const TS&... context_parts) noexcept;

        const char* what() const noexcept override;

    protected:
        std::string _context;
    };

    context_exception::context_exception(std::string context) noexcept
        : _context(std::move(context)) {}

    template <class... TS>
    context_exception::context_exception(const TS&... context_parts) noexcept
        : _context(join(context_parts...)) {}

    const char* context_exception::what() const noexcept {
        return _context.c_str();
    }
}

#endif //DRONE_UTILITY_HPP
