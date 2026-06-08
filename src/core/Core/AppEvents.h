#pragma once
#include <cstdint>

namespace App
{
    /** Event representing a quit request. */
    struct EventQuit {};
    /** Event representing a close request. */
    struct EventClose {};
    /** Event representing a minimized request. */
    struct EventMinimized {};
    /** Event representing a shown request. */
    struct EventShown {};
    /** Event representing a restored request. */
    struct EventRestored {};
    /** Event representing a display change request. */
    struct EventDisplayChanged
    {
        /** The index of the new display. */
        int32_t displayIndex;
    };
}
