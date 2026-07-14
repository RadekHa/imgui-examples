#pragma once
#include "IUiState.h"

namespace Ui
{
    /** Start state shown as the initial application dialog. */
    class StateStart : public IUiState
    {
    public:
        /** {@inheritDoc} */
        IUiState* update (App::DataModel& model) override;
    };
}
