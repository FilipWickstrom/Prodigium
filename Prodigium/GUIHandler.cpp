#include "GUIHandler.h"
using namespace ImGui;

GUIHandler::GUIHandler()
{
	this->io = {};
}
GUIHandler::~GUIHandler()
{
}

void PushGuiStyle()
{
	ImGuiStyle& style = GetStyle();
	

	style.Alpha = 1.0f;
	style.FrameRounding = 0;
    style.Colors[ImGuiCol_Text]                                     = ImVec4(0.f, 1.f, 0.f, 1.f);
    style.Colors[ImGuiCol_TextDisabled]                             = ImVec4(0.f, 0.f, 0.f, 1.f);
    style.Colors[ImGuiCol_WindowBg]                                 = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PopupBg]                                  = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_Border]                                   = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_BorderShadow]                             = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_FrameBg]                                  = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_FrameBgHovered]                           = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_FrameBgActive]                            = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_TitleBg]                                  = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_TitleBgCollapsed]                         = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_TitleBgActive]                            = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_MenuBarBg]                                = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ScrollbarBg]                              = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ScrollbarGrab]                            = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]                     = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]                      = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_CheckMark]                                = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_SliderGrab]                               = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_SliderGrabActive]                         = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_Button]                                   = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ButtonHovered]                            = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ButtonActive]                             = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_Header]                                   = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_HeaderHovered]                            = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_HeaderActive]                             = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ResizeGrip]                               = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ResizeGripHovered]                        = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ResizeGripActive]                         = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotLines]                                = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotLinesHovered]                         = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotHistogram]                            = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotHistogramHovered]                     = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_TextSelectedBg]                           = ImVec4(0.f, 0.f, 0.f, 0.f);	
}

void GUIHandler::Initialize(const HWND& window)
{
	CreateContext();
	io = GetIO();

	ImGui_ImplDX11_Init(Graphics::GetDevice() , Graphics::GetContext());
	ImGui_ImplWin32_Init(window);
}

void GUIHandler::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	NewFrame();

	CreateFPSCounter(120);

	EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(GetDrawData());
}

void GUIHandler::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();
}

void GUIHandler::CreateFPSCounter(const float& fps)
{
	Begin("");
	Text("%d", (int)fps);
	PushGuiStyle();
	End();
}

