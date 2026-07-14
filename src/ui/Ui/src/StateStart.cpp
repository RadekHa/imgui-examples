#include "StateStart.h"
#include "StateLogin.h"
#include "Types/DataModel.h"

#include "imgui.h"

using namespace App;
using namespace Ui;

IUiState* StateStart::update (App::DataModel& /*model*/)
{
    IUiState* state = nullptr;

    ImVec2 center = ImGui::GetMainViewport ()->GetCenter ();
    ImGui::SetNextWindowPos (center, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));
    ImGui::SetNextWindowFocus ();

    ImGui::OpenPopup ("Start");

    if (ImGui::BeginPopupModal ("Start", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text ("All those beautiful files will be deleted.\nThis operation cannot be undone!");
        ImGui::Separator ();

        if (ImGui::Button ("OK", ImVec2 (120, 0)))
        {
            ImGui::CloseCurrentPopup ();

            state = new StateLogin;
        }
        ImGui::EndPopup ();
    }
    return state;
}
