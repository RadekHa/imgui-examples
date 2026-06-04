#pragma once
#include <cstdint>

/** Forward class declaration of SDL window. */
typedef struct SDL_Window SDL_Window;

namespace App
{
    /** Class responsible for handling DPI scaling based on the display index. */
    class DpiHandler
    {
    public:
        /** Get the DPI scale factor for a specific display index. */
        static float getScale (int32_t displayIndex);
        /** Get the DPI scale factor for a given SDL window. */
        static float getScale (SDL_Window* window);
    };
}
