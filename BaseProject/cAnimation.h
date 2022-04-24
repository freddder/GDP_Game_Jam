#pragma once

#include "Keyframes.h"

class cAnimation
{
public:
	bool playing;
	float duration;
	float currentTime;
	float speed;
	bool repeat;
	bool isDone;

	cAnimation() 
	{
		playing = true;
		duration = 0.f;
		currentTime = 0.f;
		speed = 1.f;
		repeat = false;
		isDone = false;
	}
};