#include "Application.hpp"
#include "Core/Log.hpp"
#include "Core/Window.hpp"

// #include <backends/imgui_impl_sdl2.h>
// #include <SDL2/SDL.h>
#include <tracy/Tracy.hpp>

using namespace App;
using namespace std;

Application::Application (const string& title)
    : m_exitStatus {ExitStatus::SUCCESS}
    , m_isRunning {true}
    , m_window (title)
    , m_renderer (m_window.native ())
    , m_imgui (m_window.native (), m_renderer.native ())
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

    EventBus::Subscription a = m_bus.subscribe<EventClose> (onStop);
    EventBus::Subscription b = m_bus.subscribe<EventQuit> (onStop);


    while (m_isRunning)
    {
        ZoneScopedN ("MainLoop");
        m_window.pollEvents (m_bus);

        m_renderer.beginFrame ();

        m_imgui.begin ();
        m_ui.update (m_model);
        m_imgui.end ();

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

// void Application::pollEvents ()
// {
//    // SDL_Event event{};
//
//    // while (SDL_PollEvent (&event) == 1)
//    // {
//    //    ZoneScopedN ("EventPolling");
//
//    //    ImGui_ImplSDL2_ProcessEvent (&event);
//
//    //    if (event.type == SDL_QUIT)
//    //    {
//    //        stop ();
//    //    }
//
//    //    if ((event.type == SDL_WINDOWEVENT) &&
//    //        (event.window.windowID == SDL_GetWindowID (m_window->getNativeWindow ())))
//    //    {
//    //        m_window->onEvent (event.window);
//    //    }
//    // }
// }
