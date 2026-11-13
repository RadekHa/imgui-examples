#pragma once
#include "SdlWrapper.h"

struct SDL_Renderer;

/** Forward declaration of camera frame. */
namespace Camera
{
    struct CameraFrame;
}

namespace Sdl
{
    class SdlCameraTexture
    {
    public:
        /* Creates a camera texture backed by the given SDL renderer.
          * @param renderer The SDL renderer to use for texture creation. Must not be null. */
        explicit SdlCameraTexture (SDL_Renderer* renderer);

        /* Updates the texture with a new camera frame.
          * @param frame The camera frame containing pixel data and dimensions. */
        void update (const Camera::CameraFrame& frame);

        /* Gets the ImGui texture ID for rendering.
          * @return Pointer to the SDL texture, or nullptr if invalid. */
        void* getImguiTextureId () const;

        /* Gets the current texture width in pixels.
          * @return Width of the texture, or 0 if not initialized. */
        int getWidth () const;

        /* Gets the current texture height in pixels.
          * @return Height of the texture, or 0 if not initialized. */
        int getHeight () const;

        /* Checks if the texture is valid and ready for rendering.
          * @return True if the texture has been successfully created. */
        bool isValid () const;

    private:
        /* Recreates the internal texture with the specified dimensions.
          * @param width  New texture width in pixels.
          * @param height New texture height in pixels. */
        void recreate (int width, int height);

        /** The SDL renderer used for texture creation. */
        SDL_Renderer* m_renderer;
        /** The SDL texture representing the camera frame. */
        SdlTexturePtr m_texture;
        /** Current width of the texture in pixels. */
        int m_width;
        /** Current height of the texture in pixels. */
        int m_height;
    };
}
