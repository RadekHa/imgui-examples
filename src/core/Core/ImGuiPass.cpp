#include "ImGuiPass.h"

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

using namespace App;

ImGuiPass::ImGuiPass (SDL_Window* window, SDL_Renderer* renderer)
    : m_renderer (renderer)
{
    IMGUI_CHECKVERSION ();
    ImGui::CreateContext ();

    ImGuiIO& io = ImGui::GetIO ();

    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard |
        ImGuiConfigFlags_DockingEnable |
        ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplSDL2_InitForSDLRenderer (window, renderer);
    ImGui_ImplSDLRenderer2_Init (renderer);
}

ImGuiPass::~ImGuiPass ()
{
    ImGui_ImplSDLRenderer2_Shutdown ();
    ImGui_ImplSDL2_Shutdown ();
    ImGui::DestroyContext ();
}

void ImGuiPass::begin ()
{
    ImGui_ImplSDLRenderer2_NewFrame ();
    ImGui_ImplSDL2_NewFrame ();
    ImGui::NewFrame ();
}

void ImGuiPass::end ()
{
    ImGui::Render ();
    ImGui_ImplSDLRenderer2_RenderDrawData (ImGui::GetDrawData (), m_renderer);
}
