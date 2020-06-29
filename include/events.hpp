#ifndef DRONE_EVENTS_HPP
#define DRONE_EVENTS_HPP

#include <iostream>

#include "../include/utility.hpp"

namespace drone::events {

    using event_priority = enum : unsigned char { PRIORITY_LOW = 0, PRIORITY_DEFAULT = 1, PRIORITY_HIGH = 2 };

    template <class T>
    struct event_priority_traits;

    template<>
    struct event_priority_traits<event_priority> {
        static constexpr size_t level_count = 3;
        static constexpr event_priority default_level = PRIORITY_DEFAULT;
    };

    template <class T = void>
    struct event_handler_traits {
    public:
        using event_priority_type = event_priority;
    };


    class event;

    class basic_event_handler {
    public:
        virtual ~basic_event_handler() = default;
        virtual void handle(const event& event) {};
    };

    template <class E>
    class event_handler : public basic_event_handler {
    public:
        using event_type = E;

        void handle(const event& event) override {
            handle_event(static_cast<const event_type&>(event));
        }

        virtual void handle_event(const event_type& event) {};
    };

    class event {};

    template <class E>
    class event_pool {
    public:
        using event_priority_type = event_handler_traits<>::event_priority_type;
        using event_priority_traits = event_priority_traits<event_priority_type>;

        using handler_type = basic_event_handler;
        using handler_pointer_type = std::shared_ptr<basic_event_handler>;
        using handler_list_type = utility::chain<handler_pointer_type>;

        using priority_handler_list_type = std::vector<handler_list_type>;

        static priority_handler_list_type handlers;
        static priority_handler_list_type concatenate_handlers();
    };

    template <class E>
    typename event_pool<E>::priority_handler_list_type event_pool<E>::concatenate_handlers() {
        priority_handler_list_type next_handlers;
        next_handlers.reserve(event_priority_traits::level_count);

        for (const auto& priority_handlers : event_pool<E>::handlers)
            next_handlers.push_back(handler_list_type::join(priority_handlers));

        return next_handlers;
    }

    template <class E>
    typename event_pool<E>::priority_handler_list_type event_pool<E>::handlers;

    template <>
    event_pool<event>::priority_handler_list_type event_pool<event>::handlers(event_pool<event>::event_priority_traits::level_count);


    class event_manager {
    public:
        using event_priority_type = event_handler_traits<>::event_priority_type;
        using event_priority_traits = event_priority_traits<event_priority_type>;

    private:
        template <class E>
        using event_handler_template = event_handler<E>;

        static constexpr event_priority_type default_priority = event_priority_traits::default_level;

    public:
        template <class E>
        static void register_handler(event_handler_template<E>* handler_pointer, event_priority_type priority = default_priority);

        template <class H>
        static void register_handler(const H& handler, event_priority_type priority = default_priority);

        template <class H>
        static void register_handler(H&& handler, event_priority_type priority = default_priority);

        template <class E>
        static void fire_event(const E& event);
    };

    template <class E>
    void event_manager::register_handler(event_handler_template<E>* handler_pointer, event_priority_type priority) {
        if (event_pool<E>::handlers.empty())
            event_pool<E>::handlers = event_pool<typename E::base_event_type>::concatenate_handlers();

        event_pool<E>::handlers[priority].emplace(handler_pointer);
    }

    template <class H>
    void event_manager::register_handler(const H& handler, event_priority_type priority) {
        event_manager::register_handler<typename H::event_type>(new H(handler), priority);
    }

    template <class H>
    void event_manager::register_handler(H&& handler, event_priority_type priority) {
        event_manager::register_handler<typename H::event_type>(new H(std::forward<H>(handler)), priority);
    }

    template <class E>
    void event_manager::fire_event(const E& event) {
        for (auto& priority_handlers : event_pool<E>::handlers)
            for (auto& handler : priority_handlers)
                handler->handle(event);
    }
}

#endif //DRONE_EVENTS_HPP
