#pragma once

#include "cAnimation.h"
#include <vector>

class cModelAnimation : public cAnimation
{
	glm::vec3& positionRef;
	glm::vec3& rotationRef;
	glm::vec3& scaleRef;

public:
	glm::vec3 initPosition;
	glm::vec3 initRotation;
	glm::vec3 initScale;

	std::vector<KeyFrameVec3> positionKeyframes;
	std::vector<KeyFrameVec3> rotationKeyframes;
	std::vector<KeyFrameVec3> scaleKeyframes;

	cModelAnimation(glm::vec3& _posRef, glm::vec3& _rotRef, glm::vec3& _sclRef);
	void AddPositionKeyFrame(KeyFrameVec3 newKeyframe);
	void AddRotationKeyFrame(KeyFrameVec3 newKeyframe);
	void AddScaleKeyFrame(KeyFrameVec3 newKeyframe);
	void SetPositionRefValue(glm::vec3 newValue);
	void SetRotationRefValue(glm::vec3 newValue);
	void SetScaleRefValue(glm::vec3 newValue);
};