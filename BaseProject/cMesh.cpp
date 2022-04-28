#include "cMesh.h"


cMesh::cMesh()
{
	this->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	this->orientationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	this->scale = glm::vec3(1.0f);

	this->bIsWireframe = false;

	this->objectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);	// White
	this->bUseObjectDebugColour = false;
	this->bDontLight = false;

	this->wholeObjectDiffuseRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);	// The "colour" of the object

	this->wholeObjectSpecularRGB = glm::vec3(1.0f, 1.0f, 1.0f);

	this->wholeObjectShininess_SpecPower = 1.0f;	// 1.0 to ??

	this->discardTexture = false;
	this->discardColor = glm::vec4(1.f, 1.f, 1.f, 1.f);

	this->discardBlack = false;

	this->isInstanced = false;
	this->instancedNumber = 1;

	// Default to 1.0f (all solid)
	this->alphaTransparency = 1.0f;

	// Clear all the textures
	for (unsigned int index = 0; index != cMesh::MAX_TEXTURES; index++)
	{
		this->textureNames[index] = "";
		this->textureRatios[index] = 0.0f;
	}
	this->textureNames[0] = "BrightColouredUVMap.bmp";
	this->textureRatios[0] = 1.0f;

	// Set these to an identiy matrix (in case we don't need them)
	this->matTranslateBack = glm::mat4(1.0f);
	this->matTranslateToLocalOrigin = glm::mat4(1.0f);

	// Assign a unique ID
	this->m_UniqueID = cMesh::m_NextID;
	cMesh::m_NextID++;

}


void cMesh::clearTextureRatiosToZero(void)
{
	for (unsigned int index = 0; index != cMesh::MAX_TEXTURES; index++)
	{
		this->textureRatios[index] = 0.0f;
	}
	return;
}


unsigned int cMesh::getUniqueID(void)
{
	return this->m_UniqueID;
}

//static 
unsigned int cMesh::m_NextID = cMesh::INITIAL_UNIQUE_ID;

