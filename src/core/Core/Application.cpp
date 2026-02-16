#include "Application.hpp"

#include <SDL2/SDL.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <imgui.h>
#include <tracy/Tracy.hpp>

#include <memory>
#include <string>

#include "Core/DPIHandler.hpp"
#include "Core/Log.hpp"
#include "Core/Resources.hpp"
#include "Core/Window.hpp"
#include "Settings/Project.hpp"

namespace App
{
    Application::Application (const std::string& title)
    {
        ZoneScoped;

        const unsigned int init_flags{SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER};

        if (SDL_Init (init_flags) != 0)
        {
            APP_ERROR ("Error: %s\n", SDL_GetError ());
            m_exit_status = ExitStatus::FAILURE;
        }
        m_window = std::make_unique<Window> (Window::Settings{title});
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

        if (m_exit_status == ExitStatus::FAILURE)
        {
            return m_exit_status;
        }
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION ();
        ImGui::CreateContext ();
        ImGuiIO& io{ImGui::GetIO ()};

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable |
                          ImGuiConfigFlags_ViewportsEnable;

        const std::string user_config_path{SDL_GetPrefPath (COMPANY_NAMESPACE.c_str (), APP_NAME.c_str ())};
        APP_DEBUG ("User config path: {}", user_config_path);

        // Absolute imgui.ini path to preserve settings independent of app location.
        static const std::string imgui_ini_filename{user_config_path + "imgui.ini"};
        io.IniFilename = imgui_ini_filename.c_str ();

        // ImGUI font
        const float font_scaling_factor{DPIHandler::get_scale ()};
        const float font_size{18.0F * font_scaling_factor};
        const std::string font_path{Resources::font_path ("Manrope.ttf").generic_string ()};

        io.Fonts->AddFontFromFileTTF (font_path.c_str (), font_size);
        io.FontDefault = io.Fonts->AddFontFromFileTTF (font_path.c_str (), font_size);
        DPIHandler::set_global_font_scaling (&io);

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForSDLRenderer (m_window->get_native_window (), m_window->get_native_renderer ());
        ImGui_ImplSDLRenderer2_Init (m_window->get_native_renderer ());

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
                    (event.window.windowID == SDL_GetWindowID (m_window->get_native_window ())))
                {
                    on_event (event.window);
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

            SDL_RenderSetScale (m_window->get_native_renderer (),
                                io.DisplayFramebufferScale.x,
                                io.DisplayFramebufferScale.y);
            SDL_SetRenderDrawColor (m_window->get_native_renderer (), 100, 100, 100, 255);
            SDL_RenderClear (m_window->get_native_renderer ());
            ImGui_ImplSDLRenderer2_RenderDrawData (ImGui::GetDrawData (), m_window->get_native_renderer ());
            SDL_RenderPresent (m_window->get_native_renderer ());
        }
        return m_exit_status;
    }

    void Application::stop ()
    {
        ZoneScoped;

        m_running = false;
    }

    void Application::on_event (const SDL_WindowEvent& event)
    {
        ZoneScoped;

        switch (event.event)
        {
        case SDL_WINDOWEVENT_CLOSE:
            return on_close ();

        case SDL_WINDOWEVENT_MINIMIZED:
            return on_minimize ();

        case SDL_WINDOWEVENT_SHOWN:
            return on_shown ();

        default:
            // Do nothing otherwise
            return;
        }
    }

    void Application::on_minimize ()
    {
        ZoneScoped;

        m_minimized = true;
    }

    void Application::on_shown ()
    {
        ZoneScoped;

        m_minimized = false;
    }

    void Application::on_close ()
    {
        ZoneScoped;

        stop ();
    }
}  // namespace App
