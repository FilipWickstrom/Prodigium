#include "InputHandler.h"
InputHandler* InputHandler::instance = nullptr;
InputHandler::InputHandler()
{
	if (!InputHandler::instance)
	{
		InputHandler::instance = new InputHandler;
	}
}
InputHandler::~InputHandler()
{
	if (InputHandler::instance)
	{
		delete InputHandler::instance;
	}
}
