#pragma once
#include "UsefulHeader.h"
#include "Graphics.h"
#pragma warning(push, 0)
#include <Keyboard.h>
#include <Mouse.h>
#pragma warning(pop)
using namespace DirectX;
class InputHandler
{
private:
	InputHandler();
	virtual ~InputHandler();
	std::unique_ptr<Keyboard> keyboard;
	std::unique_ptr<Mouse> mouse;
	std::unique_ptr<Keyboard::KeyboardStateTracker> kBTracker;
	std::unique_ptr<Mouse::ButtonStateTracker> mouseTracker;
	Keyboard::State kbState;
	Mouse::State mouseState;
	static InputHandler* instance;
	MSG state;
	bool isRunning;
	bool confineCursor;

public:
	DELETE_COPY_ASSIGNMENT(InputHandler);
	static const bool Initialize();
	static void setWindow(const HWND& windowHandle);
	static const int GetMouseX();
	static const int GetMouseY();
	static void UpdateKeyboardAndMouse();
	static const Keyboard::KeyboardStateTracker* GetKBStateTracker();
	static const Mouse::ButtonStateTracker* getMouseStateTracker();
	static const Mouse::Mode getMouseMode();
	static const bool IsKeyPressed(const Keyboard::Keys& key);
	static const bool IsKeyHeld(const Keyboard::Keys& key);
	static const bool IsKeyReleased(const Keyboard::Keys& key);
	static const bool IsLMBPressed();
	static const bool IsLMBHeld();
	static const bool IsRMBPressed();
	static const bool IsRMBHeld();
	static const bool IsMMBPressed();
	static const bool IsMMBHeld();
	static const bool IsRunning();
	static void Destroy();
	static void HandleMessages();
	static LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

