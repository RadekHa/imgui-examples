#pragma once
#include "Ui/IAppUi.h"

namespace Ui
{
    /** Main application UI implementation. */
    class AppUi final : public IAppUi
    {
    public:
        /** Initializes the UI. */
        AppUi ();
        /** Cleaning up resources. */
        ~AppUi () override;

        /** Updates the UI based on the given data model. */
        void update (App::DataModel& model) override;
    };
}
