#include "cAnimationSystem.h"
#include <glm/gtx/easing.hpp>

void cAnimationSystem::Process(float deltaTime)
{
	// Update sprite animations
	for (unsigned int animationIndex = 0; animationIndex < spriteAnimations.size(); animationIndex++)
	{
		if (spriteAnimations[animationIndex]->isDone)
			continue;

		spriteAnimations[animationIndex]->currentTime += deltaTime * spriteAnimations[animationIndex]->speed;

		for (unsigned int keyFrameIndex = 0; keyFrameIndex < spriteAnimations[animationIndex]->keyframes.size(); keyFrameIndex++)
		{
			if (spriteAnimations[animationIndex]->keyframes[keyFrameIndex].time >= spriteAnimations[animationIndex]->currentTime)
			{
				// no interpolation I guess
				if(keyFrameIndex != 0)
					spriteAnimations[animationIndex]->SetReferenceValue(spriteAnimations[animationIndex]->keyframes[keyFrameIndex - 1].value);
				else
					spriteAnimations[animationIndex]->SetReferenceValue(spriteAnimations[animationIndex]->initName);

				break;
			}
		}

		if (spriteAnimations[animationIndex]->currentTime >= spriteAnimations[animationIndex]->duration)
		{
			if (spriteAnimations[animationIndex]->repeat)
				spriteAnimations[animationIndex]->currentTime = 0.f;
			else
				spriteAnimations[animationIndex]->isDone = true;
		}
	}

	// Update model animations
	for (unsigned int animationIndex = 0; animationIndex < modelAnimations.size(); animationIndex++)
	{
		if (modelAnimations[animationIndex]->isDone)
			continue;

		modelAnimations[animationIndex]->currentTime += deltaTime * modelAnimations[animationIndex]->speed;

		// Position
		KeyFrameVec3 currPosKeyframe;
		KeyFrameVec3 nextPosKeyframe;
		float posFraction = 1;
		for (unsigned int keyFrameIndex = 0; keyFrameIndex < modelAnimations[animationIndex]->positionKeyframes.size(); keyFrameIndex++)
		{
			if (modelAnimations[animationIndex]->positionKeyframes[keyFrameIndex].time >= modelAnimations[animationIndex]->currentTime)
			{
				nextPosKeyframe = modelAnimations[animationIndex]->positionKeyframes[keyFrameIndex];

				if(keyFrameIndex != 0)
					currPosKeyframe = modelAnimations[animationIndex]->positionKeyframes[keyFrameIndex - 1];
				else
				{
					currPosKeyframe.time = 0.f;
					currPosKeyframe.value = modelAnimations[animationIndex]->initPosition;
				}

				posFraction = (modelAnimations[animationIndex]->currentTime - currPosKeyframe.time) / (nextPosKeyframe.time - currPosKeyframe.time);

				switch (nextPosKeyframe.easingType)
				{
				case EasingType::EaseIn :
					posFraction = glm::sineEaseIn(posFraction);
					break;
				case EasingType::EaseOut:
					posFraction = glm::sineEaseOut(posFraction);
					break;
				case EasingType::EaseInOut:
					posFraction = glm::sineEaseInOut(posFraction);
					break;
				default:
					break;
				}

				glm::vec3 newPosition = currPosKeyframe.value + (nextPosKeyframe.value - currPosKeyframe.value) * posFraction;
				modelAnimations[animationIndex]->SetPositionRefValue(newPosition);

				break;
			}
		}

		if (modelAnimations[animationIndex]->positionKeyframes.size() != 0)
		{
			if (modelAnimations[animationIndex]->currentTime >= modelAnimations[animationIndex]->duration)
			{
				glm::vec3 finalPosition = modelAnimations[animationIndex]->positionKeyframes[modelAnimations[animationIndex]->positionKeyframes.size() - 1].value;
				modelAnimations[animationIndex]->SetPositionRefValue(finalPosition);
			}
		}

		// Orientation


		// Scale
		KeyFrameVec3 currScaleKeyframe;
		KeyFrameVec3 nextScaleKeyframe;
		float scaleFraction = 1;
		for (unsigned int keyFrameIndex = 0; keyFrameIndex < modelAnimations[animationIndex]->scaleKeyframes.size(); keyFrameIndex++)
		{
			if (modelAnimations[animationIndex]->scaleKeyframes[keyFrameIndex].time >= modelAnimations[animationIndex]->currentTime)
			{
				nextScaleKeyframe = modelAnimations[animationIndex]->scaleKeyframes[keyFrameIndex];

				if (keyFrameIndex != 0)
					currScaleKeyframe = modelAnimations[animationIndex]->scaleKeyframes[keyFrameIndex - 1];
				else
				{
					currScaleKeyframe.time = 0.f;
					currScaleKeyframe.value = modelAnimations[animationIndex]->initScale;
				}

				scaleFraction = (modelAnimations[animationIndex]->currentTime - currScaleKeyframe.time) / (nextScaleKeyframe.time - currScaleKeyframe.time);

				switch (nextScaleKeyframe.easingType)
				{
				case EasingType::EaseIn:
					scaleFraction = glm::sineEaseIn(scaleFraction);
					break;
				case EasingType::EaseOut:
					scaleFraction = glm::sineEaseOut(scaleFraction);
					break;
				case EasingType::EaseInOut:
					scaleFraction = glm::sineEaseInOut(scaleFraction);
					break;
				default:
					break;
				}

				glm::vec3 newScale = currScaleKeyframe.value + (nextScaleKeyframe.value - currScaleKeyframe.value) * scaleFraction;
				modelAnimations[animationIndex]->SetScaleRefValue(newScale);

				break;
			}
		}

		if (modelAnimations[animationIndex]->scaleKeyframes.size() != 0)
		{
			if (modelAnimations[animationIndex]->currentTime >= modelAnimations[animationIndex]->duration)
			{
				glm::vec3 finalScale = modelAnimations[animationIndex]->scaleKeyframes[modelAnimations[animationIndex]->scaleKeyframes.size() - 1].value;
				modelAnimations[animationIndex]->SetScaleRefValue(finalScale);
			}
		}


		if (modelAnimations[animationIndex]->currentTime >= modelAnimations[animationIndex]->duration)
		{
			//glm::vec3 finalPosition;
			//modelAnimations[animationIndex]->SetPositionRefValue();

			if (modelAnimations[animationIndex]->repeat)
				modelAnimations[animationIndex]->currentTime = 0.f;
			else
				modelAnimations[animationIndex]->isDone = true;
		}
	}
}

void cAnimationSystem::AddSpriteAnimation(cSpriteAnimation* newAnimation)
{
	spriteAnimations.push_back(newAnimation);
}

void cAnimationSystem::AddModelAnimation(cModelAnimation* newAnimation)
{
	modelAnimations.push_back(newAnimation);
}
