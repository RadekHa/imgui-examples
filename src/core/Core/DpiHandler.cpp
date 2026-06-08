#include "DpiHandler.h"
#include "Log.hpp"

#include <SDL2/SDL.h>

float App::dpi::getScale (int32_t displayIndex)
{
    constexpr float default_dpi{96.0f};
    float dpi{default_dpi};

    if (SDL_GetDisplayDPI (displayIndex, nullptr, &dpi, nullptr) != 0)
    {
        APP_WARN ("SDL_GetDisplayDPI failed: {}", SDL_GetError ());
        return 1.0f;
    }

    if (dpi <= 0.0f)
    {
        return 1.0f;
    }
    return dpi / default_dpi;
}

float App::dpi::getScale (SDL_Window* window)
{
    int displayIndex = SDL_GetWindowDisplayIndex (window);

    if (displayIndex < 0)
    {
        APP_WARN ("SDL_GetWindowDisplayIndex failed: {}", SDL_GetError ());
        return 1.0f;
    }
    return getScale (displayIndex);
}
