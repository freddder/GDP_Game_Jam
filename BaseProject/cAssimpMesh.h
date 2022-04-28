#pragma once

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include "sVertex_types.h"
#include <glad/glad.h>
#include <vector>
#include <map>

class cAssimpMesh
{
	Assimp::Importer m_AssimpImporter;
public:

	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 scale;

	bool isInstanced;
	unsigned int instancedNumber;

	unsigned int numMeshes;
	std::vector<sModelDrawInfo> allMeshesInfo;
	std::vector<std::string> texturesUsed;

	cAssimpMesh();
	~cAssimpMesh();

	void LoadModel(std::string fileName);
	void LoadModelIntoVAO(GLuint shaderProgramID, std::vector<glm::vec4>& offsets);
	void DrawObject(GLuint shaderProgramID);
};