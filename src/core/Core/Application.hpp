#pragma once
#include "Camera/ICamera.h"
#include "Events/Bus.h"
#include "Sdl/IPathService.h"
#include "Types/DataModel.h"
#include "Ui/IAppUi.h"

#include <memory>
#include <string>

namespace App
{
    /** Forward declarations of Sdl module classes. */
    class SdlWindow;
    class SdlRenderer;
    class ImGuiPass;
    class SdlEventTranslator;

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
        /** Refreshes the list of available cameras from the current device list. */
        void refreshCameraList ();

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
        std::unique_ptr<SdlWindow> m_window;
        /** The renderer for drawing to the window. */
        std::unique_ptr<SdlRenderer> m_renderer;
        /** The ImGui pass for rendering the UI. */
        std::unique_ptr<ImGuiPass> m_imgui;
        /** The data model representing the state of the application. */
        DataModel m_model;
        /** The SDL event translator for processing SDL events and translating them into application events. */
        std::unique_ptr<SdlEventTranslator> m_eventTranslator;
        /** The user interface for the application. */
        std::unique_ptr<Ui::IAppUi> m_ui;
        /** Currently open camera; nullptr when no camera is open or open attempts have failed. */
        std::unique_ptr<Camera::ICamera> m_camera;
        /** Positional index of the currently open camera device. */
        int m_cameraIndex;
    };
}
