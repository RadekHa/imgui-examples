#pragma once
#include "EventBus.h"
#include "SDL_events.h"

#include <vector>

namespace App
{
    /** Class responsible for processing SDL events and translating them into application events. */
    class InputSystem
    {
    public:
        /** Processes a vector of SDL events and translates them into application events using the provided event bus. */
        void process (const std::vector<SDL_Event>& events, EventBus& bus);

    private:
        /* Translates a single SDL event into an application event and publishes it to the event bus. */
        void translate (const SDL_Event& e, EventBus& bus);
    };
}
