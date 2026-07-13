#include "StateNull.h"

using namespace Ui;

IUiState* StateNull::update (App::DataModel& /*model*/)
{
    return nullptr;
}
