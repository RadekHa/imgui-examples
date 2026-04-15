#pragma once
#include "EventBus.h"
#include "SDL_events.h"

#include <vector>

namespace App
{
    class InputSystem
    {
    public:
        void process (const std::vector<SDL_Event>& events, EventBus& bus);

    private:
        void translate (const SDL_Event& e, EventBus& bus);
    };
}
