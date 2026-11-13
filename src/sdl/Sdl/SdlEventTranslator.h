#pragma once
#include "Events/Bus.h"

/** Forward class declaration of SDL event. */
typedef union SDL_Event SDL_Event;

namespace App
{
    /** Class responsible for processing SDL events and translating them into application events. */
    class SdlEventTranslator
    {
    public:
        /** Processes an array of SDL events and translates them into application events using the provided event bus. */
        void process (const SDL_Event* events, int count, EventBus& bus);

    private:
        /* Translates a single SDL event into an application event and publishes it to the event bus. */
        void translate (const SDL_Event& e, EventBus& bus);
    };
}
