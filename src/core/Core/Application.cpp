#include "Application.hpp"
#include "DpiHandler.h"
#include "FrameContext.h"
#include "SdlCameraTexture.h"
#include "TraceLog/Log.hpp"
#include "TraceLog/Tracy.hpp"

using namespace App;
using namespace Camera;
using namespace Sdl;
using namespace std;

Application::Application (const string& title, const IPathService* paths)
    : m_exitStatus {ExitStatus::SUCCESS}
    , m_isRunning {true}
    , m_isMinimized {}
    , m_window (title)
    , m_renderer (m_window.native (), paths)
    , m_imgui (m_window.native (), m_renderer.native (), paths)
    , m_camera {createCamera (0)}
{
    ZoneScoped;

    init ();

    char status = m_model.serial.openDevice ("COM4", 9600);

    if (status != 1)
    {
        APP_ERROR ("Cannot open COM port");
    }
}

Application::~Application ()
{
    ZoneScoped;

    m_model.serial.closeDevice ();
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

    CameraFrame frame;
    SdlCameraTexture camTexture{m_renderer.native ()};

    MatchResult matchResult;

    while (m_isRunning)
    {
        ZoneScopedN ("MainLoop");

        if (!matchResult.found && m_camera && m_camera->read (frame, matchResult))
        {
            camTexture.update (frame);

            if (matchResult.found)
            {
                m_model.isSmile = matchResult.found;
            }
        }
        FrameContext frameContext;

        m_window.pollEvents (events);
        m_eventTranslator.process (events, m_bus);

        m_imgui.beginFrame (events);
        m_imgui.fillFrameContext (frameContext);

        m_renderer.beginFrame (frameContext);

        if (!m_isMinimized)
        {
            m_ui.update (m_model, &camTexture);
            m_renderer.update (m_model);
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

        float scale = dpi::getScale (e.displayIndex);
        m_window.resize (scale);
        m_imgui.rebuildFonts (scale);
    }));
}
