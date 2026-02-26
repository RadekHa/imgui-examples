#pragma once
#include "Core/Window.hpp"

#include <SDL2/SDL.h>

#include <memory>
#include <string>

namespace App
{
    /** Enum representing the exit status of the application. */
    enum class ExitStatus : int
    {
        SUCCESS = 0,
        FAILURE = 1
    };

    /** Main application class. */
    class Application
    {
    public:
        /** Initializes the application with a given title. */
        explicit Application (const std::string& title);
        /* Cleaning up resources. */
        ~Application ();

        /** The copy constructor is deleted. */
        Application (const Application&) = delete;
        /** The move constructor is deleted. */
        Application (Application&&) = delete;
        /** The copy assignment operator is deleted. */
        Application& operator= (Application) = delete;
        /** The move assignment operator is deleted. */
        Application& operator= (Application&&) = delete;

        /** Runs the main application loop and returns the exit status. */
        ExitStatus run ();

        /** Stops the application. */
        void stop ();

        /** Handle SDL events. */
        void onEvent (const SDL_WindowEvent& event);
        /** Handle window minimize event. */
        void onMinimize ();
        /** Handle window restore event. */
        void onShown ();
        /** Handle window close event. */
        void onClose ();

    private:
        /** Exit status of the application. */
        ExitStatus m_exitStatus;
        /** Unique pointer to the application window. */
        std::unique_ptr<Window> m_window{nullptr};

        bool m_running;
        bool m_minimized;
        bool m_show_some_panel;
        bool m_show_debug_panel;
        bool m_show_demo_panel;
    };
}
