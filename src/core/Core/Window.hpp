#pragma once
#include "SdlWrapper.h"

#include <SDL_events.h>

#include <string>
#include <vector>

/** Forward class declaration of SDL window. */
typedef struct SDL_Window SDL_Window;

namespace App
{
    /** Class representing the application window. */
    class Window
    {
    public:
        /** Initializes the window with a given title. */
        explicit Window (const std::string& title);
        /** Cleaning up resources. */
        ~Window ();

        /** Get the display index of the window. */
        int getWindowDisplayIndex () const;

        /** Returns the native SDL window pointer. */
        SDL_Window* native () const;
        /* Polls and returns a vector of SDL events. */
        void pollEvents (std::vector<SDL_Event>& events) const;
        /* Resizes the window based on the provided scale factor. */
        void resize (float scale);
    private:
        /** The native SDL window pointer. */
        sdl::SdlWindowPtr m_window;
        /** The current scale factor applied to the window. */
        float m_scale;
    };
}
