#pragma once
#include "AppUi.h"
#include "DataModel.h"
#include "ImGuiPass.h"
#include "InputSystem.h"
#include "IPathService.h"
#include "Renderer.h"
#include "Window.hpp"

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
        explicit Application (const std::string& title, const IPathService* paths);
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

    private:
        /** Exit status of the application. */
        ExitStatus m_exitStatus;
        /** Flag indicating whether the main application loop is running. */
        bool m_isRunning;

        EventBus m_bus;

        Window m_window;
        Renderer m_renderer;
        ImGuiPass m_imgui;
        DataModel m_model;
        InputSystem m_input;

        AppUi m_ui;
    };
}
