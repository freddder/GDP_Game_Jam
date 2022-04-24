#include "cPlayer.h"
#include "globalThings.h"

cPlayer::cPlayer(cMesh* _model)
{
	model = _model;
	nextLeft = false;
	isMoving = false;
	lastDir = Directions::Down;
	currSpriteAnim = new cSpriteAnimation(model->textureNames[0]);
	currSpriteAnim->isDone = true;
	currModelAnim = new cModelAnimation(model->positionXYZ, model->orientationXYZ, model->scale);
	currModelAnim->isDone = true;

	g_AnimationSystem->AddModelAnimation(currModelAnim);
	g_AnimationSystem->AddSpriteAnimation(currSpriteAnim);
}

cPlayer::~cPlayer()
{
	delete currSpriteAnim;
	delete currModelAnim;
}

void cPlayer::Walk(Directions direction)
{
	isMoving = !(currModelAnim->isDone && currSpriteAnim->isDone);

	if (!isMoving) // currently not moving
	{
		if (direction == Directions::NONE)
		{
			if(lastDir == Directions::Down)
				model->textureNames[0] = "Nate_Idle_Down.bmp";
			else if (lastDir == Directions::Up)
				model->textureNames[0] = "Nate_Idle_Up.bmp";
			else if (lastDir == Directions::Right)
				model->textureNames[0] = "Nate_Idle_Right.bmp";
			else if (lastDir == Directions::Left)
				model->textureNames[0] = "Nate_Idle_Left.bmp";
		}
		else
		{
			currSpriteAnim->isDone = false;
			currSpriteAnim->currentTime = 0.f;
			currSpriteAnim->keyframes.clear();
			currSpriteAnim->initName = model->textureNames[0];
			currSpriteAnim->duration = 0.f;

			currModelAnim->isDone = false;
			currModelAnim->currentTime = 0.f;
			currModelAnim->positionKeyframes.clear();
			currModelAnim->rotationKeyframes.clear();
			currModelAnim->scaleKeyframes.clear();
			currModelAnim->initPosition = model->positionXYZ;
			currModelAnim->initRotation = model->orientationXYZ;
			currModelAnim->initScale = model->scale;
			currModelAnim->duration = 0.f;
			
			if (direction == Directions::Down)
			{
				glm::vec3 currLocation = model->positionXYZ;
				currLocation.z += 1;

				if (g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].walkable)
				{
					int heightDiff = round(g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].height - currLocation.y);
					currLocation.y += heightDiff;

					if (heightDiff == 1 || heightDiff == -1 || heightDiff == 0)
						currModelAnim->AddPositionKeyFrame(KeyFrameVec3(0.3f, currLocation));
				}

				if (nextLeft) currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Walk_Down_1.bmp"));
				else currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Walk_Down_2.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.2f, "Nate_Idle_Down.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.3f, "Nate_Idle_Down.bmp"));

				nextLeft = !nextLeft;
				lastDir = Directions::Down;
			}
			else if (direction == Directions::Up)
			{
				glm::vec3 currLocation = model->positionXYZ;
				currLocation.z -= 1;

				if (g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].walkable)
				{
					int heightDiff = round(g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].height - currLocation.y);
					currLocation.y += heightDiff;

					if (heightDiff == 1 || heightDiff == -1 || heightDiff == 0)
						currModelAnim->AddPositionKeyFrame(KeyFrameVec3(0.3f, currLocation));
				}

				if (nextLeft) currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Walk_Up_1.bmp"));
				else currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Walk_Up_2.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.2f, "Nate_Idle_Up.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.3f, "Nate_Idle_Up.bmp"));

				nextLeft = !nextLeft;
				lastDir = Directions::Up;
			}
			else if (direction == Directions::Left)
			{
				glm::vec3 currLocation = model->positionXYZ;
				currLocation.x -= 1;

				if (g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].walkable)
				{
					int heightDiff = round(g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].height - currLocation.y);
					currLocation.y += heightDiff;

					if (heightDiff == 1 || heightDiff == -1 || heightDiff == 0)
						currModelAnim->AddPositionKeyFrame(KeyFrameVec3(0.3f, currLocation));
				}

				if (nextLeft) currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Walk_Left_1.bmp"));
				else currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Walk_Left_2.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.2f, "Nate_Idle_Left.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.3f, "Nate_Idle_Left.bmp"));

				nextLeft = !nextLeft;
				lastDir = Directions::Left;
			}
			else if (direction == Directions::Right)
			{
				glm::vec3 currLocation = model->positionXYZ;
				currLocation.x += 1;

				if (g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].walkable)
				{
					int heightDiff = round(g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].height - currLocation.y);
					currLocation.y += heightDiff;

					if(heightDiff == 1 || heightDiff == -1 || heightDiff == 0)
						currModelAnim->AddPositionKeyFrame(KeyFrameVec3(0.3f, currLocation));
				}

				if (nextLeft) currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Walk_Right_1.bmp"));
				else currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Walk_Right_2.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.2f, "Nate_Idle_Right.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.3f, "Nate_Idle_Right.bmp"));

				nextLeft = !nextLeft;
				lastDir = Directions::Right;
			}
		}		
	}
}

void cPlayer::Run(Directions direction)
{
	isMoving = !(currModelAnim->isDone && currSpriteAnim->isDone);

	if (!isMoving) // currently not moving
	{
		if (direction == Directions::NONE)
		{
			if (lastDir == Directions::Down)
				model->textureNames[0] = "Nate_Idle_Down.bmp";
			else if (lastDir == Directions::Up)
				model->textureNames[0] = "Nate_Idle_Up.bmp";
			else if (lastDir == Directions::Right)
				model->textureNames[0] = "Nate_Idle_Right.bmp";
			else if (lastDir == Directions::Left)
				model->textureNames[0] = "Nate_Idle_Left.bmp";
		}
		else
		{
			currSpriteAnim->isDone = false;
			currSpriteAnim->currentTime = 0.f;
			currSpriteAnim->keyframes.clear();
			currSpriteAnim->initName = model->textureNames[0];
			currSpriteAnim->duration = 0.f;

			currModelAnim->isDone = false;
			currModelAnim->currentTime = 0.f;
			currModelAnim->positionKeyframes.clear();
			currModelAnim->rotationKeyframes.clear();
			currModelAnim->scaleKeyframes.clear();
			currModelAnim->initPosition = model->positionXYZ;
			currModelAnim->initRotation = model->orientationXYZ;
			currModelAnim->initScale = model->scale;
			currModelAnim->duration = 0.f;

			if (direction == Directions::Down)
			{
				glm::vec3 currLocation = model->positionXYZ;
				currLocation.z += 1;

				if (g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].walkable)
				{
					int heightDiff = round(g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].height - currLocation.y);
					currLocation.y += heightDiff;

					if (heightDiff == 1 || heightDiff == -1 || heightDiff == 0)
						currModelAnim->AddPositionKeyFrame(KeyFrameVec3(0.17f, currLocation));
				}

				if (nextLeft) currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Run_Down_1.bmp"));
				else currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Run_Down_2.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.1f, "Nate_Run_Down_Middle.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.17f, "Nate_Run_Down_Middle.bmp"));

				nextLeft = !nextLeft;
				lastDir = Directions::Down;
			}
			else if (direction == Directions::Up)
			{
				glm::vec3 currLocation = model->positionXYZ;
				currLocation.z -= 1;

				if (g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].walkable)
				{
					int heightDiff = round(g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].height - currLocation.y);
					currLocation.y += heightDiff;

					if (heightDiff == 1 || heightDiff == -1 || heightDiff == 0)
						currModelAnim->AddPositionKeyFrame(KeyFrameVec3(0.17f, currLocation));
				}

				if (nextLeft) currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Run_Up_1.bmp"));
				else currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Run_Up_2.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.1f, "Nate_Run_Up_Middle.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.17f, "Nate_Run_Up_Middle.bmp"));

				nextLeft = !nextLeft;
				lastDir = Directions::Up;
			}
			else if (direction == Directions::Left)
			{
				glm::vec3 currLocation = model->positionXYZ;
				currLocation.x -= 1;

				if (g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].walkable)
				{
					int heightDiff = round(g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].height - currLocation.y);
					currLocation.y += heightDiff;

					if (heightDiff == 1 || heightDiff == -1 || heightDiff == 0)
						currModelAnim->AddPositionKeyFrame(KeyFrameVec3(0.17f, currLocation));
				}

				if (nextLeft) currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Run_Left_1.bmp"));
				else currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Run_Left_2.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.1f, "Nate_Run_Left_Middle.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.17f, "Nate_Run_Left_Middle.bmp"));

				nextLeft = !nextLeft;
				lastDir = Directions::Left;
			}
			else if (direction == Directions::Right)
			{
				glm::vec3 currLocation = model->positionXYZ;
				currLocation.x += 1;

				if (g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].walkable)
				{
					int heightDiff = round(g_MapLoader->map[-(int)currLocation.z][(int)currLocation.x].height - currLocation.y);
					currLocation.y += heightDiff;

					if (heightDiff == 1 || heightDiff == -1 || heightDiff == 0)
						currModelAnim->AddPositionKeyFrame(KeyFrameVec3(0.17f, currLocation));
				}

				if (nextLeft) currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Run_Right_1.bmp"));
				else currSpriteAnim->AddKeyFrame(KeyFrameString(0.01f, "Nate_Run_Right_2.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.1f, "Nate_Run_Right_Middle.bmp"));
				currSpriteAnim->AddKeyFrame(KeyFrameString(0.17f, "Nate_Run_Right_Middle.bmp"));

				nextLeft = !nextLeft;
				lastDir = Directions::Right;
			}
		}
	}
}

glm::vec3 cPlayer::GetPosition()
{
	return model->positionXYZ;
}
