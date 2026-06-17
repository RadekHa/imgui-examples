#include "AppUi.h"
#include "SdlCameraTexture.h"

#include "imgui.h"

#include <stdio.h>

using namespace App;
using namespace Sdl;

void AppUi::update (DataModel& model, const SdlCameraTexture* camera)
{
    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGui::DockSpaceOverViewport (0, ImGui::GetMainViewport (), dockspaceFlags);

    if (ImGui::BeginMainMenuBar ())
    {
        if (ImGui::BeginMenu ("View"))
        {
            ImGui::MenuItem ("Demo", nullptr, &model.showDemo);
            ImGui::MenuItem ("Stats", nullptr, &model.showStats);
            ImGui::EndMenu ();
        }
        ImGui::EndMainMenuBar ();
    }

    if (model.showDemo)
    {
        ImGui::ShowDemoWindow (&model.showDemo);
    }

    if (model.showStats)
    {
        drawFpsGraph ();
    }

    if (camera->isValid ())
    {
        ImGui::Begin ("Camera");

        ImVec2 available = ImGui::GetContentRegionAvail ();

        float texW = float (camera->getWidth ());
        float texH = float (camera->getHeight ());

        float scale = std::min (available.x / texW, available.y / texH);
        ImVec2 size = ImVec2 (texW * scale, texH * scale);

        ImVec2 cursor = ImGui::GetCursorPos ();

        ImVec2 pos = ImVec2 (cursor.x + (available.x - size.x) * 0.5f,
                             cursor.y + (available.y - size.y) * 0.5f);

        ImGui::SetCursorPos (pos);

        ImGui::Image (camera->getImguiTextureId (),
                      size,
                      ImVec2 (1, 0),
                      ImVec2 (0, 1));
        ImGui::End ();
    }
}

void AppUi::drawFpsGraph ()
{
    static float values [90]{};
    static int offset{};
    static double nextTime{};

    if (ImGui::GetTime () > nextTime)
    {
        values [offset] = ImGui::GetIO ().Framerate;
        offset = (offset + 1) % IM_ARRAYSIZE (values);
        nextTime = ImGui::GetTime () + 0.1;
    }
    float avg{};

    for (float v : values)
    {
        avg += v;
    }
    avg /= IM_ARRAYSIZE (values);

    char overlay [32];
    snprintf (overlay, sizeof (overlay), "AVG %.1f FPS", avg);

    ImGui::Begin ("Stats");

    ImGui::PlotLines (
        "FPS",
        values,
        IM_ARRAYSIZE (values),
        offset,
        overlay,
        0.0f,
        120.0f,
        ImVec2 (0, 80));

    ImGui::End ();
}
