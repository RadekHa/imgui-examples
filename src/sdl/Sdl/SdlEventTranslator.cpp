#include "SdlEventTranslator.h"

#include <SDL_events.h>

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
    case SDL_QUIT:
        bus.publish (EventQuit{});
        break;

    case SDL_WINDOWEVENT:

        switch (e.window.event)
        {
        case SDL_WINDOWEVENT_CLOSE:
            bus.publish (EventClose{});
            break;

        case SDL_WINDOWEVENT_MINIMIZED:
            bus.publish (EventMinimized{});
            break;

        case SDL_WINDOWEVENT_SHOWN:
            bus.publish (EventShown{});
            break;

        case SDL_WINDOWEVENT_RESTORED:
            bus.publish (EventRestored{});
            break;

        case SDL_WINDOWEVENT_DISPLAY_CHANGED:
            bus.publish (EventDisplayChanged{.displayIndex = e.window.data1});
            break;
        }
    }
}
