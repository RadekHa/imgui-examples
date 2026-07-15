#pragma once
#include "SdlWrapper.h"

struct SDL_Renderer;

namespace Camera
{
    /** Forward declaration of camera frame. */
    struct CameraFrame;
}

namespace Sdl
{
    class SdlCameraTexture
    {
    public:
        explicit SdlCameraTexture (SDL_Renderer* renderer);

        void update (const Camera::CameraFrame& frame);


        void* getImguiTextureId () const;

        int getWidth ()  const;

        int getHeight () const;

        bool isValid () const;

    private:
        void recreate (int width, int height);

        SDL_Renderer* m_renderer;
        SdlTexturePtr m_texture;
        int m_width{};
        int m_height{};
    };
}
