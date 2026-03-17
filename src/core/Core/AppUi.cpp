#include "AppUi.h"

#include "imgui.h"

#include <stdio.h>

using namespace App;

void AppUi::update (DataModel& model)
{
    ImGui::DockSpaceOverViewport ();

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
    sprintf (overlay, "AVG %.1f FPS", avg);

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
