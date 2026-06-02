#include "Application.hpp"
#include "FrameContext.h"
#include "Log.hpp"
#include "Window.hpp"

#include <tracy/Tracy.hpp>

using namespace App;
using namespace std;

Application::Application (const string& title, const IPathService* paths)
    : m_exitStatus {ExitStatus::SUCCESS}
    , m_isRunning {true}
    , m_isMinimized {}
    , m_window (title)
    , m_renderer (m_window.native ())
    , m_imgui (m_window.native (), m_renderer.native (), paths)
{
    ZoneScoped;
    init ();
}

Application::~Application ()
{
    ZoneScoped;
}

ExitStatus Application::run ()
{
    ZoneScoped;

    if (m_exitStatus != ExitStatus::SUCCESS)
    {
        return m_exitStatus;
    }
    m_isRunning = true;

    vector<SDL_Event> events;

    while (m_isRunning)
    {
        ZoneScopedN ("MainLoop");
        FrameContext frameContext;

        m_window.pollEvents (events);
        m_input.process (events, m_bus);

        m_imgui.beginFrame ();
        m_imgui.fillFrameContext (frameContext);

        m_renderer.beginFrame (frameContext);

        if (!m_isMinimized)
        {
            m_ui.update (m_model);
        }
        m_imgui.endFrame ();
        m_renderer.endFrame ();
        // Dispatch events after processing the frame.
        m_bus.dispatch ();
    }
    return ExitStatus::SUCCESS;
}

void Application::stop ()
{
    ZoneScoped;
    m_isRunning = false;
}

void Application::init ()
{
    m_subscriptions.emplace_back (m_bus.subscribe<EventClose> (this, &Application::stop));
    m_subscriptions.emplace_back (m_bus.subscribe<EventQuit> (this, &Application::stop));
    m_subscriptions.emplace_back (m_bus.subscribe<EventMinimized> ( [this] (const auto&) {
        m_isMinimized = true;
    }));
    m_subscriptions.emplace_back (m_bus.subscribe<EventRestored> ( [this] (const auto&) {
        m_isMinimized = false;
    }));

    m_subscriptions.emplace_back (m_bus.subscribe<EventDisplayChanged> ( [this] (const auto& e) {
        APP_INFO ("Display changed: {}", e.displayIndex);
    }));
}
