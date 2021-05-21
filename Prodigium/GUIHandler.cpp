#include "GUIHandler.h"
using namespace ImGui;
GUIHandler* GUIHandler::instance = nullptr;

GUIHandler::GUIHandler()
{
	io = {};
    trap1Active = true;
    trap2Active = false;
    isPaused = false;
    shouldQuit = false;
    shouldResume = false;
    showMainMenu = false;
    showGameGUI = false;
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

void SetUpGUIStyleDEBUG()
{
    ImGuiStyle& style = GetStyle();


    style.Alpha = 1.0f;
    style.FrameRounding = 0;
    style.Colors[ImGuiCol_Text] = ImVec4(0.f, 1.f, 0.f, 1.f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.f, 0.f, 0.f, 1.f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(46.f / 255.f, 68.f / 255.f, 138.f / 255.f, 1.f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.f, 0.f, 0.f, 0.6f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.f, 1.f, 1.f, 0.5f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.f, 1.f, 1.f, 1.f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.f, 0.f, 0.f, 1.f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
}

void SetUpGUIStyleGame()
{
	ImGuiStyle& style = GetStyle();
	

	style.Alpha = 1.0f;
	style.FrameRounding = 0;
    style.Colors[ImGuiCol_Text]                                     = ImVec4(0.6f, 0.6f, 0.6f, 1.f);
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

void SetUpGUIStyleMainMenu()
{
    ImGuiStyle& style = GetStyle();


    style.Alpha = 1.0f;
    style.FrameRounding = 0;
    style.Colors[ImGuiCol_Text] = ImVec4(0.6f, 0.6f, 1.0f, 1.f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.f, 0.f, 0.f, 1.f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(46.f / 255.f, 68.f / 255.f, 138.f / 255.f, 1.f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(46.f / 255.f, 68.f / 255.f, 138.f / 255.f, 0.6f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(46.f / 255.f, 68.f / 255.f, 138.f / 255.f, 0.6f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.6f, 0.6f, 1.0f, 1.f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.f, 1.f, 1.f, 0.6f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.6f, 0.6f, 1.0f, 0.6f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.f, 0.f, 0.f, 0.6f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.f, 1.f, 1.f, 0.5f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.f, 1.f, 1.f, 1.f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.f, 0.f, 0.f, 1.f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
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

void GUIHandler::Render(int playerHp, int clues, float& timer1, float& timer2, OptionsHandler& options)
{
    if (GUIHandler::instance->isPaused)
    {
        GetIO().WantCaptureMouse = true;
        GetIO().WantCaptureKeyboard = true;
        GetIO().MouseDrawCursor = true;
    }
    else
    {
        GetIO().WantCaptureMouse = false;
        GetIO().WantCaptureKeyboard = false;
        GetIO().MouseDrawCursor = false;
    }
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	NewFrame();

    if (options.hasDebugInfo)
    {
        SetUpGUIStyleDEBUG();
        GUIHandler::instance->RenderDebugGUI();
    }

    if (GUIHandler::instance->showMainMenu)
    {
        SetUpGUIStyleMainMenu();
        GUIHandler::instance->RenderMainMenu();
    }
    if (GUIHandler::instance->showGameGUI)
    {
        SetUpGUIStyleGame();
        GUIHandler::instance->RenderTrapGUI(timer1, timer2, options);
        GUIHandler::instance->RenderBrainGUI(playerHp, clues, options);
        if (GUIHandler::instance->isPaused && GUIHandler::instance->showPauseMenu)
            GUIHandler::instance->RenderPauseMenu();
    }
    if (GUIHandler::instance->showOptionsMenu)
    {
        SetUpGUIStyleMainMenu();
        GetIO().WantCaptureMouse = true;
        GetIO().WantCaptureKeyboard = true;
        GetIO().MouseDrawCursor = true;
        GUIHandler::instance->RenderOptionsMenu(options);
    }
    

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
    GUIHandler::instance->shouldResume = false;
    GUIHandler::instance->showPauseMenu = true;
}

void GUIHandler::ResumeGame()
{
    GUIHandler::instance->isPaused = false;
    GUIHandler::instance->shouldResume = true;
}

const bool GUIHandler::ShouldResume()
{
    return GUIHandler::instance->shouldResume;
}

const bool GUIHandler::ShouldQuit()
{
    return GUIHandler::instance->shouldQuit;
}

void GUIHandler::ShowMainMenu(const bool& show)
{
    GUIHandler::instance->showMainMenu = show;
}

void GUIHandler::ShowGameGUI(const bool& show)
{
    GUIHandler::instance->showGameGUI = show;
}

void GUIHandler::ShowOptionsMenu(const bool& show)
{
    GUIHandler::instance->showOptionsMenu = show;
}

void GUIHandler::ShowInGameOptionsMenu(const bool& show)
{
    if (!show)
    {
        GUIHandler::instance->showGameGUI = true;
        GUIHandler::instance->showOptionsMenu = false;
        GUIHandler::instance->showPauseMenu = true;
    }
}

const bool GUIHandler::ActiveTrap()
{
    return GUIHandler::instance->trap1Active;
}

void GUIHandler::RenderDebugGUI()
{
    SetNextWindowPos(ImVec2(0, -25));
    SetNextWindowSize(ImVec2(250, 250), 0);
	Begin("DEBUG");
	Text("FPS: %d", (int)GetIO().Framerate);
    Text("Player Pos X: %f", this->playerPos.x);
    Text("Player Pos Z: %f", this->playerPos.z);
	End();
}

void GUIHandler::RenderTrapGUI(float& timer1, float& timer2, OptionsHandler& options)
{
    bool* trap1 = new bool(trap1Active);
    bool* trap2 = new bool(trap2Active);
    SetNextWindowPos(ImVec2(50,(float)Graphics::GetWindowHeight() - 150));
    SetNextWindowSize(ImVec2((float)imageWidth, (float)imageHeight));

        
    Begin("TRAP 1", trap1, ImGuiWindowFlags_NoTitleBar);
    Image((void*)textureTrap1, ImVec2((float)imageWidth / 2, (float)imageHeight / 2));
    End();

    SetNextWindowPos(ImVec2(50, (float)Graphics::GetWindowHeight() - 165));
    SetNextWindowSize(ImVec2((float)imageWidth, (float)imageHeight));
    Begin("TRAP 1 TIMER", trap1, ImGuiWindowFlags_NoTitleBar);

    // Limit to one decimal
    std::string t1(std::to_string(timer1));
    if(timer1 > 10)
        t1.erase(t1.begin() + 4, t1.end());
    else
        t1.erase(t1.begin() + 3, t1.end());
    Text(t1.c_str());
    End();
    
    SetNextWindowPos(ImVec2(200, (float)Graphics::GetWindowHeight() - 150));
    SetNextWindowSize(ImVec2((float)imageWidth, (float)imageHeight));
    Begin("TRAP 2", trap2, ImGuiWindowFlags_NoTitleBar);
    Image((void*)textureTrap2, ImVec2((float)imageWidth / 2, (float)imageHeight / 2));
    End();

    SetNextWindowPos(ImVec2(200, (float)Graphics::GetWindowHeight() - 165));
    SetNextWindowSize(ImVec2((float)imageWidth, (float)imageHeight));
    Begin("TRAP 2 TIMER", trap2, ImGuiWindowFlags_NoTitleBar);

    // Limit to one decimal
    std::string t2(std::to_string(timer2));
    if (timer2 > 10)
        t2.erase(t2.begin() + 4, t2.end());
    else
        t2.erase(t2.begin() + 3, t2.end());
    Text(t2.c_str());
    End();

    if (this->clockTimer)
    {
        SetNextWindowPos(ImVec2(15, 15));
        SetNextWindowSize(ImVec2(60, 30));
        Begin("GAME TIMER", trap2, ImGuiWindowFlags_NoTitleBar);

        // Limit to one decimal
        std::string t3(std::to_string((int)options.gameTimer));
        Text(t3.c_str());
        End();
    }

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

void GUIHandler::RenderBrainGUI(int playerHp, int clues, OptionsHandler& options)
{
    float fade = 1.0f;
    float hp = (float)playerHp;
    fade = std::max(std::min(hp, 100.0f), 10.0f) * 0.01f;

    bool* isActive = new bool(true);
    SetNextWindowPos(ImVec2((float)Graphics::GetWindowWidth() - 250, 25));    
    SetNextWindowSize(ImVec2((float)imageWidth + 25, (float)imageHeight + 25));
    Begin("BRAIN GUI", isActive, ImGuiWindowFlags_NoTitleBar);
    Image((void*)textureBrain, ImVec2((float)imageWidth, (float)imageHeight), ImVec2(0, 0)
    , ImVec2(1, 1), ImVec4(fade, fade, fade, fade));
    End();

    std::string rest(std::to_string(playerHp));
    rest.append(" / 100");
    SetNextWindowPos(ImVec2((float)Graphics::GetWindowWidth() - 200, 200));
    SetNextWindowSize(ImVec2(500, 500), 0);
    Begin("HEALTH", isActive, ImGuiWindowFlags_NoTitleBar);
    SetWindowFontScale(2.f);
    Text(rest.c_str());

    std::string cl(std::to_string(clues));
    cl.append("/" + std::to_string(options.difficulty * 2));
    Text(cl.c_str());

    End();
}

void GUIHandler::RenderOptionsMenu(OptionsHandler& options)
{
    SetNextWindowSize(ImVec2((float)Graphics::GetWindowWidth() * 0.65f, (float)Graphics::GetWindowHeight() * 0.65f));
    SetNextWindowPos(ImVec2((float)Graphics::GetWindowWidth() * 0.175f, (float)Graphics::GetWindowHeight() * 0.1f));
    SetNextWindowBgAlpha(0.5);
    bool* isActive = new bool(true);

    Begin("Options Menu", isActive, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    SetNextWindowPos(ImVec2((float)Graphics::GetWindowWidth() * 0.33f, (float)Graphics::GetWindowHeight() * 0.25f));
    
    BeginChild("SLIDERS", ImVec2((float)Graphics::GetWindowWidth() * 0.5f, (float)Graphics::GetWindowHeight() * 0.5f), true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
   
    SliderFloat("Master Volume", &options.masterVolume, 0.0f, 1.0f, "%.2f");
    SliderFloat("SFX Volume", &options.sfxVolume, 0.0f, 1.0f, "%.2f");
    SliderFloat("Ambient Volume", &options.ambientVolume, 0.0f, 1.0f, "%.2f");
    SliderFloat("Music Volume", &options.musicVolume, 0.0f, 1.0f, "%.2f");
    SliderFloat("Mouse Sensitivity", &options.mouseSens, 0.5f, 5.0f, "%.2f");

    std::string invSens = "Inverted Mouse: ";
    if (options.inverseSens)
        invSens.append("ON");
    else
        invSens.append("OFF");

    Checkbox(invSens.c_str(), &options.inverseSens);

    
    if (options.state == MAINMENU)
    {
        SliderInt("Difficulty", &options.difficulty, 1, 5);

        std::string timeC = "In-game Timer: ";

        // Displays ON or OFF depending on the state.
        if (this->clockTimer)
            timeC.append("ON");
        else
            timeC.append("OFF");
        Checkbox(timeC.c_str(), &clockTimer);
    }

    std::string isBlur = "Blur: ";
    if (options.hasBlur)
        isBlur.append("ON");
    else
        isBlur.append("OFF");
    
    Checkbox(isBlur.c_str(), &options.hasBlur);

    std::string isParticles = "Particles: ";
    if (options.hasParticles)
        isParticles.append("ON");
    else
        isParticles.append("OFF");

    Checkbox(isParticles.c_str(), &options.hasParticles);

    std::string isDebugDisplay = "Debug Info: ";
    if (options.hasDebugInfo)
        isDebugDisplay.append("ON");
    else
        isDebugDisplay.append("OFF");

    Checkbox(isDebugDisplay.c_str(), &options.hasDebugInfo);
    Text("\n\nTip:");
    Text("Difficulty will change the cooldown time for trap placement.\nBut also increase the amount of clues needed to be collected!\n\nRumors are there is also secret difficulty options somewhere!");

    // Ultra epic space creator for the aesthetics
    Text("\n\n\n\n\n\n\n\n\n");
    Text("Press 'Escape' to return to main menu.");
    EndChild();

    End();

    delete isActive;
}


void GUIHandler::RenderPauseMenu()
{
    SetNextWindowSize(ImVec2((float)Graphics::GetWindowWidth() * 0.5f, (float)Graphics::GetWindowHeight() * 0.5f));
    SetNextWindowPos(ImVec2((float)Graphics::GetWindowWidth() * 0.25f, (float)Graphics::GetWindowHeight() * 0.25f));
    SetNextWindowBgAlpha(0.5);
    bool* isActive = new bool;

    Begin("Pause Menu", isActive, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

        SetNextWindowPos(ImVec2((float)Graphics::GetWindowWidth() * 0.5f - 125, (float)Graphics::GetWindowHeight() * 0.33f));
        BeginChild("Resume Button", ImVec2(250, 50), isActive, ImGuiWindowFlags_NoTitleBar);
        SetWindowFontScale(1.5f);
        if(Button("Resume", ImVec2(250, 50)))
        {
            ResumeGame();
        }
        EndChild();

        SetNextWindowPos(ImVec2((float)Graphics::GetWindowWidth() * 0.5f - 125, (float)Graphics::GetWindowHeight() * 0.45f));
        BeginChild("Options Button", ImVec2(250, 50), isActive, ImGuiWindowFlags_NoTitleBar);
        SetWindowFontScale(1.5f);
        if (Button("Options", ImVec2(250, 50)))
        {
            this->showOptionsMenu = true;
            this->showGameGUI = false;
            this->showPauseMenu = false;
        }
        EndChild();


        SetNextWindowPos(ImVec2((float)Graphics::GetWindowWidth() * 0.5f - 125, (float)Graphics::GetWindowHeight() * 0.6f));
        BeginChild("Quit Button", ImVec2(250, 50), isActive, ImGuiWindowFlags_NoTitleBar);
        SetWindowFontScale(1.5f);
        if(Button("Quit", ImVec2(250, 50)))
        {
            QuitGame();
        }
        EndChild();

    End();

    delete isActive;
}

void GUIHandler::RenderMainMenu()
{
    bool* isActive = new bool;
    SetNextWindowPos(ImVec2((float)(Graphics::GetWindowWidth() / 2) - 125, (float)Graphics::GetWindowHeight() - 150));
    SetNextWindowSize(ImVec2(500, 600), 0);
    
    Begin("MENU", isActive, ImGuiWindowFlags_NoTitleBar);
    SetWindowFontScale(1.5f);
    Text("Press 'Space' to start game.");
    Text("Press 'P' to open Options.");
    Text("Press 'ESC' to quit game.");
    End();
    delete isActive;
}

void GUIHandler::QuitGame()
{
    GUIHandler::instance->shouldQuit = true;
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

