#include "cSpriteAnimation.h"

cSpriteAnimation::cSpriteAnimation(std::string& _spriteRef) :
	spriteRef(_spriteRef)
{
	initName = _spriteRef;
}

void cSpriteAnimation::AddKeyFrame(KeyFrameString newKeyframe)
{
	if (newKeyframe.time > duration)
		duration = newKeyframe.time;

	keyframes.push_back(newKeyframe);
}

void cSpriteAnimation::SetReferenceValue(std::string newValue)
{
	spriteRef = newValue;
}
