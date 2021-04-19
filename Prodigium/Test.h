#pragma once
#include "Resource.h"
#include <iostream>

class Test : public Resource
{
private:

public:
	Test();
	virtual ~Test();

	// Inherited via Resource
	virtual bool Load() override;
};