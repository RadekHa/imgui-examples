#include "Core/DPIHandler.hpp"
#include "Core/Window.hpp"

#include <imgui.h>
#include <SDL2/SDL.h>
#include <tracy/Tracy.hpp>

#include <cmath>

namespace App
{
    float DPIHandler::get_scale ()
    {
        ZoneScoped;

        constexpr int display_index{0};
        // @todo: This should be 72.0F on Mac, but it seems like it is not. I'm not
        //  sure why, but this works ¯\_(ツ)_/¯
        const float default_dpi{96.0F};
        float dpi{default_dpi};

        SDL_GetDisplayDPI (display_index, nullptr, &dpi, nullptr);

        return std::floor (dpi / default_dpi);
    }

    WindowSize DPIHandler::get_dpi_aware_window_size (const Window::Settings& settings)
    {
        ZoneScoped;

        return {
                   settings.width, settings.height
        };
    }

    void DPIHandler::set_global_font_scaling (ImGuiIO* io)
    {
        ZoneScoped;

        io->FontGlobalScale = 1.0F / get_scale ();
    }
}  // namespace App
