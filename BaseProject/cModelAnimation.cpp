#include "cModelAnimation.h"

cModelAnimation::cModelAnimation(glm::vec3& _posRef, glm::vec3& _rotRef, glm::vec3& _sclRef) :
	positionRef(_posRef),
	rotationRef(_rotRef),
	scaleRef(_sclRef)
{
	initPosition = _posRef;
	initRotation = _rotRef;
	initScale = _sclRef;
}

void cModelAnimation::AddPositionKeyFrame(KeyFrameVec3 newKeyframe)
{
	if (newKeyframe.time > duration)
		duration = newKeyframe.time;

	positionKeyframes.push_back(newKeyframe);
}

void cModelAnimation::AddRotationKeyFrame(KeyFrameVec3 newKeyframe)
{
	if (newKeyframe.time > duration)
		duration = newKeyframe.time;

	rotationKeyframes.push_back(newKeyframe);
}

void cModelAnimation::AddScaleKeyFrame(KeyFrameVec3 newKeyframe)
{
	if (newKeyframe.time > duration)
		duration = newKeyframe.time;

	scaleKeyframes.push_back(newKeyframe);
}

void cModelAnimation::SetPositionRefValue(glm::vec3 newValue)
{
	positionRef = newValue;
}

void cModelAnimation::SetRotationRefValue(glm::vec3 newValue)
{
	rotationRef = newValue;
}

void cModelAnimation::SetScaleRefValue(glm::vec3 newValue)
{
	scaleRef = newValue;
}
