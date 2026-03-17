#pragma once
#include "DataModel.h"

namespace App
{
    class AppUi
    {
    public:

        void update (DataModel& model);

    private:

        void drawFpsGraph ();
    };
}
