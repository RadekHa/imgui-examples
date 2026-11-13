#include "Application.hpp"
#include "Camera/ICamera.h"
#include "Sdl/FrameContext.h"
#include "Sdl/SdlCameraTexture.h"
#include "TraceLog/Log.hpp"
#include "TraceLog/Tracy.hpp"
#include "Ui/IAppUi.h"

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
    , m_ui {Ui::createAppUi ()}
    , m_cameraIndex {-1}
{
    ZoneScoped;
    init ();
    refreshCameraList ();
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

    CameraFrame frame;
    SdlCameraTexture camTexture{m_renderer.native ()};
    bool hasPreview{false};

    while (m_isRunning)
    {
        ZoneScopedN ("MainLoop");

        if (!m_camera || (m_model.selectedCameraIndex != m_cameraIndex))
        {
            m_camera = createCamera (m_model.selectedCameraIndex);
            m_cameraIndex = m_model.selectedCameraIndex;
            hasPreview = false;
            m_model.camera.isValid = false;

            if (m_camera)
            {
                APP_INFO ("Camera opened: index {}", m_cameraIndex);
            }
        }

        if (m_camera && m_camera->read (frame))
        {
            camTexture.update (frame);
            hasPreview = true;
        }
        FrameContext frameContext;

        m_window.pollEvents (events);
        m_eventTranslator.process (events, m_bus);

        m_imgui.beginFrame (events);
        m_imgui.fillFrameContext (frameContext);

        m_renderer.beginFrame (frameContext);

        if (!m_isMinimized && hasPreview)
        {
            m_model.camera = Ui::ImageInfo {
                .textureId = camTexture.getImguiTextureId (),
                .width = camTexture.getWidth (),
                .height = camTexture.getHeight (),
                .isValid = camTexture.isValid ()
            };

            m_ui->update (m_model);
            m_renderer.update (m_model);
        }
        m_imgui.endFrame ();
        m_renderer.endFrame ();
        // Dispatch events after processing the frame.
        m_bus.dispatch ();
    }
    return ExitStatus::SUCCESS;
}

void Application::refreshCameraList ()
{
    m_model.cameras = Camera::enumerateCameras ();

    if (!m_model.cameras.empty () && (m_model.selectedCameraIndex >= static_cast<int> (m_model.cameras.size ())))
    {
        m_model.selectedCameraIndex = 0;
    }
    APP_INFO ("Camera list refreshed: {} camera(s), selected index {}",
              m_model.cameras.size (),
              m_model.selectedCameraIndex);
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

    m_subscriptions.emplace_back (m_bus.subscribe<EventScaleChanged> ( [this] (const auto& e) {
        APP_INFO ("Scale changed: {}", e.scale);
        m_window.resize (e.scale);
        m_imgui.rebuildFonts (e.scale);
    }));
}
