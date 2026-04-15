#pragma once
#include "SDL_events.h"

#include <string>
#include <vector>

/** Forward class declaration of SDL window. */
typedef struct SDL_Window SDL_Window;

namespace App
{
    class Window
    {
    public:

        struct Settings
        {
            /* The title of the window. */
            std::string title;
            /* The width of the window in pixels. */
            int width{1280};
            /* The height of the window in pixels. */
            int height{720};
        };

        explicit Window (const std::string& title);

        ~Window ();

        SDL_Window* native () const;

        std::vector<SDL_Event> pollEvents () const;

    private:

        SDL_Window* m_window;
    };
}
