#pragma once
#include <memory>
#include <string>

namespace Camera
{
    struct CameraFrame
    {
        const uint8_t* data{nullptr};
        int width{};
        int height{};
        int channels{};
    };

    class ICamera
    {
    public:
        virtual ~ICamera () = default;
        virtual void close () = 0;
        virtual bool read (CameraFrame& frame) = 0;
        virtual bool isOpen () const = 0;
    };

    /** Factory function to create an instance of ICamera. */
    std::unique_ptr<ICamera> createCamera (int cameraIndex);
}
