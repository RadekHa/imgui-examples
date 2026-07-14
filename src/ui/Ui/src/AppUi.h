#pragma once
#include "IUiState.h"
#include "Ui/IAppUi.h"

#include <memory>

namespace Ui
{
    /** Main application UI implementation. */
    class AppUi final : public IAppUi
    {
    public:
        /** Initializes the UI with the start state. */
        AppUi ();
        /** Cleaning up resources. */
        ~AppUi () override;

        /** Updates the UI based on the given data model. */
        void update (App::DataModel& model) override;

    private:
        /** Current state of the application state machine. */
        std::unique_ptr<IUiState> m_state;
    };
}
