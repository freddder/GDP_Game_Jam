#ifndef _cVAOManager_HG_
#define _cVAOManager_HG_

// Will load the models and place them 
// into the vertex and index buffers to be drawn

#include <string>
#include <map>
#include <vector>
#include <glm/vec4.hpp>

#include "sVertex_types.h"


class cVAOManager
{
public:

	bool LoadModelIntoVAO(std::string fileName,
		sModelDrawInfo& drawInfo,
		unsigned int shaderProgramID,
		std::vector<glm::vec4>& offsets);

	// Same as above, but doesn't load into the VAO
	// i.e. you'd call these in two steps, passing the filename in the drawInfo.meshName
	bool LoadPLYModelFromFile(std::string fileName, sModelDrawInfo& drawInfo);
	// Uses a populated drawInfo structure, with the filename in the drawInfo.meshName
	bool LoadModelIntoVAO(sModelDrawInfo& drawInfo, unsigned int shaderProgramID, std::vector<glm::vec4>& offsets);

	// We don't want to return an int, likely
	bool FindDrawInfoByModelName(std::string filename,
		sModelDrawInfo& drawInfo);

	std::string getLastError(bool bAndClear = true);

	void setFilePath(std::string filePath);
	std::string getFilePath(void);

	// This is modified from GenerateSphericalTextureCoords.cpp file,
	//	specifically that it's using the sModelDrawInfo structure
	enum enumTEXCOORDBIAS {
		POSITIVE_X, POSITIVE_Y, POSITIVE_Z
	};
	void GenerateSphericalTextureCoords(
		enumTEXCOORDBIAS uBias, enumTEXCOORDBIAS vBias,
		sModelDrawInfo& drawInfo, bool basedOnNormals, float scale, bool fast);
	// Defaults to POSITIVE_X, POSITIVE_Y
	void GenerateSphericalTextureCoords(
		sModelDrawInfo& drawInfo, bool basedOnNormals, float scale, bool fast);

	// Added November 24, 2021: To handle the cylindrical projection UV mapping for applying planet textures to spheres
	// This projects on a cylinder that has an axis along the y-axis.
	void GenerateCylindricalTextureCoords_FromNormals(sModelDrawInfo& drawInfo);

	// Added November 24, 2021: To save the planet model so we can reload it
	// The ".ply" is added automatically.
	bool SaveAsPlyFile(sModelDrawInfo& drawInfo, std::string fileName, bool bOverwrite,
		bool bSaveRGB, bool bSaveNormals, bool bSaveUVs,
		std::string& error);

private:

	std::map< std::string /*model name*/,
		sModelDrawInfo /* info needed to draw*/ >
		m_map_ModelName_to_VAOID;

	// This will be added to the path when a file is loaded
	std::string m_FilePath;

	std::string m_lastError;
};

#endif	// _cVAOManager_HG_
