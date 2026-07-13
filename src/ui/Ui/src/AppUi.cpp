#include "AppUi.h"
#include "StateStart.h"
#include "Types/DataModel.h"

#include "imgui.h"

using namespace App;
using namespace Ui;


AppUi::AppUi ()
    : m_state {new StateStart}
{
}

AppUi::~AppUi () = default;

namespace Ui
{
    IAppUi* createAppUi ()
    {
        return new AppUi;
    }
}

void AppUi::update (App::DataModel& model)
{
    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGui::DockSpaceOverViewport (0, ImGui::GetMainViewport (), dockspaceFlags);

    if (ImGui::BeginMainMenuBar ())
    {
        if (ImGui::BeginMenu ("View"))
        {
            ImGui::MenuItem ("Demo", nullptr, &model.showDemo);
            ImGui::EndMenu ();
        }
        ImGui::EndMainMenuBar ();
    }

    if (model.showDemo)
    {
        ImGui::ShowDemoWindow (&model.showDemo);
    }

    if (model.camera.valid)
    {
        ImGui::Begin ("Camera");

        ImVec2 available = ImGui::GetContentRegionAvail ();

        float texW = float (model.camera.width);
        float texH = float (model.camera.height);

        float scale = std::min (available.x / texW, available.y / texH);
        ImVec2 size = ImVec2 (texW * scale, texH * scale);

        ImVec2 cursor = ImGui::GetCursorPos ();

        ImVec2 pos = ImVec2 (cursor.x + (available.x - size.x) * 0.5f,
                             cursor.y + (available.y - size.y) * 0.5f);

        ImGui::SetCursorPos (pos);

        ImGui::Image (model.camera.textureId,
                      size,
                      ImVec2 (1, 0),
                      ImVec2 (0, 1));
        ImGui::End ();
    }
    IUiState* state = m_state->update (model);

    if (state != nullptr)
    {
        m_state.reset (state);
    }
}
