#include "SdlCameraTexture.h"

#include "Camera/ICamera.h"
#include "TraceLog/Log.hpp"

#include <SDL.h>
#include <stdexcept>

using namespace Camera;
using namespace Sdl;
using namespace std;

SdlCameraTexture::SdlCameraTexture (SDL_Renderer* renderer)
    : m_renderer {renderer}
    , m_width {}
    , m_height {}
{
    if (!renderer)
    {
        throw invalid_argument ("CameraTexture: renderer is null");
    }
}

void SdlCameraTexture::update (const CameraFrame& frame)
{
    if (!frame.data || (frame.width <= 0) || (frame.height <= 0))
    {
        return;
    }

    if (!m_texture || (m_width != frame.width) || (m_height != frame.height))
    {
        recreate (frame.width, frame.height);
    }

    if (!m_texture)
    {
        return;
    }
    const int pitch = frame.width * frame.channels;

    if (SDL_UpdateTexture (m_texture.get (), nullptr, frame.data, pitch) != 0)
    {
        APP_WARN ("SDL_UpdateTexture failed: {}", SDL_GetError ());
    }
}

void* SdlCameraTexture::getImguiTextureId () const
{
    return static_cast<void*> (m_texture.get ());
}

int SdlCameraTexture::getWidth () const
{
    return m_width;
}

int SdlCameraTexture::getHeight () const
{
    return m_height;
}

bool SdlCameraTexture::isValid () const
{
    return m_texture != nullptr;
}

void SdlCameraTexture::recreate (int width, int height)
{
    m_texture.reset ();

    SDL_Texture* tex = SDL_CreateTexture (
        m_renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        width, height);

    if (!tex)
    {
        APP_WARN ("SDL_CreateTexture failed: {}", SDL_GetError ());
        return;
    }
    m_texture = SdlTexturePtr{tex};
    m_width = width;
    m_height = height;

    APP_INFO ("CameraTexture recreated: {}x{}", width, height);
}
