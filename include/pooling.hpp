#ifndef DRONE_POOLING_HPP
#define DRONE_POOLING_HPP

#include <iostream>
#include <vector>
#include <exception>

#include <cstdlib>


namespace drone::pooling {

    template <class U>
    class basic_contiguous_allocator {
    public:
        using unit_type = U;
        using unit_pointer_type = unit_type*;

        static unit_pointer_type allocate(size_t size);
        static void deallocate(unit_pointer_type pointer);

        static unit_pointer_type reallocate(unit_pointer_type pointer, size_t size);

        template <class I1, class I2>
        static unit_pointer_type reallocate(unit_pointer_type pointer, size_t size, I1 first, I2 last);

        template <class R>
        static void construct(R* pointer, const R& instance);

        template <class R>
        static void construct(R* pointer, R&& instance);

        template <class R, class... AS>
        static void construct(R* pointer, AS&&... arguments);

        template <class R>
        static void destruct(R* pointer);

        template <class A, class B>
        static void copy(A* destination_pointer, B* source_pointer, size_t source_size);
    };

    template <class U>
    typename basic_contiguous_allocator<U>::unit_pointer_type basic_contiguous_allocator<U>::allocate(size_t size) {
        auto unit_pointer = (unit_pointer_type) std::malloc(size);

        if (unit_pointer == nullptr)
            throw std::bad_alloc();

        return unit_pointer;
    }

    template <class U>
    void basic_contiguous_allocator<U>::deallocate(unit_pointer_type pointer) {
        std::free((void*) pointer);
    }

    template <class U>
    typename basic_contiguous_allocator<U>::unit_pointer_type basic_contiguous_allocator<U>::reallocate(unit_pointer_type pointer,
                                                                                                        size_t size) {
        auto unit_pointer = (unit_pointer_type) std::realloc((void*) pointer, size);

        if (unit_pointer == nullptr)
            throw std::bad_alloc();

        return unit_pointer;
    }

    template <class U>
    template <class I1, class I2>
    typename basic_contiguous_allocator<U>::unit_pointer_type basic_contiguous_allocator<U>::reallocate(
                                                              unit_pointer_type pointer, size_t size, I1 first, I2 last) {
        for (; first != last; ++first)
            destruct(pointer + *first);

        return basic_contiguous_allocator<U>::reallocate(pointer, size);
    }

    template <class U>
    template <class R>
    void basic_contiguous_allocator<U>::construct(R* pointer, const R& instance) {
        new ((void*) pointer) R(instance);
    }

    template <class U>
    template <class R>
    void basic_contiguous_allocator<U>::construct(R* pointer, R&& instance) {
        new ((void*) pointer) R(std::forward<R>(instance));
    }

    template <class U>
    template <class R, class... AS>
    void basic_contiguous_allocator<U>::construct(R* pointer, AS&&... arguments) {
        new ((void*) pointer) R(std::forward<AS>(arguments)...);
    }

    template <class U>
    template <class R>
    void basic_contiguous_allocator<U>::destruct(R* pointer) {
        pointer->~R();
    }

    template <class U>
    template <class A, class B>
    void basic_contiguous_allocator<U>::copy(A* destination_pointer, B* source_pointer, size_t source_size) {
        std::memcpy((void*) destination_pointer, (void*) source_pointer, source_size);
    }

    using contiguous_allocator = basic_contiguous_allocator<std::byte>;








    template <class T, T Value>
    struct basic_allocation {
        static constexpr T _size_factor = Value;

        static size_t calculate(size_t size, size_t desired_size) {
            T new_size = size;

            do    (new_size *= _size_factor);
            while (new_size <= desired_size);

            return static_cast<size_t>(new_size);
        }
    };

    template <>
    struct basic_allocation<size_t, 2u> {
        static size_t calculate(size_t size, size_t desired_size) {
            static constexpr size_t n = sizeof(size_t);

            desired_size--;
            for (size_t i = 1; i < n; i *= 2)
                desired_size |= desired_size >> i;
            desired_size++;

            return desired_size;


            return (desired_size >> 1u) << 2u;
        }
    };

    using allocation = basic_allocation<size_t, 2u>;


    template <class A>
    class basic_contiguous_memory {
    public:
        using allocator = A_T;

        using pointer_type = P_T;
        using data_pointer_type = DP_T;

        basic_contiguous_memory(const basic_contiguous_memory<P_T, DP_T, A_T>& memory);
        basic_contiguous_memory(basic_contiguous_memory<P_T, DP_T, A_T>&& memory) noexcept;
        ~basic_contiguous_memory();

        basic_contiguous_memory() : basic_contiguous_memory(_initial_size) {}
        explicit basic_contiguous_memory(size_t initial_size);

        basic_contiguous_memory<P_T, DP_T, A_T>& operator=(const basic_contiguous_memory<P_T, DP_T, A_T>& memory);
        basic_contiguous_memory<P_T, DP_T, A_T>& operator=(basic_contiguous_memory<P_T, DP_T, A_T>&& memory) noexcept;

        inline data_pointer_type get(size_t position);

        template <class _P_T>
        inline void set(size_t destination_position, _P_T source_pointer, size_t source_size);

        inline data_pointer_type operator[](size_t position) const;

    //private:
        //inline size_t reallocation_size(size_t destination_size);
        static constexpr size_t _size_factor = 2u;
        static constexpr size_t _initial_size = 1024u;

        using allocation = basic_allocation<size_t, _size_factor>;

        data_pointer_type _data_pointer;
        size_t _data_size;
    };

    template <class P_T, class DP_T, class A_T>
    basic_contiguous_memory<P_T, DP_T, A_T>::basic_contiguous_memory(const basic_contiguous_memory<P_T, DP_T, A_T>& memory)
            : _data_size(memory._data_size)
    {
        _data_pointer = allocator::allocate(memory._data_size);
        allocator::copy(_data_pointer, memory._data_pointer, memory._data_size);
    }

    template <class P_T, class DP_T, class A_T>
    basic_contiguous_memory<P_T, DP_T, A_T>::basic_contiguous_memory(basic_contiguous_memory<P_T, DP_T, A_T>&& memory) noexcept
            : _data_pointer(memory._data_pointer), _data_size(memory._data_size)
    {
        memory._data_pointer = nullptr;
        memory._data_size = 0;
    }

    template <class P_T, class DP_T, class A_T>
    basic_contiguous_memory<P_T, DP_T, A_T>::basic_contiguous_memory(size_t initial_size)
        : _data_size(initial_size)
    {
        _data_pointer = allocator::allocate(initial_size);
    }

    template <class P_T, class DP_T, class A_T>
    basic_contiguous_memory<P_T, DP_T, A_T>::~basic_contiguous_memory() {
        allocator::deallocate(_data_pointer);
        _data_size = 0; // TODO ?
    }

    template <class P_T, class DP_T, class A_T>
    basic_contiguous_memory<P_T, DP_T, A_T>& basic_contiguous_memory<P_T, DP_T, A_T>::operator=(const basic_contiguous_memory<P_T, DP_T, A_T>& memory) {
        if (_data_size != memory._data_size) {
            _data_pointer = allocator::reallocate(_data_pointer, memory._data_size);
            _data_size = memory._data_size;
        }

        allocator::copy(_data_pointer, memory._data_pointer, memory._data_size);
    }

    template <class P_T, class DP_T, class A_T>
    basic_contiguous_memory<P_T, DP_T, A_T>& basic_contiguous_memory<P_T, DP_T, A_T>::operator=(basic_contiguous_memory<P_T, DP_T, A_T>&& memory) noexcept {
        // TODO maybe check if equal
        _data_size = memory._data_size;
        _data_pointer = memory._data_pointer;

        memory._data_pointer = nullptr;
        memory._data_size = 0;
    }

    template <class P_T, class DP_T, class A_T>
    typename basic_contiguous_memory<P_T, DP_T, A_T>::data_pointer_type basic_contiguous_memory<P_T, DP_T, A_T>::get(size_t position) {
        return _data_pointer + position;
    }

    template <class P_T, class DP_T, class A_T>
    template <class _P_T>
    void basic_contiguous_memory<P_T, DP_T, A_T>::set(size_t destination_position,
                                                      _P_T source_pointer, size_t source_size) {
        size_t destination_size = destination_position + source_size;

        if (destination_size > _data_size) {
            size_t new_data_size = allocation::calculate(_data_size, destination_size);
            _data_pointer = allocator::reallocate(_data_pointer, new_data_size);
            std::cout << "realloc: " << _data_size << " " << new_data_size << std::endl;
            _data_size = new_data_size;
        }

        allocator::copy(_data_pointer + destination_position, source_pointer, source_size);
    }

    template <class P_T, class DP_T, class A_T>
    typename basic_contiguous_memory<P_T, DP_T, A_T>::data_pointer_type basic_contiguous_memory<P_T, DP_T, A_T>::operator[](size_t position) const {
        return get(position);
    }

    using contiguous_memory = basic_contiguous_memory<void*, char*>;


    template <class M_T, class B_T>
    class object_pool_iterator;

    template <class M_T, class B_T>
    using const_object_pool_iterator = object_pool_iterator<const typename std::remove_const<M_T>::type, B_T>;


    template <class M_T, class B_T = void>
    class basic_object_pool {
    private:
        using object_block_type = M_T;
        using object_size_table_type = std::vector<size_t>;

    public:
        using base_object_type = B_T;

        using iterator = object_pool_iterator<M_T, B_T>;
        using const_iterator = const_object_pool_iterator<M_T, B_T>;

        friend iterator;
        friend const_iterator;

        basic_object_pool() = default;
        explicit basic_object_pool(size_t initial_block_size);
        explicit basic_object_pool(size_t initial_block_size, size_t initial_table_size);

        template <class T>
        void push(const T& object);

        template <class T = base_object_type>
        T* at_position(size_t position);

        template <class T = base_object_type>
        T* at(size_t index);

        iterator begin() noexcept;
        const_iterator begin() const noexcept;

        iterator end() noexcept;
        const_iterator end() const noexcept;

    //protected:
        object_block_type object_block;
        object_size_table_type object_size_table;
    };

    template <class M_T, class B_T>
    basic_object_pool<M_T, B_T>::basic_object_pool(size_t initial_block_size, size_t initial_table_size)
        : basic_object_pool<M_T, B_T>(initial_block_size)
    {
        object_size_table.reserve(initial_table_size);
    }

    template <class M_T, class B_T>
    basic_object_pool<M_T, B_T>::basic_object_pool(size_t initial_block_size)
        : object_block(initial_block_size), object_size_table(1, 0) {}

    template <class M_T, class B_T>
    template <class T>
    void basic_object_pool<M_T, B_T>::push(const T& object) {
        size_t object_size = sizeof(T);
        size_t destination_position = object_size_table.back();

        //std::cout << destination_position << " at push of size: " << object_size << std::endl;

        object_block.set(destination_position, &object, object_size);
        object_size_table.push_back(destination_position + object_size);
    }

    template <class M_T, class B_T>
    template <class T>
    T* basic_object_pool<M_T, B_T>::at_position(size_t position) {
        return (T*) object_block.get(position);
    }

    template <class M_T, class B_T>
    template <class T>
    T* basic_object_pool<M_T, B_T>::at(size_t index) {
        return at_position<T>(object_size_table[index]);
    }

    template <class M_T, class B_T>
    typename basic_object_pool<M_T, B_T>::iterator basic_object_pool<M_T, B_T>::begin() noexcept {
        return iterator(object_block, object_size_table.begin());
    }

    template <class M_T, class B_T>
    typename basic_object_pool<M_T, B_T>::const_iterator basic_object_pool<M_T, B_T>::begin() const noexcept {
        return const_iterator(object_block, object_size_table.begin());
    }

    template <class M_T, class B_T>
    typename basic_object_pool<M_T, B_T>::iterator basic_object_pool<M_T, B_T>::end() noexcept {
        return iterator(object_block, --object_size_table.end());
    }

    template <class M_T, class B_T>
    typename basic_object_pool<M_T, B_T>::const_iterator basic_object_pool<M_T, B_T>::end() const noexcept {
        return const_iterator(object_block, --object_size_table.end());
    }


    template <class B_T>
    using object_pool = basic_object_pool<contiguous_memory, B_T>;


    template <class M_T, class B_T>
    class object_pool_iterator : std::iterator<std::forward_iterator_tag, B_T> {
    private:
        using base = std::iterator<std::forward_iterator_tag, B_T>;

        using object_pool_type = basic_object_pool<M_T, B_T>;
        using object_block_type = typename object_pool_type::object_block_type;

        using object_size_iterator = typename object_pool_type::object_size_table_type::const_iterator;

    public:
        using typename base::iterator_category;
        using typename base::value_type;
        using typename base::difference_type;
        using typename base::pointer;
        using typename base::reference;

        object_pool_iterator() = delete;
        explicit object_pool_iterator(object_block_type& object_block,
                                      object_size_iterator object_size_it) noexcept;

        pointer operator->() const;
        reference operator*() const;

        object_pool_iterator<M_T, B_T>& operator++();
        const object_pool_iterator<M_T, B_T> operator++(int);

        bool operator==(const object_pool_iterator<M_T, B_T>& rhs) const;
        bool operator!=(const object_pool_iterator<M_T, B_T>& rhs) const;

    private:
        object_block_type* object_block;
        object_size_iterator object_size_it;
    };

    template <class M_T, class B_T>
    typename object_pool_iterator<M_T, B_T>::pointer object_pool_iterator<M_T, B_T>::operator->() const {
        return (pointer) object_block->get(*object_size_it);
    }

    template <class M_T, class B_T>
    typename object_pool_iterator<M_T, B_T>::reference object_pool_iterator<M_T, B_T>::operator*() const {
        return *operator->();
    }

    template <class M_T, class B_T>
    object_pool_iterator<M_T, B_T>::object_pool_iterator(object_block_type& object_block,
                                                         object_size_iterator object_size_it) noexcept
        : object_block(&object_block), object_size_it(std::move(object_size_it)) {}

    template <class M_T, class B_T>
    object_pool_iterator<M_T, B_T>& object_pool_iterator<M_T, B_T>::operator++() {
        ++object_size_it;
        return *this;
    }

    template <class M_T, class B_T>
    const object_pool_iterator<M_T, B_T> object_pool_iterator<M_T, B_T>::operator++(int) {
        object_pool_iterator<M_T, B_T> iterator(*this);
        operator++();

        return iterator;
    }

    template <class M_T, class B_T>
    bool object_pool_iterator<M_T, B_T>::operator==(const object_pool_iterator<M_T, B_T>& rhs) const {
        return object_size_it == rhs.object_size_it;
    }

    template <class M_T, class B_T>
    bool object_pool_iterator<M_T, B_T>::operator!=(const object_pool_iterator<M_T, B_T>& rhs) const {
        return !operator==(rhs);
    }



















    class memory_pool {
    public:
        memory_pool() = delete;
        explicit memory_pool(size_t size) noexcept
                : memory_pointer(new char[size]), memory_size(size), prefix_size_table(1, 0) {}

        template <class T>
        T* get(size_t i) noexcept {
            size_t object_size = sizeof(T);
            size_t object_begin_index = prefix_size_table[i];

            char* raw_pointer = memory_pointer.get();
            return (T*) (raw_pointer + object_begin_index);
        }

        template <class T>
        bool put(const T& object) noexcept {
            size_t object_size = sizeof(T);

            size_t object_begin_index = prefix_size_table.back();
            size_t object_end_index = object_begin_index + object_size;

            if (memory_size < object_end_index)
                return false;

            prefix_size_table.push_back(object_end_index);

            char* raw_pointer = memory_pointer.get();
            char* raw_object_pointer = (char*) &object;

            std::copy(raw_object_pointer, raw_object_pointer + object_size, raw_pointer + object_begin_index);
            return true;
        }

    private:
        std::unique_ptr<char> memory_pointer;
        size_t memory_size;

        std::vector<size_t> prefix_size_table;
    };

    class _memory_pool { // maybe just char vector tbh
    public:
        _memory_pool() = delete;
        explicit _memory_pool(size_t size) noexcept
                : prefix_size_table(1, 0) {
            memory_pointer.reserve(size);
        }

        template <class T>
        T* get(size_t i) noexcept {
            size_t object_size = sizeof(T);
            size_t object_begin_index = prefix_size_table[i];

            char* raw_pointer = memory_pointer.data();
            return (T*) (raw_pointer + object_begin_index);
        }

        template <class T>
        void put(const T& object) noexcept {
            size_t object_size = sizeof(T);

            size_t object_begin_index = prefix_size_table.back();
            size_t object_end_index = object_begin_index + object_size;

            if (memory_pointer.capacity() < object_end_index)
                memory_pointer.reserve(memory_pointer.capacity() * 2);

            prefix_size_table.push_back(object_end_index);

            char* raw_pointer = memory_pointer.data();
            char* raw_object_pointer = (char*) &object;

            std::copy(raw_object_pointer, raw_object_pointer + object_size, raw_pointer + object_begin_index);
        }

    private:
        std::vector<char> memory_pointer;
        std::vector<size_t> prefix_size_table;
    };

}

#endif //DRONE_POOLING_HPP
