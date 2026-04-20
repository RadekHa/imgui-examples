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
    , m_window (title)
    , m_renderer (m_window.native ())
    , m_imgui (m_window.native (), m_renderer.native (), paths)
{
    ZoneScoped;
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

    auto onStop = [this] (const auto&) {
                      stop ();
                  };

    auto subClose = m_bus.subscribe<EventClose> (onStop);
    auto subStop = m_bus.subscribe<EventQuit> (onStop);


    while (m_isRunning)
    {
        ZoneScopedN ("MainLoop");
        FrameContext frameContext;

        auto events = m_window.pollEvents ();
        m_input.process (events, m_bus);

        m_imgui.beginFrame ();
        m_imgui.fillFrameContext (frameContext);

        m_renderer.beginFrame (frameContext);

        m_ui.update (m_model);

        m_imgui.endFrame ();
        m_renderer.endFrame ();

        m_bus.dispatch ();
    }
    return ExitStatus::SUCCESS;
}

void Application::stop ()
{
    ZoneScoped;
    m_isRunning = false;
}
