#pragma once
#include <algorithm>
#include <memory>
#include <tuple>
#include <typeindex>
#include <variant>
#include <vector>

namespace App
{
    // A simple type-safe event bus that allows subscribing to and publishing events of different types.
    template<typename Variant>
    class EventBus;

    // Specialization for std::variant. The event bus will support all event types listed in the variant.
    template<typename... Events>
    class EventBus<std::variant<Events...> >
    {
    public:
        using VariantType = std::variant<Events...>;
        using SubscriptionId = std::size_t;

    private:

        template<typename TEvent>
        struct HandlerEntry
        {
            SubscriptionId id;
            void* object;
            void (* invoke)(void*, const TEvent&);
            void (* destroy)(void*);
        };

        std::tuple<std::vector<HandlerEntry<Events> > ...> m_handlers;

        SubscriptionId m_lastId = 0;

    public:

        ~EventBus ()
        {
            ( [&] {
                auto& vec = getVector<Events>();

                for (auto& e : vec)
                {
                    e.destroy (e.object);
                }
            }
              (), ...);
        }

        // Subscription object that automatically unsubscribes when it goes out of scope.
        class Subscription
        {
        public:
            Subscription () = default;

            Subscription (EventBus* bus, std::type_index type, SubscriptionId id)
                : m_bus (bus)
                , m_type (type)
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
                    m_bus->unsubscribeRaw (m_type, m_id);
                    m_bus = nullptr;
                }
            }

        private:
            void moveFrom (Subscription&& other)
            {
                m_bus = other.m_bus;
                m_type = other.m_type;
                m_id = other.m_id;

                other.m_bus = nullptr;
            }

            EventBus* m_bus = nullptr;
            std::type_index m_type{typeid (void)};
            SubscriptionId m_id = 0;
        };

        // Subscribe with a lambda or any callable object.
        // The callable will be stored on the heap, so it must be copyable or movable.
        template<typename TEvent, typename F>
        Subscription subscribe (F&& f)
        {
            using Functor = std::decay_t<F>;
            auto* heapObject = new Functor (std::forward<F> (f));
            auto& vec = getVector<TEvent>();

            SubscriptionId id = ++m_lastId;

            vec.push_back ({
                id,
                heapObject,
                [] (void* obj, const TEvent& event)
                {
                    (*static_cast<Functor*> (obj))(event);
                },
                [] (void* obj)
                {
                    delete static_cast<Functor*> (obj);
                }
            });

            return Subscription (this, typeid (TEvent), id);
        }

        // Subscribe with a member function pointer.
        template<typename TEvent, typename TObject>
        Subscription subscribe (TObject* instance, void (TObject::* method) (const TEvent &))
        {
            return subscribe<TEvent> ( [instance, method] (const TEvent& e) {
                (instance->*method)(e);
            });
        }

        // Publish an event to all subscribers of that event type.
        template<typename TEvent>
        void publish (const TEvent& event)
        {
            auto& vec = getVector<TEvent>();

            for (size_t i = 0; i < vec.size (); ++i)
            {
                auto entry = vec [i];
                entry.invoke (entry.object, event);
            }
        }

        // Publish an event from a variant. The correct handler will be called based on the actual type of the event.
        void publishVariant (const VariantType& v)
        {
            std::visit ( [this] (const auto& e) {
                publish (e);
            }, v);
        }

    private:

        template<typename TEvent>
        auto& getVector ()
        {
            return std::get<std::vector<HandlerEntry<TEvent> > > (m_handlers);
        }

        void unsubscribeRaw (std::type_index type, SubscriptionId id)
        {
            ((type == typeid (Events) ? removeFromVector<Events> (id) : void ()), ...);
        }

        template<typename TEvent>
        void removeFromVector (SubscriptionId id)
        {
            auto& vec = getVector<TEvent>();

            auto it = std::find_if (vec.begin (), vec.end (), [id] (const auto& e) {
                return e.id == id;
            });

            if (it != vec.end ())
            {
                it->destroy (it->object);
                vec.erase (it);
            }
        }
    };
}
