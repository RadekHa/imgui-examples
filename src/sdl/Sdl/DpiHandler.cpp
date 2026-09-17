#include "DpiHandler.h"
#include "TraceLog/Log.hpp"

#include <SDL3/SDL.h>

float App::dpi::getScale (SDL_Window* window)
{
    float scale = SDL_GetWindowDisplayScale (window);

    if (scale <= 0.0f)
    {
        return 1.0f;
    }
    return scale;
}
