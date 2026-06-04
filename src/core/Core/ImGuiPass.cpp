#include "DpiHandler.h"
#include "FrameContext.h"
#include "ImGuiPass.h"
#include "Log.hpp"

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <SDL_events.h>

#include <stdexcept>

using namespace App;
using namespace std;

ImGuiPass::ImGuiPass (SDL_Window* window, SDL_Renderer* renderer, const IPathService* paths)
    : m_renderer {renderer}
    , m_paths {paths}
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

    float scale = DpiHandler::getScale (window);
    applyPaths (scale);

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

void ImGuiPass::beginFrame (const vector<SDL_Event>& events)
{
    for (const auto& e : events)
    {
        ImGui_ImplSDL2_ProcessEvent (&e);
    }
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

void ImGuiPass::rebuildFonts (float scale)
{
    ImGuiIO& io = ImGui::GetIO ();

    ImGui_ImplSDLRenderer2_DestroyDeviceObjects ();

    io.Fonts->Clear ();
    io.FontDefault = nullptr;
    applyPaths (scale);
    io.Fonts->Build ();

    ImGui_ImplSDLRenderer2_CreateDeviceObjects ();

    // Reset style to default and apply scaling.
    ImGuiStyle& style = ImGui::GetStyle ();
    style = ImGuiStyle ();
    style.ScaleAllSizes (scale);
}

void ImGuiPass::applyPaths (float scale)
{
    ImGuiIO& io = ImGui::GetIO ();

    constexpr float baseFontSize = 18.0f;
    const float fontSize = baseFontSize * scale;

    io.IniFilename = m_paths->getUserConfigFilePath ();

    string fontFile = m_paths->getFontPath ("Manrope.ttf");

    ImFont* font = io.Fonts->AddFontFromFileTTF (fontFile.c_str (), fontSize);

    if (font)
    {
        io.FontDefault = font;
    }
    else
    {
        APP_WARN ("Failed to load font from path: {}", fontFile.c_str ());
    }
}
