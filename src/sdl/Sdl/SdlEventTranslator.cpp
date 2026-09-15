#include "SdlEventTranslator.h"

#include <SDL3/SDL_events.h>

using namespace App;
using namespace std;

void SdlEventTranslator::process (const vector<SDL_Event>& events, EventBus& bus)
{
    for (const auto& e : events)
    {
        translate (e, bus);
    }
}

void SdlEventTranslator::translate (const SDL_Event& e, EventBus& bus)
{
    switch (e.type)
    {
    case SDL_EVENT_QUIT:
        bus.publish (EventQuit{});
        break;

    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        bus.publish (EventClose{});
        break;

    case SDL_EVENT_WINDOW_MINIMIZED:
        bus.publish (EventMinimized{});
        break;

    case SDL_EVENT_WINDOW_SHOWN:
        bus.publish (EventShown{});
        break;

    case SDL_EVENT_WINDOW_RESTORED:
        bus.publish (EventRestored{});
        break;

    case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
        bus.publish (EventDisplayChanged{.displayIndex = e.window.data1});
        break;

    case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
        SDL_Window* window = SDL_GetWindowFromID (e.window.windowID);

        if (window)
        {
            float scale = SDL_GetWindowDisplayScale (window);
            bus.publish (EventScaleChanged{.scale = scale});
        }
        break;
    }
}
