#pragma once
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <map>

struct Tile
{
	float height;
	glm::vec3 color;
	bool walkable;
};

class cMapLoader
{
	int decorationHeight;
	int decorationWidth;
	int collisionHeight;
	int collisionWidth;
	std::map<std::string, glm::vec3> colors;

public:
	Tile** map;

	cMapLoader();
	~cMapLoader();
	bool LoadMap(std::string decorationMapFileName, std::string collisionMapFileName, std::string& errorString);

	void Load_Grass_M_Offsets(std::vector<glm::vec4>& offsets);

	void Load_Trees_Offsets(std::vector<glm::vec4>& offsets);

	void Load_Water_Offsets(std::vector<glm::vec4>& offsets);

	void Load_Edge_T_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Edge_B_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Edge_L_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Edge_R_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Edge_BR_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Edge_BL_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Edge_BRC_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Edge_BLC_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Edge_TR_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Edge_TL_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Edge_TRC_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Edge_TLC_Offsets(std::vector<glm::vec4>& offsets);

	void Load_Path_M_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Path_T_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Path_B_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Path_L_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Path_R_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Path_BR_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Path_BL_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Path_BRC_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Path_BLC_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Path_TR_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Path_TL_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Path_TRC_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Path_TLC_Offsets(std::vector<glm::vec4>& offsets);

	void Load_Wall_B_Offsets(std::vector<glm::vec4>& offsets, 
							std::vector<glm::vec4>& edges_B_Offsets, 
							std::vector<glm::vec4>& edges_TLC_Offsets, 
							std::vector<glm::vec4>& edges_TRC_Offsets);
	void Load_Wall_R_Offsets(std::vector<glm::vec4>& offsets,
							std::vector<glm::vec4>& edges_R_Offsets,
							std::vector<glm::vec4>& edges_TLC_Offsets);
	void Load_Wall_L_Offsets(std::vector<glm::vec4>& offsets,
							std::vector<glm::vec4>& edges_L_Offsets,
							std::vector<glm::vec4>& edges_TRC_Offsets);
	void Load_Wall_BL_Offsets(std::vector<glm::vec4>& offsets,
								std::vector<glm::vec4>& edges_BL_Offsets);
	void Load_Wall_BR_Offsets(std::vector<glm::vec4>& offsets,
							std::vector<glm::vec4>& edges_BR_Offsets);

	void Load_Stair_R_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Stair_L_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Stair_U_Offsets(std::vector<glm::vec4>& offsets);
	void Load_Stair_D_Offsets(std::vector<glm::vec4>& offsets);

	void Load_Weed_Offsets(std::vector<glm::vec4>& offsets, std::vector<glm::vec4>& grassOffsets, float percent);

	void Load_Structure_Offsets(std::vector<glm::vec4>& offsets, std::string tileName);

	void PlacePlayer(glm::vec3& playerPosition);
};