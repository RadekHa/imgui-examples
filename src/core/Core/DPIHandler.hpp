#pragma once
#include "Core/Window.hpp"

#include <imgui.h>
#include <SDL2/SDL.h>

namespace App
{
    struct WindowSize
    {
        int width;
        int height;
    };

    class DPIHandler
    {
    public:
        [ [nodiscard]] static float get_scale ();

        [ [nodiscard]] static WindowSize get_dpi_aware_window_size (const Window::Settings& settings);

        static void set_render_scale (SDL_Renderer* renderer);
        static void set_global_font_scaling (ImGuiIO* io);
    };
}
