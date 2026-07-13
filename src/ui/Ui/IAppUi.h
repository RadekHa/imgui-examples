#pragma once

namespace App
{
    struct DataModel;
}

namespace Ui
{
    /** Interface for the application user interface. */
    struct IAppUi
    {
        /** Virtual destructor to allow proper cleanup of derived classes. */
        virtual ~IAppUi () = default;
        /** Updates the UI based on the given data model. */
        virtual void update (App::DataModel& model) = 0;
    };

    /** Create the concrete application UI instance. */
    [[nodiscard]] IAppUi* createAppUi ();
}
