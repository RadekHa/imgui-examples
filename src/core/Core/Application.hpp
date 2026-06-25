#pragma once
#include "AppUi.h"
#include "Camera/ICamera.h"
#include "DataModel.h"
#include "EventBus.h"
#include "ImGuiPass.h"
#include "IPathService.h"
#include "SdlEventTranslator.h"
#include "SdlRenderer.h"
#include "SdlWindow.h"

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

    private:
        /** Stops the application. */
        void stop ();
        /** Initializes the application, setting up necessary resources and state. */
        void init ();

        /** Exit status of the application. */
        ExitStatus m_exitStatus;
        /** Flag indicating whether the main application loop is running. */
        bool m_isRunning;
        /** Flag indicating whether the application window is currently minimized. */
        bool m_isMinimized;

        /** The event bus for handling application events. */
        EventBus m_bus;
        /** Vector of subscriptions to application events, allowing for automatic unsubscription when the Application object is destroyed. */
        std::vector<EventBus::Subscription> m_subscriptions;
        /** The main application window. */
        SdlWindow m_window;
        /** The renderer for drawing to the window. */
        SdlRenderer m_renderer;
        /** The ImGui pass for rendering the UI. */
        ImGuiPass m_imgui;
        /** The data model representing the state of the application. */
        DataModel m_model;
        /** The SDL event translator for processing SDL events and translating them into application events. */
        SdlEventTranslator m_eventTranslator;
        /** The user interface for the application. */
        Ui::AppUi m_ui;

        std::unique_ptr<Camera::ICamera> m_camera;
    };
}
