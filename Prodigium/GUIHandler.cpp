#include "GUIHandler.h"
using namespace ImGui;

GUIHandler::GUIHandler()
{
	this->io = {};
    this->trap1Active = false;
    this->trap2Active = false;
    this->trap1ImageWidth = 0;
    this->trap1ImageHeight = 0;
    this->trap2ImageWidth = 0;
    this->trap2ImageHeight = 0;

    this->textureTrap1 = nullptr;
    this->textureTrap2 = nullptr;
}
GUIHandler::~GUIHandler()
{
    delete this->textureTrap1;
    delete this->textureTrap2;
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
    PushGuiStyle();

    bool ret = LoadTextureFromFile("Textures/ImGuiTest.png", &textureTrap1, &trap1ImageWidth, &trap1ImageHeight);
    IM_ASSERT(ret);
    bool ret2 = LoadTextureFromFile("Textures/ImGuiTest2.jpg", &textureTrap2, &trap2ImageWidth, &trap2ImageHeight);
    IM_ASSERT(ret2);

}

void GUIHandler::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	NewFrame();

	CreateFPSCounter(120);
    CreateTrapGUI();

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

void GUIHandler::ChangeActiveTrap()
{
    if (this->trap1Active)
    {
        this->trap1Active = false;
        this->trap2Active = true;
    }
    else if (this->trap2Active)
    {
        this->trap2Active = false;
        this->trap1Active = true;
    }
    else if (!this->trap1Active && !this->trap2Active)
        this->trap1Active = true;
}

void GUIHandler::CreateFPSCounter(const float& fps)
{
    SetNextWindowPos(ImVec2(0, -15));
    SetNextWindowSize(ImVec2(250, 250), 0);
	Begin("FPS");
	Text("FPS: %d", (int)fps);
	End();
}

void GUIHandler::CreateTrapGUI()
{
    bool* trap1 = new bool(trap1Active);
    bool* trap2 = new bool(trap2Active);
    SetNextWindowPos(ImVec2(200,(float)Graphics::GetWindowHeight() - 250));
    SetNextWindowSize(ImVec2((float)trap1ImageWidth + 50.f, (float)trap1ImageHeight + 50.f));
    
    if (this->trap1Active)
    {
        Begin("TRAP 1", trap1, ImGuiWindowFlags_NoTitleBar);
        Image((void*)textureTrap1, ImVec2((float)trap1ImageWidth, (float)trap1ImageHeight));
        End();
    }
    if (this->trap2Active)
    {
        Begin("TRAP 2", trap2, ImGuiWindowFlags_NoTitleBar);
        Image((void*)textureTrap2, ImVec2((float)trap2ImageWidth, (float)trap2ImageHeight));
        End();
    }

    delete trap1;
    delete trap2;
}

bool GUIHandler::LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int *out_width, int *out_height)
{
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if(image_data == NULL)
        return false;

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    Graphics::GetDevice()->CreateTexture2D(&desc, &subResource, &pTexture);
    if (pTexture == NULL)
        return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    Graphics::GetDevice()->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    *out_width = image_width;
    *out_height = image_height;
    stbi_image_free(image_data);

    return true;
}

