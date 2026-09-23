#pragma once
#include "Camera/ICamera.h"

#include <SDL3/SDL.h>

#include <memory>
#include <mutex>

namespace Camera
{
    /**
      * SDL3-based camera implementation using the native SDL_Camera API.
      */
    class SDLCamera : public ICamera
    {
    public:
        /** Default constructor. */
        SDLCamera ();
        /** Destructor - closes the camera if open. */
        ~SDLCamera () override;

        /**
          * Open a camera device by index.
          * @param cameraIndex Index of the camera device to open (0 = first device).
          * @return true if camera was successfully opened, false otherwise.
          */
        bool open (int cameraIndex);

        /** {@inheritDoc} */
        void close () override;
        /** {@inheritDoc} */
        bool read (CameraFrame& frame) override;
        /** {@inheritDoc} */
        bool isOpen () const override;

    private:
        using SDLCameraPtr = std::unique_ptr<SDL_Camera, void (*)(SDL_Camera*)>;
        /** Opaque SDL camera handle managed via RAII. */
        SDLCameraPtr m_camera{nullptr, SDL_CloseCamera};
        /** True when user has granted camera permission. */
        bool m_permissionGranted = false;
        /** RGB24 converted surface for frame data. */
        std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)> m_rgbSurface;
        /** Mutex protecting frame access. */
        std::mutex m_mutex;
    };
}
