#pragma once

#include "cSpriteAnimation.h"
#include "cModelAnimation.h"

class cAnimationSystem
{
	std::vector<cSpriteAnimation*> spriteAnimations;
	std::vector<cModelAnimation*> modelAnimations;
public:

	void Process(float deltaTime);
	void AddSpriteAnimation(cSpriteAnimation* newAnimation);
	void AddModelAnimation(cModelAnimation* newAnimation);
};