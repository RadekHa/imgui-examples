#include "Camera.h"
#include "TraceLog/Log.hpp"

#include <SDL3/SDL.h>

using namespace Camera;
using namespace std;

namespace details
{
    struct SdlCameraIdDeleter
    {
        void operator() (SDL_CameraID* id) const noexcept
        {
            SDL_free (id);
        }
    };

    using SDLCameraIdPtr = unique_ptr<SDL_CameraID, SdlCameraIdDeleter>;
}

///////////////////////////////////////////////////////////////////////////////
// Factory function.

unique_ptr<ICamera> Camera::createCamera (int cameraIndex)
{
    unique_ptr<SDLCamera> cam = make_unique<SDLCamera>();

    if (!cam->open (cameraIndex))
    {
        return nullptr;
    }
    return cam;
}

///////////////////////////////////////////////////////////////////////////////
// SDLCamera

SDLCamera::SDLCamera ()
    : m_camera {nullptr, SDL_CloseCamera}
    , m_permissionGranted {}
    , m_rgbSurface {nullptr, SDL_DestroySurface}
{
}

SDLCamera::~SDLCamera ()
{
    close ();
}

bool SDLCamera::open (int cameraIndex)
{
    bool result = false;
    close ();

    int count = 0;
    auto devices = details::SDLCameraIdPtr (SDL_GetCameras (&count));

    if (devices && (count > 0) && (cameraIndex >= 0) && (cameraIndex < count))
    {
        SDL_CameraID deviceId = devices.get () [cameraIndex];
        SDL_Camera* cam = SDL_OpenCamera (deviceId, nullptr);

        if (cam)
        {
            m_camera.reset (cam);
            APP_INFO ("Camera opened: index {}", cameraIndex);
            result = true;
        }
        else
        {
            APP_ERROR ("[Camera] Failed to open camera: {}", SDL_GetError ());
        }
    }
    else
    {
        if (devices)
        {
            devices.reset ();
        }
        APP_ERROR ("[Camera] Invalid camera index {} (available: {})", cameraIndex, count);
    }
    m_permissionGranted = false;
    return result;
}

void SDLCamera::close ()
{
    m_camera.reset ();
    m_permissionGranted = false;
}

bool SDLCamera::read (CameraFrame& frame)
{
    bool result = false;

    if (m_camera)
    {
        SDL_CameraPermissionState permState = SDL_GetCameraPermissionState (m_camera.get ());

        if (permState == SDL_CAMERA_PERMISSION_STATE_DENIED)
        {
            APP_ERROR ("[Camera] Camera permission denied");
            close ();
        }
        else if (permState != SDL_CAMERA_PERMISSION_STATE_PENDING)
        {
            m_permissionGranted = true;

            Uint64 timestampNS = 0;
            SDL_Surface* sdlFrame = SDL_AcquireCameraFrame (m_camera.get (), &timestampNS);

            if (sdlFrame)
            {
                {
                    lock_guard lock{m_mutex};

                    SDL_Surface* rgbSurface = SDL_ConvertSurface (sdlFrame, SDL_PIXELFORMAT_RGB24);

                    if (rgbSurface)
                    {
                        m_rgbSurface.reset (rgbSurface);

                        frame.data = static_cast<const uint8_t*> (m_rgbSurface->pixels);
                        frame.width = m_rgbSurface->w;
                        frame.height = m_rgbSurface->h;
                        frame.channels = 3;
                        result = true;
                    }
                    else
                    {
                        APP_WARN ("[Camera] Failed to convert surface to RGB24: {}", SDL_GetError ());
                    }
                }
                SDL_ReleaseCameraFrame (m_camera.get (), sdlFrame);
            }
        }
    }
    return result;
}

bool SDLCamera::isOpen () const
{
    return m_camera != nullptr && m_permissionGranted;
}
