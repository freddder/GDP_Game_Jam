#pragma once

// This represents a single mesh object we are drawing

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <vector>

class cMesh
{
public:
	cMesh();
	std::string meshName;		// The 3D Mesh model we are drawing

	std::string friendlyName;
	unsigned int friendlyID;

	glm::vec3 positionXYZ;
	glm::vec3 orientationXYZ;		// "angle"
	glm::vec3 scale;
	
	glm::mat4 matModelBaseOrigin;
	glm::mat4 matTranslateToLocalOrigin;
	glm::mat4 matTranslateBack;

	glm::vec4 objectDebugColourRGBA;
	bool bUseObjectDebugColour;
	bool bDontLight;

	bool bIsWireframe;

	glm::vec4 wholeObjectDiffuseRGBA;		// The "colour" of the object
	bool bUseWholeObjectDiffuseColour;		// If true, then wholeObject colour (not model vertex) is used

	glm::vec3 wholeObjectSpecularRGB;		// Specular HIGHLIGHT colour (usually the same as the light, or white)
	float wholeObjectShininess_SpecPower;	// 1.0 to ??

	bool discardTexture;
	glm::vec4 discardColor;

	bool discardBlack;

	bool isInstanced;
	unsigned int instancedNumber;

	// Alpha Transparency (0.0 to 1.0f)
	float alphaTransparency;

	unsigned int getUniqueID(void);

	// All the "child" mesh objects
	std::vector< cMesh* > vec_pChildMeshes;

	// If you only have 1 texture per object...
	//std::string textureName;
	// OR...
	// Your object can have how every many textures you want...
	//std::vector< std::string > vecTextures;
	static const unsigned int MAX_TEXTURES = 8;
	std::string textureNames[MAX_TEXTURES];
	float textureRatios[MAX_TEXTURES];

	void clearTextureRatiosToZero(void);

private:
	unsigned int m_UniqueID;
	static unsigned int m_NextID;
	// All our unique objects start at 9999
	const static unsigned int INITIAL_UNIQUE_ID = 9999;
};
