#pragma once
#include "IUiState.h"

namespace Ui
{
    /** Null state representing an inactive or terminal state. */
    class StateNull : public IUiState
    {
    public:
        /** {@inheritDoc} */
        IUiState* update (App::DataModel& model) override;
    };
}
