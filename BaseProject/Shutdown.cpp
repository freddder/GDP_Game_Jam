#include "globalThings.h"


// Deletes global things, etc. 
bool g_ShutDown(GLFWwindow* pWindow)
{

	delete ::g_pShaderManager;

	delete ::g_pVAOManager;

	delete ::g_pTextureManager;

	delete ::g_pTheLights;

	delete ::g_pFlyCamera;

	delete ::g_AnimationSystem;

	delete ::g_MapLoader;

	return true;
}