#pragma once

#include <SDL2/SDL.h>

#include <string>

namespace App
{
    /** Class representing the application window and its associated renderer. */
    class Window
    {
    public:
        /* Struct to hold the settings for window creation. */
        struct Settings
        {
            /* The title of the window. */
            std::string title;
            /* The width of the window in pixels. */
            int width{1280};
            /* The height of the window in pixels. */
            int height{720};
        };

        /** Initializes the window with the given settings. */
        explicit Window (const Settings& settings);
        /* Cleans up the window and renderer resources. */
        ~Window ();

        /** The copy constructor is deleted. */
        Window (const Window&) = delete;
        /** The move constructor is deleted. */
        Window (Window&&) = delete;
        /** The copy assignment operator is deleted. */
        Window& operator= (const Window&) = delete;
        /** The move assignment operator is deleted. */
        Window& operator= (Window&&) = delete;

        /* Returns a pointer to the native SDL window. */
        [ [nodiscard]] SDL_Window* getNativeWindow () const;
        /* Returns a pointer to the native SDL renderer associated with the window. */
        [ [nodiscard]] SDL_Renderer* getNativeRenderer () const;

    private:
        /* Pointer to the SDL window. */
        SDL_Window* m_window;
        /* Pointer to the SDL renderer associated with the window. */
        SDL_Renderer* m_renderer;
    };
}
