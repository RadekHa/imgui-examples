#pragma once
#include <functional>
#include <queue>
#include <tuple>
#include <variant>
#include <vector>

namespace App
{
    /** Template class representing an event bus for handling events of various types. */
    template<typename Variant>
    class EventBusImpl;

    /** Specialization of the EventBusImpl template class for a variant of events. */
    template<typename... Events>
    class EventBusImpl<std::variant<Events...> >
    {
    public:
        /** Type alias for the variant type representing all events. */
        using VariantType = std::variant<Events...>;
        /** Type alias for the subscription ID used to identify event handlers. */
        using SubscriptionId = size_t;

    private:
        /** Struct representing an event handler, containing a subscription ID and a function to handle the event. */
        template<typename T>
        struct Handler
        {
            SubscriptionId id;
            std::function<void (const T&)> fn;
        };

        /** Type alias for a list of handlers for a specific event type. */
        template<typename T>
        using HandlerList = std::vector<Handler<T> >;
        /** Tuple containing handler lists for all event types. */
        std::tuple<HandlerList<Events> ...> m_handlers;
        /** Queue for storing published events before they are dispatched. */
        std::queue<VariantType> m_queue;
        /** Variable to keep track of the last subscription ID assigned to an event handler. */
        SubscriptionId m_lastId = 0;

    public:

        /** Class representing a subscription to an event, allowing for automatic unsubscription when the object goes out of scope. */
        class Subscription
        {
        public:
            /** Default constructor for Subscription. */
            Subscription () = default;
            /** Constructor for Subscription. */
            Subscription (EventBusImpl* bus, size_t typeIndex, SubscriptionId id)
                : m_bus (bus)
                , m_typeIndex (typeIndex)
                , m_id (id)
            {
            }

            /** The copy constructor is deleted. */
            Subscription (const Subscription&) = delete;
            /** The copy assignment operator is deleted. */
            Subscription& operator= (const Subscription&) = delete;
            /** The move constructor for Subscription, allowing for efficient transfer of ownership. */
            Subscription (Subscription&& other) noexcept
            {
                moveFrom (std::move (other));
            }

            /** The move assignment operator for Subscription, allowing for efficient transfer of ownership. */
            Subscription& operator= (Subscription&& other) noexcept
            {
                if (this != &other)
                {
                    unsubscribe ();
                    moveFrom (std::move (other));
                }
                return *this;
            }

            /** Destructor for Subscription, automatically unsubscribing from the event when the object goes out of scope. */
            ~Subscription ()
            {
                unsubscribe ();
            }

            /** Unsubscribes from the event, removing the associated handler from the event bus. */
            void unsubscribe ()
            {
                if (m_bus)
                {
                    m_bus->unsubscribe (m_typeIndex, m_id);
                    m_bus = nullptr;
                }
            }

        private:
            /* Helper function to move the subscription data from another Subscription object. */
            void moveFrom (Subscription&& other)
            {
                m_bus = other.m_bus;
                m_typeIndex = other.m_typeIndex;
                m_id = other.m_id;

                other.m_bus = nullptr;
            }

            /* Pointer to the event bus that this subscription is associated with. */
            EventBusImpl* m_bus = nullptr;
            /* Index representing the type of event this subscription is associated with. */
            size_t m_typeIndex = 0;
            /* Subscription ID used to identify the specific handler associated with this subscription. */
            SubscriptionId m_id = 0;
        };

    public:
        /** Subscribes a handler function to a specific event type, returning a Subscription object that can be used to manage the subscription. */
        template<typename Event, typename F>
        Subscription subscribe (F&& f)
        {
            auto& vec = getVector<Event>();

            SubscriptionId id = ++m_lastId;

            vec.push_back ({id, std::forward<F> (f)});

            return Subscription (this, typeIndex<Event>(), id);
        }

        /** Subscribes a member function of an object to a specific event type, returning a Subscription object that can be used to manage the subscription. */
        template<typename Event, typename Obj>
        Subscription subscribe (Obj* object, void (Obj::* method) (const Event &))
        {
            return subscribe<Event> ( [object, method] (const Event& e) {
                (object->*method)(e);
            });
        }

    public:
        /** Publishes an event to all subscribers of that event type. */
        template<typename Event>
        void publish (const Event& e)
        {
            m_queue.emplace (e);
        }

        /** Dispatches all events in the queue to their respective handlers. */
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
        /** Helper function to dispatch a single event to its respective handlers. */
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

        /** Helper function to get the vector of handlers for a specific event type. */
        template<typename Event>
        auto& getVector ()
        {
            return std::get<HandlerList<Event> > (m_handlers);
        }

        /** Helper function to get the index of a specific event type in the variant. */
        template<typename Event>
        constexpr size_t typeIndex ()
        {
            return variantIndex<Event, VariantType>();
        }

        /** Helper function to unsubscribe a handler from the event bus based on the type index and subscription ID. */
        void unsubscribe (size_t typeIndex, SubscriptionId id)
        {
            unsubscribeImpl (typeIndex, id, std::index_sequence_for<Events...> {});
        }

        /** Helper function to unsubscribe a handler from the event bus based on the type index and subscription ID, using an index sequence to iterate through the event types. */
        template<size_t... I>
        void unsubscribeImpl (size_t typeIndex, SubscriptionId id, std::index_sequence<I...> )
        {
            ((typeIndex == I ? removeFromVector<Events> (id) : void ()), ...);
        }

        /** Helper function to remove a handler from the vector of handlers for a specific event type based on the subscription ID. */
        template<typename Event>
        void removeFromVector (SubscriptionId id)
        {
            auto& vec = getVector<Event>();
            vec.erase (std::remove_if (vec.begin (), vec.end (), [id] (const auto& h) {
                return h.id == id;
            }), vec.end ());
        }

    private:
        /** Helper struct to compute the index of a specific type in the variant. */
        template<typename T, typename Variant>
        struct VariantIndex;

        /** Specialization of the VariantIndex struct for when the type T is the first type in the variant. */
        template<typename T, typename... Ts>
        struct VariantIndex<T, std::variant<T, Ts...> >
        {
            static constexpr size_t value = 0;
        };

        /** Specialization of the VariantIndex struct for when the type T is not the first type in the variant, recursively checking the remaining types. */
        template<typename T, typename U, typename... Ts>
        struct VariantIndex<T, std::variant<U, Ts...> >
        {
            static constexpr size_t value = 1 + VariantIndex<T, std::variant<Ts...> >::value;
        };

        /** Static helper function to get the index of a specific type in the variant using the VariantIndex struct. */
        template<typename T, typename Variant>
        static constexpr size_t variantIndex ()
        {
            return VariantIndex<T, Variant>::value;
        }
    };

    /** Event representing a quit request. */
    struct EventQuit {};
    /** Event representing a close request. */
    struct EventClose {};
    /** Event representing a minimized request. */
    struct EventMinimized {};
    /** Event representing a shown request. */
    struct EventShown {};

    /** Type alias for the variant of all events used in the application. */
    using Events = std::variant<EventQuit, EventClose, EventMinimized, EventShown>;

    /** Type alias for the event bus used in the application, based on the defined events. */
    using EventBus = EventBusImpl<Events>;
}
