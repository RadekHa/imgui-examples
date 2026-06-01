#include "DPIHandler.hpp"
#include "FrameContext.h"
#include "ImGuiPass.h"
#include "Log.hpp"

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

#include <stdexcept>

using namespace App;
using namespace std;

ImGuiPass::ImGuiPass (SDL_Window* window, SDL_Renderer* renderer, const IPathService* paths)
    : m_renderer (renderer)
{
    if ((window == nullptr) || (renderer == nullptr) || (paths == nullptr))
    {
        throw invalid_argument ("ImGuiPass received null dependency");
    }
    IMGUI_CHECKVERSION ();

    ImGui::CreateContext ();

    ImGuiIO& io = ImGui::GetIO ();

    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard |
        ImGuiConfigFlags_DockingEnable |
        ImGuiConfigFlags_ViewportsEnable;

    applyPaths (paths);

    if (!ImGui_ImplSDL2_InitForSDLRenderer (window, renderer))
    {
        throw runtime_error ("ImGui_ImplSDL2_InitForSDLRenderer failed");
    }

    if (!ImGui_ImplSDLRenderer2_Init (renderer))
    {
        throw runtime_error ("ImGui_ImplSDLRenderer2_Init failed");
    }
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

    io.IniFilename = paths->getUserConfigFilePath ();

    string fontFile = paths->getFontPath ("Manrope.ttf");

    ImFont* font = io.Fonts->AddFontFromFileTTF (fontFile.c_str (), fontSize);

    if (font)
    {
        io.FontDefault = font;
    }
    else
    {
        APP_WARN ("Failed to load font from path: %s", fontFile.c_str ());
    }
    DPIHandler::set_global_font_scaling (&io);
}
