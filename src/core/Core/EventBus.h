#pragma once
#include <functional>
#include <queue>
#include <tuple>
#include <variant>
#include <vector>

namespace App
{
    template<typename Variant>
    class EventBusImpl;

    template<typename... Events>
    class EventBusImpl<std::variant<Events...> >
    {
    public:

        using VariantType = std::variant<Events...>;
        using SubscriptionId = size_t;

    private:

        template<typename T>
        struct Handler
        {
            SubscriptionId id;
            std::function<void (const T&)> fn;
        };

        template<typename T>
        using HandlerList = std::vector<Handler<T> >;

        std::tuple<HandlerList<Events> ...> m_handlers;

        std::queue<VariantType> m_queue;

        SubscriptionId m_lastId = 0;

    public:

        class Subscription
        {
        public:

            Subscription () = default;

            Subscription (EventBusImpl* bus, size_t typeIndex, SubscriptionId id)
                : m_bus (bus)
                , m_typeIndex (typeIndex)
                , m_id (id)
            {
            }

            Subscription (const Subscription&) = delete;
            Subscription& operator= (const Subscription&) = delete;

            Subscription (Subscription&& other) noexcept
            {
                moveFrom (std::move (other));
            }

            Subscription& operator= (Subscription&& other) noexcept
            {
                if (this != &other)
                {
                    unsubscribe ();
                    moveFrom (std::move (other));
                }
                return *this;
            }

            ~Subscription ()
            {
                unsubscribe ();
            }

            void unsubscribe ()
            {
                if (m_bus)
                {
                    m_bus->unsubscribe (m_typeIndex, m_id);
                    m_bus = nullptr;
                }
            }

        private:

            void moveFrom (Subscription&& other)
            {
                m_bus = other.m_bus;
                m_typeIndex = other.m_typeIndex;
                m_id = other.m_id;

                other.m_bus = nullptr;
            }

            EventBusImpl* m_bus = nullptr;
            size_t m_typeIndex = 0;
            SubscriptionId m_id = 0;
        };

    public:

        template<typename Event, typename F>
        Subscription subscribe (F&& f)
        {
            auto& vec = getVector<Event>();

            SubscriptionId id = ++m_lastId;

            vec.push_back ({id, std::forward<F> (f)});

            return Subscription (this, typeIndex<Event>(), id);
        }

        template<typename Event, typename Obj>
        Subscription subscribe (Obj* object, void (Obj::* method) (const Event &))
        {
            return subscribe<Event> ( [object, method] (const Event& e) {
                (object->*method)(e);
            });
        }

    public:

        template<typename Event>
        void publish (const Event& e)
        {
            m_queue.emplace (e);
        }

        void dispatch ()
        {
            while (!m_queue.empty ())
            {
                VariantType v = std::move (m_queue.front ());
                m_queue.pop ();

                std::visit ( [this] (auto&& e)
                {
                    dispatchSingle (e);
                }, v);
            }
        }

    private:

        template<typename Event>
        void dispatchSingle (const Event& e)
        {
            // Copy handlers to avoid issues with handlers being modified during dispatch.
            HandlerList<Event> handlers = getVector<Event>();

            for (auto& h : handlers)
            {
                h.fn (e);
            }
        }

        template<typename Event>
        auto& getVector ()
        {
            return std::get<HandlerList<Event> > (m_handlers);
        }

        template<typename Event>
        constexpr size_t typeIndex ()
        {
            return variantIndex<Event, VariantType>();
        }

        void unsubscribe (size_t typeIndex, SubscriptionId id)
        {
            unsubscribeImpl (typeIndex, id, std::index_sequence_for<Events...> {});
        }

        template<size_t... I>
        void unsubscribeImpl (size_t typeIndex, SubscriptionId id, std::index_sequence<I...> )
        {
            ((typeIndex == I ? removeFromVector<Events> (id) : void ()), ...);
        }

        template<typename Event>
        void removeFromVector (SubscriptionId id)
        {
            auto& vec = getVector<Event>();
            vec.erase (std::remove_if (vec.begin (), vec.end (), [id] (const auto& h) {
                return h.id == id;
            }), vec.end ());
        }

    private:

        template<typename T, typename Variant>
        struct VariantIndex;

        template<typename T, typename... Ts>
        struct VariantIndex<T, std::variant<T, Ts...> >
        {
            static constexpr size_t value = 0;
        };

        template<typename T, typename U, typename... Ts>
        struct VariantIndex<T, std::variant<U, Ts...> >
        {
            static constexpr size_t value = 1 + VariantIndex<T, std::variant<Ts...> >::value;
        };

        template<typename T, typename Variant>
        static constexpr size_t variantIndex ()
        {
            return VariantIndex<T, Variant>::value;
        }
    };


    struct EventQuit {};
    struct EventClose {};
    struct EventMinimized {};
    struct EventShown {};

    using Events = std::variant<EventQuit, EventClose, EventMinimized, EventShown>;

    using EventBus = EventBusImpl<Events>;
}
