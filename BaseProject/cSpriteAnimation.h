#pragma once

#include "cAnimation.h"
#include <vector>

class cSpriteAnimation : public cAnimation
{
	std::string& spriteRef;
public:
	std::vector<KeyFrameString> keyframes; // dont call pushback
	cSpriteAnimation(std::string& _spriteRef);
	void AddKeyFrame(KeyFrameString newKeyframe);
	void SetReferenceValue(std::string newValue);

	std::string initName;
};