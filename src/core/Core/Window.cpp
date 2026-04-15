#include "Core/DPIHandler.hpp"
#include "Core/Log.hpp"
#include "Window.hpp"

#include <SDL2/SDL.h>

using namespace App;
using namespace std;

Window::Window (const string& title)
{
    SDL_SetHint (SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");

    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        throw runtime_error (SDL_GetError ());
    }
    const auto flags = static_cast<SDL_WindowFlags> (SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    Window::Settings settings;
    const WindowSize size{DPIHandler::get_dpi_aware_window_size (settings)};

    m_window = SDL_CreateWindow (
        title.c_str (),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        size.width,
        size.height,
        flags);

    if (!m_window)
    {
        throw runtime_error (SDL_GetError ());
    }
}

Window::~Window ()
{
    SDL_DestroyWindow (m_window);
    SDL_Quit ();
}

SDL_Window* Window::native () const
{
    return m_window;
}

vector<SDL_Event> Window::pollEvents () const
{
    vector<SDL_Event> events;

    SDL_Event e;

    while (SDL_PollEvent (&e))
    {
        events.push_back (e);
    }
    return events;
}

//
// #include <backends/imgui_impl_sdl2.h>
// #include <backends/imgui_impl_sdlrenderer2.h>
//
// #include <imgui.h>
// #include <SDL2/SDL.h>
// #include <string>
// #include <tracy/Tracy.hpp>
//
// using namespace App;
// using namespace std;
//
// Window::Window (const Settings& settings)
//    : m_window {}
//    , m_renderer {}
//    , m_isMinimized {}
//    //, m_show_some_panel {true}
//    //, m_show_debug_panel {true}
//    //, m_show_demo_panel {true}
// {
//    ZoneScoped;
//
//    const auto window_flags{
//        static_cast<SDL_WindowFlags> (SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)
//    };
//
//    const WindowSize size{DPIHandler::get_dpi_aware_window_size (settings)};
//
//    m_window = SDL_CreateWindow (settings.title.c_str (),
//                                 SDL_WINDOWPOS_CENTERED,
//                                 SDL_WINDOWPOS_CENTERED,
//                                 size.width,
//                                 size.height,
//                                 window_flags);
//
//    Uint32 renderer_flags{SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED};
//    m_renderer = SDL_CreateRenderer (m_window, -1, renderer_flags);
//
//    if (m_renderer == nullptr)
//    {
//        APP_ERROR ("Error creating SDL_Renderer!");
//        return;
//    }
//    SDL_RendererInfo info;
//    SDL_GetRendererInfo (m_renderer, &info);
//
//    APP_DEBUG ("Current SDL_Renderer: {}", info.name);
//
//    // Setup Dear ImGui context
//    IMGUI_CHECKVERSION ();
//    ImGui::CreateContext ();
//    ImGuiIO& io{ImGui::GetIO ()};
//
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable |
//                      ImGuiConfigFlags_ViewportsEnable;
//
//    const string user_config_path{SDL_GetPrefPath (COMPANY_NAMESPACE.c_str (), APP_NAME.c_str ())};
//    APP_DEBUG ("User config path: {}", user_config_path);
//
//    // Absolute imgui.ini path to preserve settings independent of app location.
//    static const string imgui_ini_filename{user_config_path + "imgui.ini"};
//    io.IniFilename = imgui_ini_filename.c_str ();
//
//    // ImGUI font.
//    const float font_scaling_factor{DPIHandler::get_scale ()};
//    const float font_size{18.0F * font_scaling_factor};
//    const string font_path{Resources::font_path ("Manrope.ttf").generic_string ()};
//
//    io.Fonts->AddFontFromFileTTF (font_path.c_str (), font_size);
//    io.FontDefault = io.Fonts->AddFontFromFileTTF (font_path.c_str (), font_size);
//    DPIHandler::set_global_font_scaling (&io);
//
//    // Setup Platform/Renderer backends.
//    ImGui_ImplSDL2_InitForSDLRenderer (getNativeWindow (), getNativeRenderer ());
//    ImGui_ImplSDLRenderer2_Init (getNativeRenderer ());
// }
//
// Window::~Window ()
// {
//    ZoneScoped;
//
//    SDL_DestroyRenderer (m_renderer);
//    SDL_DestroyWindow (m_window);
//
//    ImGui_ImplSDLRenderer2_Shutdown ();
//    ImGui_ImplSDL2_Shutdown ();
//    ImGui::DestroyContext ();
// }
//
// void Window::update ()
// {
//    ImGuiIO& io{ImGui::GetIO ()};
//
//    // Start the Dear ImGui frame
//    ImGui_ImplSDLRenderer2_NewFrame ();
//    ImGui_ImplSDL2_NewFrame ();
//    ImGui::NewFrame ();
//
//    if (!m_isMinimized)
//    {
//        ImGui::DockSpaceOverViewport ();
//
//        if (ImGui::BeginMainMenuBar ())
//        {
//            if (ImGui::BeginMenu ("File"))
//            {
//                if (ImGui::MenuItem ("Exit", "Cmd+Q"))
//                {
//                    onClose ();
//                }
//                ImGui::EndMenu ();
//            }
//
//            if (ImGui::BeginMenu ("View"))
//            {
//                ImGui::MenuItem ("Some Panel", nullptr, &m_show_some_panel);
//                ImGui::MenuItem ("ImGui Demo Panel", nullptr, &m_show_demo_panel);
//                ImGui::MenuItem ("Debug Panels", nullptr, &m_show_debug_panel);
//                ImGui::EndMenu ();
//            }
//            ImGui::EndMainMenuBar ();
//        }
//
//        // Whatever GUI to implement here ...
//        if (m_show_some_panel)
//        {
//            ImGui::Begin ("Some panel", &m_show_some_panel);
//            ImGui::Text ("Hello World");
//            ImGui::End ();
//        }
//
//        // ImGUI demo panel
//        if (m_show_demo_panel)
//        {
//            ImGui::ShowDemoWindow (&m_show_demo_panel);
//        }
//
//        // Debug panel
//        if (m_show_debug_panel)
//        {
//            ImGui::ShowMetricsWindow ();
//            ImGui::ShowDebugLogWindow ();
//
//            ImGui::Begin ("App debug panel", &m_show_debug_panel);
//            // ImGui::Text ("User config path: %s", user_config_path.c_str ());
//            ImGui::Separator ();
//            // ImGui::Text ("Font path: %s", font_path.c_str ());
//            // ImGui::Text ("Font size: %f", font_size);
//            ImGui::Text ("Global font scaling %f", io.FontGlobalScale);
//            // ImGui::Text ("UI scaling factor: %f", font_scaling_factor);
//            ImGui::End ();
//        }
//        // -----------------------------------------------
//        static float values [90] = {0};
//        static int values_offset = 0;
//        static double refresh_time = 0.0;
//
//        if (ImGui::GetTime () > refresh_time)
//        {
//            values [values_offset] = ImGui::GetIO ().Framerate;
//
//            values_offset = (values_offset + 1) % IM_ARRAYSIZE (values);
//
//            refresh_time = ImGui::GetTime () + 0.1f;
//        }
//        float average = 0.0f;
//
//        for (int n = 0; n < IM_ARRAYSIZE (values); n++)
//        {
//            average += values [n];
//        }
//        average /= (float) IM_ARRAYSIZE (values);
//
//        char overlay [32];
//        sprintf_s (overlay, "AVG: %.2f FPS", average);
//
//        ImGui::Begin ("MOje okno");
//
//        ImGui::PlotLines ("##FPS_Graf",
//                          values,
//                          IM_ARRAYSIZE (values),
//                          values_offset,
//                          overlay,
//                          0.0f,
//                          120.0f,
//                          ImVec2 (0, 80.0f));
//        ImGui::PlotHistogram ("##FPS_Hist",
//                              values,
//                              IM_ARRAYSIZE (values),
//                              values_offset,
//                              overlay,
//                              0.0f,
//                              120.0f,
//                              ImVec2 (0, 80.0f));
//        ImGui::End ();
//
//        // -----------------------------------------------
//    }
//    // Rendering
//    ImGui::Render ();
//
//    SDL_RenderSetScale (getNativeRenderer (), io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
//    SDL_SetRenderDrawColor (getNativeRenderer (), 100, 100, 100, 255);
//    SDL_RenderClear (m_renderer);
//
//    ImGui_ImplSDLRenderer2_RenderDrawData (ImGui::GetDrawData (), m_renderer);
//    SDL_RenderPresent (m_renderer);
// }
//
// void Window::onEvent (const SDL_WindowEvent& event)
// {
//    ZoneScoped;
//
//    switch (event.event)
//    {
//    case SDL_WINDOWEVENT_CLOSE:
//        onClose ();
//        break;
//
//    case SDL_WINDOWEVENT_MINIMIZED:
//        onMinimize ();
//        break;
//
//    case SDL_WINDOWEVENT_SHOWN:
//        onShown ();
//        break;
//
//    default:
//        break;
//    }
// }
//
// SDL_Window* Window::getNativeWindow () const
// {
//    ZoneScoped;
//    return m_window;
// }
//
// SDL_Renderer* Window::getNativeRenderer () const
// {
//    ZoneScoped;
//    return m_renderer;
// }
//
// void Window::onMinimize ()
// {
//    m_isMinimized = true;
// }
//
// void Window::onShown ()
// {
//    m_isMinimized = false;
// }
//
// void Window::onClose ()
// {
//    SDL_Event window_close_event;
//    window_close_event.type = SDL_QUIT;
//    SDL_PushEvent (&window_close_event);
// }
