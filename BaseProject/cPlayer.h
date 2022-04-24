#pragma once
#include "cMesh.h"
#include "cSpriteAnimation.h"
#include "cModelAnimation.h"

enum Directions
{
	NONE,
	Up,
	Down,
	Left,
	Right
};

class cPlayer
{
public:

	cMesh* model;
	bool nextLeft;
	bool isMoving;
	Directions lastDir;
	cSpriteAnimation* currSpriteAnim;
	cModelAnimation* currModelAnim;

	cPlayer(cMesh* _model);
	~cPlayer();

	void Walk(Directions direction);
	void Run(Directions direction);
	glm::vec3 GetPosition();
};