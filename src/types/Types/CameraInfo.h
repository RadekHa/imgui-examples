#pragma once
#include <string>

namespace Camera
{
    /** Struct representing information about an enumerated camera device. */
    struct CameraInfo
    {
        /** Positional index of the device in the camera device list. */
        int index{};
        /** Human-readable device name. */
        std::string name;
    };
}
