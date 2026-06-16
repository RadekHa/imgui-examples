#include "DpiHandler.h"
#include "SdlWindow.h"
#include "TraceLog/Log.hpp"

using namespace App;
using namespace sdl;
using namespace std;


SdlWindow::SdlWindow (const string& title)
    : m_scale {1.0}
{
    const auto flags = static_cast<SDL_WindowFlags> (SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    constexpr int width{1280};
    constexpr int height{720};

    m_window = SdlWindowPtr{
        SDL_CreateWindow (
            title.c_str (),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            flags)
    };

    if (!m_window)
    {
        throw runtime_error (SDL_GetError ());
    }
    // Set the initial window size based on the DPI scale factor of the display.
    const float scale = dpi::getScale (m_window.get ());
    resize (scale);

    SDL_SetWindowPosition (m_window.get (), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

SdlWindow::~SdlWindow () = default;

int SdlWindow::getWindowDisplayIndex () const
{
    return SDL_GetWindowDisplayIndex (m_window.get ());
}

SDL_Window* SdlWindow::native () const
{
    return m_window.get ();
}

void SdlWindow::pollEvents (vector<SDL_Event>& events) const
{
    events.clear ();

    SDL_Event e;

    while (SDL_PollEvent (&e))
    {
        events.push_back (e);
    }
}

void SdlWindow::resize (float scale)
{
    if (scale <= 0.0f)
    {
        APP_WARN ("Invalid scale: {}", scale);
        return;
    }

    if (m_scale != scale)
    {
        int width, height;
        SDL_GetWindowSize (m_window.get (), &width, &height);

        int newWidth = static_cast<int> (width * (scale / m_scale));
        int newHeight = static_cast<int> (height * (scale / m_scale));

        APP_INFO ("Scale update: {} -> {}. Resizing window to {}x{}",
                  m_scale, scale, newWidth, newHeight);

        m_scale = scale;
        SDL_SetWindowSize (m_window.get (), newWidth, newHeight);
    }
}
