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

        SDL_Renderer* m_renderer;
        SdlTexturePtr m_texture;
        int m_width{};
        int m_height{};
    };
}
