#pragma once
#include "MeshObject.h"
class Enemy
{
private:
	MeshObject* model;
	float speed;
public:
	Enemy();
	virtual ~Enemy();
	void Move();
	void Rotate();
	void FindPlayer();

};

