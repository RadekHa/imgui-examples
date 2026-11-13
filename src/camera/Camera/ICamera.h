#pragma once
#include "Types/CameraInfo.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace Camera
{
    /** Struct representing a camera frame. */
    struct CameraFrame
    {
        /** Pointer to the frame data. */
        const uint8_t* data{nullptr};
        /** Frame width in pixels. */
        int width{};
        /** Frame height in pixels. */
        int height{};
        /** Number of color channels in the frame. */
        int channels{};
        /** Byte stride (may be > width * channels due to alignment). */
        int pitch{};
    };

    /** An interface of a camera class. */
    class ICamera
    {
    public:
        /** Virtual interface destructor. */
        virtual ~ICamera () = default;

        /** Close camera. */
        virtual void close () = 0;
        /** Read frame from camera, return true if frame is successfully retrieved. */
        virtual bool read (CameraFrame& frame) = 0;
        /** Return true if the camera is open. */
        virtual bool isOpen () const = 0;
    };

    /** Factory function to create an instance of ICamera. */
    std::unique_ptr<ICamera> createCamera (int cameraIndex);

    /** Enumerate available cameras. Returns a list of camera info (index, name). */
    std::vector<CameraInfo> enumerateCameras ();
}
