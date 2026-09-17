#pragma once
#include <cstdint>

/** Forward class declaration of SDL window. */
typedef struct SDL_Window SDL_Window;

namespace App::dpi
{
    /** Get the DPI scale factor for a given SDL window. */
    float getScale (SDL_Window* window);
}
