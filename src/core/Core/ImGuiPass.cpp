#include "DPIHandler.hpp"
#include "FrameContext.h"
#include "ImGuiPass.h"

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

using namespace App;
using namespace std;

ImGuiPass::ImGuiPass (SDL_Window* window, SDL_Renderer* renderer, const IPathService* paths)
    : m_renderer (renderer)
{
    IMGUI_CHECKVERSION ();
    ImGui::CreateContext ();

    ImGuiIO& io = ImGui::GetIO ();

    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard |
        ImGuiConfigFlags_DockingEnable |
        ImGuiConfigFlags_ViewportsEnable;

    applyPaths (paths);

    ImGui_ImplSDL2_InitForSDLRenderer (window, renderer);
    ImGui_ImplSDLRenderer2_Init (renderer);
}

ImGuiPass::~ImGuiPass ()
{
    ImGui_ImplSDLRenderer2_Shutdown ();
    ImGui_ImplSDL2_Shutdown ();
    ImGui::DestroyContext ();
}

void ImGuiPass::beginFrame ()
{
    ImGui_ImplSDLRenderer2_NewFrame ();
    ImGui_ImplSDL2_NewFrame ();
    ImGui::NewFrame ();
}

void ImGuiPass::endFrame ()
{
    ImGui::Render ();

    ImGui_ImplSDLRenderer2_RenderDrawData (ImGui::GetDrawData (), m_renderer);

    ImGuiIO& io = ImGui::GetIO ();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows ();
        ImGui::RenderPlatformWindowsDefault ();
    }
}

void ImGuiPass::fillFrameContext (FrameContext& ctx) const
{
    ImGuiIO& io = ImGui::GetIO ();
    ctx.scaleX = io.DisplayFramebufferScale.x;
    ctx.scaleY = io.DisplayFramebufferScale.y;
}

void ImGuiPass::applyPaths (const IPathService* paths)
{
    ImGuiIO& io = ImGui::GetIO ();

    constexpr float baseFontSize = 18.0f;
    const float fontSize = baseFontSize * DPIHandler::get_scale ();

    static string iniFile = paths->getUserConfigPath ();

    io.IniFilename = iniFile.c_str ();

    string fontFile = paths->getFontPath ("Manrope.ttf");

    ImFont* font = io.Fonts->AddFontFromFileTTF (fontFile.c_str (), fontSize);
    io.FontDefault = font;

    DPIHandler::set_global_font_scaling (&io);
}
