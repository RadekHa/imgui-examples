#pragma once
#include "AppEvents.h"
#include "EventBusImpl.h"

#include <variant>

namespace App
{
    /** Type alias for the variant of all events used in the application. */
    using Events = std::variant<
        EventQuit,
        EventClose,
        EventMinimized,
        EventShown,
        EventRestored,
        EventDisplayChanged
        >;

    /** Type alias for the event bus used in the application, based on the defined events. */
    using EventBus = EventBusImpl<Events>;
}
