#pragma once
#include <cstdint>
#include <memory>
#include <string>

namespace Camera
{
    struct MatchResult
    {
        /** Flag, that the template was found in the frame. */
        bool found{false};
        /** Position and size of the found template. */
        int x{};
        int y{};
        int width{};
        int height{};
        /** Confidence of the match 0.0 - 1.0. */
        float confidence{};
    };

    struct CameraFrame
    {
        const uint8_t* data{nullptr};
        int width{};
        int height{};
        int channels{};
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
}
