#include "GUIHandler.h"
using namespace ImGui;
GUIHandler* GUIHandler::instance = nullptr;

GUIHandler::GUIHandler()
{
	io = {};
    trap1Active = false;
    trap2Active = false;
    isPaused = false;
    shouldQuit = false;
    imageWidth = 0;
    imageHeight = 0;
    playerPos = {};

    textureTrap1 = nullptr;
    textureTrap2 = nullptr;
    textureBrain = nullptr;
    textureOutline = nullptr;
}
GUIHandler::~GUIHandler()
{
    textureTrap1->Release();
    textureTrap2->Release();
    textureBrain->Release();
    textureOutline->Release();  
}

void SetUpGUIStyle()
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
    style.Colors[ImGuiCol_FrameBg]                                  = ImVec4(46.f/255.f, 68.f/255.f, 138.f/255.f, 1.f);
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
    style.Colors[ImGuiCol_Button]                                   = ImVec4(0.f, 0.f, 0.f, 0.6f);
    style.Colors[ImGuiCol_ButtonHovered]                            = ImVec4(1.f, 1.f, 1.f, 0.5f);
    style.Colors[ImGuiCol_ButtonActive]                             = ImVec4(1.f, 1.f, 1.f, 1.f);
    style.Colors[ImGuiCol_Header]                                   = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_HeaderHovered]                            = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_HeaderActive]                             = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ResizeGrip]                               = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ResizeGripHovered]                        = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ResizeGripActive]                         = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotLines]                                = ImVec4(0.f, 0.f, 0.f, 1.f);
    style.Colors[ImGuiCol_PlotLinesHovered]                         = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotHistogram]                            = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotHistogramHovered]                     = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_TextSelectedBg]                           = ImVec4(0.f, 0.f, 0.f, 0.f);
}

const bool GUIHandler::Initialize(const HWND& window)
{
    if (!GUIHandler::instance)
    {
        GUIHandler::instance = new GUIHandler;
        CreateContext();
        GUIHandler::instance->io = GetIO();
        

        ImGui_ImplDX11_Init(Graphics::GetDevice(), Graphics::GetContext());
        ImGui_ImplWin32_Init(window);
        SetUpGUIStyle();

        bool ret = GUIHandler::instance->LoadTextureFromFile("Textures/StopMoving.png", &GUIHandler::instance->textureTrap1, &GUIHandler::instance->imageWidth, &GUIHandler::instance->imageHeight);
        IM_ASSERT(ret);
        ret = GUIHandler::instance->LoadTextureFromFile("Textures/SlowMoving.png", &GUIHandler::instance->textureTrap2, &GUIHandler::instance->imageWidth, &GUIHandler::instance->imageHeight);
        IM_ASSERT(ret);
        ret = GUIHandler::instance->LoadTextureFromFile("Textures/Brain.png", &GUIHandler::instance->textureBrain, &GUIHandler::instance->imageWidth, &GUIHandler::instance->imageHeight);
        IM_ASSERT(ret);
        ret = GUIHandler::instance->LoadTextureFromFile("Textures/Outline.png", &GUIHandler::instance->textureOutline, &GUIHandler::instance->imageWidth, &GUIHandler::instance->imageHeight);
        IM_ASSERT(ret);
    }
    else
    {
        std::cerr << "GUIHandler already Initialized\n";
    }
    
    return true;
}

void GUIHandler::Render()
{
  
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	NewFrame();

//#ifdef _DEBUG
    GUIHandler::instance->CreateDebugGUI();
//#endif // _DEBUG

	
    GUIHandler::instance->CreateTrapGUI();
    //CreateBrainGUI();
    if(GUIHandler::instance->isPaused)
        GUIHandler::instance->CreatePauseMenu();

	EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(GetDrawData());
}

void GUIHandler::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();
    if (GUIHandler::instance)
    {
        delete GUIHandler::instance;
    }
}

void GUIHandler::ChangeActiveTrap()
{
    if (GUIHandler::instance->trap1Active)
    {
        GUIHandler::instance->trap1Active = false;
        GUIHandler::instance->trap2Active = true;
    }
    else if (GUIHandler::instance->trap2Active)
    {
        GUIHandler::instance->trap2Active = false;
        GUIHandler::instance->trap1Active = true;
    }
    else if (!GUIHandler::instance->trap1Active && !GUIHandler::instance->trap2Active)
        GUIHandler::instance->trap1Active = true;
}

void GUIHandler::SetPlayerPos(const DirectX::SimpleMath::Vector3& playerPos)
{
    GUIHandler::instance->playerPos = playerPos;
}

void GUIHandler::PauseGame()
{
    GUIHandler::instance->isPaused = true;
}

void GUIHandler::ResumeGame()
{
    GUIHandler::instance->isPaused = false;
}

const bool GUIHandler::ShouldQuit()
{
    return GUIHandler::instance->shouldQuit;
}

const bool GUIHandler::ActiveTrap() const
{
    return this->trap1Active;
}

void GUIHandler::CreateDebugGUI()
{
    SetNextWindowPos(ImVec2(0, -25));
    SetNextWindowSize(ImVec2(250, 250), 0);
	Begin("DEBUG");
	Text("FPS: %d", (int)GetIO().Framerate);
    Text("Player Pos X: %f", this->playerPos.x);
    Text("Player Pos Z: %f", this->playerPos.z);
	End();
}

void GUIHandler::CreateTrapGUI()
{
    bool* trap1 = new bool(trap1Active);
    bool* trap2 = new bool(trap2Active);
    SetNextWindowPos(ImVec2(50,(float)Graphics::GetWindowHeight() - 150));
    SetNextWindowSize(ImVec2((float)imageWidth, (float)imageHeight));
        
    Begin("TRAP 1", trap1, ImGuiWindowFlags_NoTitleBar);
    Image((void*)textureTrap1, ImVec2((float)imageWidth / 2, (float)imageHeight / 2));
    End();
    
    SetNextWindowPos(ImVec2(200, (float)Graphics::GetWindowHeight() - 150));
    SetNextWindowSize(ImVec2((float)imageWidth, (float)imageHeight));
    Begin("TRAP 2", trap2, ImGuiWindowFlags_NoTitleBar);
    Image((void*)textureTrap2, ImVec2((float)imageWidth / 2, (float)imageHeight / 2));
    End();

    if (this->trap1Active)
    {
        SetNextWindowPos(ImVec2(50, (float)Graphics::GetWindowHeight() - 150));
        SetNextWindowSize(ImVec2((float)imageWidth, (float)imageHeight));
        Begin("TRAP 1 OUTLINE", trap1, ImGuiWindowFlags_NoTitleBar);
        Image((void*)textureOutline, ImVec2((float)imageWidth / 2, (float)imageHeight / 2));
        End();
    }
    else if (this->trap2Active)
    {
        SetNextWindowPos(ImVec2(200, (float)Graphics::GetWindowHeight() - 150));
        SetNextWindowSize(ImVec2((float)imageWidth, (float)imageHeight));
        Begin("TRAP 2 OUTLINE", trap1, ImGuiWindowFlags_NoTitleBar);
        Image((void*)textureOutline, ImVec2((float)imageWidth / 2, (float)imageHeight / 2));
        End();
    }
    

    delete trap1;
    delete trap2;
}

void GUIHandler::CreateBrainGUI()
{
    bool* isActive = new bool;
    SetNextWindowPos(ImVec2((float)Graphics::GetWindowWidth() - 250, 25));    
    SetNextWindowPos(ImVec2((float)Graphics::GetWindowWidth() - 250, 25));
    SetNextWindowSize(ImVec2((float)imageWidth + 50.f, (float)imageHeight + 50.f));
    Begin("BRAIN GUI", isActive, ImGuiWindowFlags_NoTitleBar);
    Image((void*)textureBrain, ImVec2((float)imageWidth, (float)imageHeight));
    End();
    delete isActive;
}

void GUIHandler::CreatePauseMenu()
{

    SetNextWindowSize(ImVec2(1920, 1080));
    SetNextWindowPos(ImVec2(0, 0));
    SetNextWindowBgAlpha(0.5);
    bool* isActive = new bool;

    Begin("Pause Menu", isActive, ImGuiWindowFlags_NoTitleBar);

        SetNextWindowPos(ImVec2(500, 250));
        Begin("Resume Button", isActive, ImGuiWindowFlags_NoTitleBar);
        if(Button("Resume", ImVec2(250, 50)))
        {
            ResumeGame();
        }
        End();

        SetNextWindowPos(ImVec2(500, 325));
        Begin("Quit Button", isActive, ImGuiWindowFlags_NoTitleBar);
        if(Button("Quit", ImVec2(250, 50)))
        {
            GUIHandler::instance->shouldQuit = true;
            std::cout << "QUIT! /n";
        }
        End();

    End();

    delete isActive;
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

