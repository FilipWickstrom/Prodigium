#include "Test.h"

Test::Test()
	:Resource(ResourceType::TEXTURE)
{
	Load();
}

Test::~Test()
{
}

bool Test::Load()
{
	std::cout << "I AM A TEST OBJECT" << std::endl;

	return true;
}
