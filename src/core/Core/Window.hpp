#pragma once
#include "SDL_events.h"

#include <string>
#include <vector>

/** Forward class declaration of SDL window. */
typedef struct SDL_Window SDL_Window;

namespace App
{
    /* Class representing the application window. */
    class Window
    {
    public:

        /* Struct representing the settings for creating a window. */
        struct Settings
        {
            /* The title of the window. */
            std::string title;
            /* The width of the window in pixels. */
            int width{1280};
            /* The height of the window in pixels. */
            int height{720};
        };

        /* Initializes the window with a given title. */
        explicit Window (const std::string& title);
        /* Cleaning up resources. */
        ~Window ();

        /** Returns the native SDL window pointer. */
        SDL_Window* native () const;
        /* Polls and returns a vector of SDL events. */
        std::vector<SDL_Event> pollEvents () const;

    private:
        /* The native SDL window pointer. */
        SDL_Window* m_window;
    };
}
