#include "Application.hpp"
#include "Core/DPIHandler.hpp"
#include "Core/Log.hpp"
#include "Core/Resources.hpp"
#include "Core/Window.hpp"
#include "Settings/Project.hpp"

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <tracy/Tracy.hpp>


using namespace App;
using namespace std;


Application::Application (const string& title)
    : m_exitStatus {ExitStatus::SUCCESS}
    , m_running {true}
    , m_minimized {}
    , m_show_some_panel {true}
    , m_show_debug_panel {}
    , m_show_demo_panel {}
{
    ZoneScoped;

    const unsigned int init_flags{SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER};

    if (SDL_Init (init_flags) != 0)
    {
        APP_ERROR ("Error: %s\n", SDL_GetError ());
        m_exitStatus = ExitStatus::FAILURE;
    }
    m_window = make_unique<Window> (Window::Settings{title});
}

Application::~Application ()
{
    ZoneScoped;

    ImGui_ImplSDLRenderer2_Shutdown ();
    ImGui_ImplSDL2_Shutdown ();
    ImGui::DestroyContext ();

    SDL_Quit ();
}

ExitStatus Application::run ()
{
    ZoneScoped;

    if (m_exitStatus == ExitStatus::FAILURE)
    {
        return m_exitStatus;
    }
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION ();
    ImGui::CreateContext ();
    ImGuiIO& io{ImGui::GetIO ()};

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable |
                      ImGuiConfigFlags_ViewportsEnable;

    const string user_config_path{SDL_GetPrefPath (COMPANY_NAMESPACE.c_str (), APP_NAME.c_str ())};
    APP_DEBUG ("User config path: {}", user_config_path);

    // Absolute imgui.ini path to preserve settings independent of app location.
    static const string imgui_ini_filename{user_config_path + "imgui.ini"};
    io.IniFilename = imgui_ini_filename.c_str ();

    // ImGUI font
    const float font_scaling_factor{DPIHandler::get_scale ()};
    const float font_size{18.0F * font_scaling_factor};
    const string font_path{Resources::font_path ("Manrope.ttf").generic_string ()};

    io.Fonts->AddFontFromFileTTF (font_path.c_str (), font_size);
    io.FontDefault = io.Fonts->AddFontFromFileTTF (font_path.c_str (), font_size);
    DPIHandler::set_global_font_scaling (&io);

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer (m_window->getNativeWindow (), m_window->getNativeRenderer ());
    ImGui_ImplSDLRenderer2_Init (m_window->getNativeRenderer ());

    m_running = true;

    while (m_running)
    {
        ZoneScopedN ("MainLoop");

        SDL_Event event{};

        while (SDL_PollEvent (&event) == 1)
        {
            ZoneScopedN ("EventPolling");

            ImGui_ImplSDL2_ProcessEvent (&event);

            if (event.type == SDL_QUIT)
            {
                stop ();
            }

            if ((event.type == SDL_WINDOWEVENT) &&
                (event.window.windowID == SDL_GetWindowID (m_window->getNativeWindow ())))
            {
                onEvent (event.window);
            }
        }
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame ();
        ImGui_ImplSDL2_NewFrame ();
        ImGui::NewFrame ();

        if (!m_minimized)
        {
            ImGui::DockSpaceOverViewport ();

            if (ImGui::BeginMainMenuBar ())
            {
                if (ImGui::BeginMenu ("File"))
                {
                    if (ImGui::MenuItem ("Exit", "Cmd+Q"))
                    {
                        stop ();
                    }
                    ImGui::EndMenu ();
                }

                if (ImGui::BeginMenu ("View"))
                {
                    ImGui::MenuItem ("Some Panel", nullptr, &m_show_some_panel);
                    ImGui::MenuItem ("ImGui Demo Panel", nullptr, &m_show_demo_panel);
                    ImGui::MenuItem ("Debug Panels", nullptr, &m_show_debug_panel);
                    ImGui::EndMenu ();
                }
                ImGui::EndMainMenuBar ();
            }

            // Whatever GUI to implement here ...
            if (m_show_some_panel)
            {
                ImGui::Begin ("Some panel", &m_show_some_panel);
                ImGui::Text ("Hello World");
                ImGui::End ();
            }

            // ImGUI demo panel
            if (m_show_demo_panel)
            {
                ImGui::ShowDemoWindow (&m_show_demo_panel);
            }

            // Debug panel
            if (m_show_debug_panel)
            {
                ImGui::ShowMetricsWindow ();
                ImGui::ShowDebugLogWindow ();

                ImGui::Begin ("App debug panel", &m_show_debug_panel);
                ImGui::Text ("User config path: %s", user_config_path.c_str ());
                ImGui::Separator ();
                ImGui::Text ("Font path: %s", font_path.c_str ());
                ImGui::Text ("Font size: %f", font_size);
                ImGui::Text ("Global font scaling %f", io.FontGlobalScale);
                ImGui::Text ("UI scaling factor: %f", font_scaling_factor);
                ImGui::End ();
            }
            // -----------------------------------------------
            static float values [90] = {0};
            static int values_offset = 0;
            static double refresh_time = 0.0;

            if (ImGui::GetTime () > refresh_time)
            {
                values [values_offset] = ImGui::GetIO ().Framerate;

                values_offset = (values_offset + 1) % IM_ARRAYSIZE (values);

                refresh_time = ImGui::GetTime () + 0.1f;
            }
            float average = 0.0f;

            for (int n = 0; n < IM_ARRAYSIZE (values); n++)
            {
                average += values [n];
            }
            average /= (float) IM_ARRAYSIZE (values);

            char overlay [32];
            sprintf_s (overlay, "AVG: %.2f FPS", average);

            ImGui::Begin ("MOje okno");

            ImGui::PlotLines ("##FPS_Graf",
                              values,
                              IM_ARRAYSIZE (values),
                              values_offset,
                              overlay,
                              0.0f,
                              120.0f,
                              ImVec2 (0, 80.0f));
            ImGui::PlotHistogram ("##FPS_Hist",
                                  values,
                                  IM_ARRAYSIZE (values),
                                  values_offset,
                                  overlay,
                                  0.0f,
                                  120.0f,
                                  ImVec2 (0, 80.0f));
            ImGui::End ();

            // -----------------------------------------------
        }
        // Rendering
        ImGui::Render ();

        SDL_RenderSetScale (m_window->getNativeRenderer (),
                            io.DisplayFramebufferScale.x,
                            io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor (m_window->getNativeRenderer (), 100, 100, 100, 255);
        SDL_RenderClear (m_window->getNativeRenderer ());
        ImGui_ImplSDLRenderer2_RenderDrawData (ImGui::GetDrawData (), m_window->getNativeRenderer ());
        SDL_RenderPresent (m_window->getNativeRenderer ());
    }
    return m_exitStatus;
}

void Application::stop ()
{
    ZoneScoped;
    m_running = false;
}

void Application::onEvent (const SDL_WindowEvent& event)
{
    ZoneScoped;

    switch (event.event)
    {
    case SDL_WINDOWEVENT_CLOSE:
        onClose ();
        break;

    case SDL_WINDOWEVENT_MINIMIZED:
        onMinimize ();
        break;

    case SDL_WINDOWEVENT_SHOWN:
        onShown ();
        break;

    default:
        break;
    }
}

void Application::onMinimize ()
{
    ZoneScoped;
    m_minimized = true;
}

void Application::onShown ()
{
    ZoneScoped;
    m_minimized = false;
}

void Application::onClose ()
{
    ZoneScoped;
    stop ();
}
