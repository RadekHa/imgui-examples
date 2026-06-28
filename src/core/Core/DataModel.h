#pragma once
#include "serialib.h"

namespace App
{
    /** Struct representing the data model for the application. */
    struct DataModel
    {
        bool isSmile{false};

        /** Start time after closing of the initial dialog. */
        double startTime{};

        serialib serial;
    };
}
