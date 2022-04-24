#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

enum EasingType
{
	EaseIn,
	EaseOut,
	EaseInOut,
	None
};

struct KeyFrameVec3
{
	KeyFrameVec3() { easingType = None; }
	KeyFrameVec3(float time, const glm::vec3& value, EasingType easingType = EasingType::None)
		: time(time), value(value), easingType(easingType) {}
	float time;
	glm::vec3 value;
	EasingType easingType;
};

struct KeyFrameString
{
	KeyFrameString();
	KeyFrameString(float time, const std::string& value, EasingType easingType = EasingType::None)
		: time(time), value(value) {}
	float time;
	std::string value;
};