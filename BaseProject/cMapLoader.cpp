#define _CRT_SECURE_NO_DEPRECATE
#include "cMapLoader.h"
#include <sstream>
#include <iostream>

cMapLoader::cMapLoader()
{
    colors.insert(std::pair<std::string, glm::vec3>("grass", glm::vec3(0.f, 255.f, 0.f)));
    colors.insert(std::pair<std::string, glm::vec3>("tree", glm::vec3(34.f, 177.f, 76.f)));
    colors.insert(std::pair<std::string, glm::vec3>("path", glm::vec3(255.f, 255.f, 178.f)));
    colors.insert(std::pair<std::string, glm::vec3>("edge", glm::vec3(253.f, 101.f, 0.f)));
    colors.insert(std::pair<std::string, glm::vec3>("water", glm::vec3(0.f, 162.f, 232.f)));
    colors.insert(std::pair<std::string, glm::vec3>("stair", glm::vec3(126.f, 91.f, 65.f)));
    colors.insert(std::pair<std::string, glm::vec3>("house", glm::vec3(200.f, 191.f, 231.f)));
    colors.insert(std::pair<std::string, glm::vec3>("lab", glm::vec3(255.f, 255.f, 0.f)));
    colors.insert(std::pair<std::string, glm::vec3>("church", glm::vec3(63.f, 72.f, 204.f)));
    colors.insert(std::pair<std::string, glm::vec3>("player", glm::vec3(100.f, 100.f, 100.f)));
}

cMapLoader::~cMapLoader()
{
    for (int i = 0; i < decorationHeight; ++i) 
    {
        delete[] map[i];
    }
    delete[] map;
}

bool cMapLoader::LoadMap(std::string decorationMapFileName, std::string collisionMapFileName)
{
    // Load decoration
    {
        std::string decFilePath = "assets/maps/" + decorationMapFileName;
        FILE* decFile = fopen(decFilePath.c_str(), "rb");

        if (decFile == NULL)
            throw "Argument Exception";

        unsigned char decInfo[54];
        fread(decInfo, sizeof(unsigned char), 54, decFile); // read the 54-byte header

        // extract image height and width from header
        decorationWidth = *(int*)&decInfo[18];
        decorationHeight = *(int*)&decInfo[22];

        map = new Tile * [decorationHeight];
        for (int i = 0; i < decorationHeight; i++)
            map[i] = new Tile[decorationWidth];

        std::cout << std::endl;
        std::cout << "Width of decoration map: " << decorationWidth << std::endl;
        std::cout << "Height of decoration map: " << decorationHeight << std::endl;
        std::cout << std::endl;

        int row_padded = (decorationWidth * 3 + 3) & (~3);
        unsigned char* data = new unsigned char[row_padded];
        unsigned char tmp;

        for (int i = 0; i < decorationHeight; i++)
        {
            fread(data, sizeof(unsigned char), row_padded, decFile);
            int widthCounter = 0;
            for (int j = 0; j < decorationWidth * 3; j += 3)
            {
                // Convert (B, G, R) to (R, G, B)
                tmp = data[j];
                data[j] = data[j + 2];
                data[j + 2] = tmp;
                
                map[i][widthCounter].color = glm::vec3((int)data[j], (int)data[j + 1], (int)data[j + 2]);
                widthCounter++;
            }
        }

        fclose(decFile);
    }

    //*****************************************************************************

    // Load collision
    {
        std::string colFilePath = "assets/maps/" + collisionMapFileName;
        FILE* colFile = fopen(colFilePath.c_str(), "rb");

        if (colFile == NULL)
            throw "Argument Exception";

        unsigned char colInfo[54];
        fread(colInfo, sizeof(unsigned char), 54, colFile); // read the 54-byte header

        // extract image height and width from header
        collisionWidth = *(int*)&colInfo[18];
        collisionHeight = *(int*)&colInfo[22];

        if (collisionWidth != decorationWidth || collisionHeight != decorationHeight)
        {
            fclose(colFile);
            return false;
        }

        std::cout << std::endl;
        std::cout << "Width of collision map: " << collisionWidth << std::endl;
        std::cout << "Height of collision map: " << collisionHeight << std::endl;
        std::cout << std::endl;

        int row_padded = (decorationWidth * 3 + 3) & (~3);
        unsigned char* data = new unsigned char[row_padded];
        unsigned char tmp;

        for (int i = 0; i < collisionHeight; i++)
        {
            fread(data, sizeof(unsigned char), row_padded, colFile);
            int widthCounter = 0;
            for (int j = 0; j < collisionWidth * 3; j += 3)
            {
                // Convert (B, G, R) to (R, G, B)
                tmp = data[j];
                data[j] = data[j + 2];
                data[j + 2] = tmp;

                map[i][widthCounter].walkable = ((int)data[j] != 255); // red = 255
                map[i][widthCounter].height = ((int)data[j + 1] / 15); // +1 in Y for 15 in green (or blue for gray scales)
                widthCounter++;
            }
        }

        fclose(colFile);
    }

    return true;
}

void cMapLoader::Load_Grass_M_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["grass"] || // grass
                map[h][w].color == colors["tree"] ||
                map[h][w].color == colors["player"]) // tree
            {
                glm::vec4 newOffset;
                newOffset.x = w * 1.f;
                newOffset.y = map[h][w].height;
                newOffset.z = h * -1.f;
                newOffset.w = 1.f;

                offsets.push_back(newOffset);
            }
            else if (map[h][w].color == colors["edge"]) // edges
            {
                if (map[h][w + 1].color == colors["edge"] && map[h][w - 1].color == colors["edge"]) // right & left
                {
                    // get lowest between up and down
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    if(map[h + 1][w].height < map[h - 1][w].height)
                        newOffset.y = map[h + 1][w].height;
                    else
                        newOffset.y = map[h - 1][w].height;

                    offsets.push_back(newOffset);
                }
                else if (map[h + 1][w].color == colors["edge"] && map[h - 1][w].color == colors["edge"]) // up and down
                {
                    // get lowest between left and right
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    if (map[h][w + 1].height < map[h][w - 1].height)
                        newOffset.y = map[h][w + 1].height;
                    else
                        newOffset.y = map[h][w - 1].height;

                    offsets.push_back(newOffset);
                }
                else if ((map[h][w - 1].color == colors["edge"] && map[h + 1][w].color == colors["edge"]) || // left and up OR right and down
                         (map[h][w + 1].color == colors["edge"] && map[h - 1][w].color == colors["edge"]))
                {
                    // get lowest of bottom-right and top left
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    if (map[h - 1][w + 1].height < map[h + 1][w - 1].height)
                        newOffset.y = map[h - 1][w + 1].height;
                    else
                        newOffset.y = map[h + 1][w - 1].height;

                    offsets.push_back(newOffset);
                }
                else if ((map[h][w + 1].color == colors["edge"] && map[h + 1][w].color == colors["edge"]) || // right and up OR left and down
                    (map[h][w - 1].color == colors["edge"] && map[h - 1][w].color == colors["edge"]))
                {
                    // get lowest of bottom-left and top right
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    if (map[h - 1][w - 1].height < map[h + 1][w + 1].height)
                        newOffset.y = map[h - 1][w - 1].height;
                    else
                        newOffset.y = map[h + 1][w + 1].height;

                    offsets.push_back(newOffset);
                }
                else // single edge
                {
                    // check left-right or up-down should be enough -> it kinda wasnt
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    if (map[h][w - 1].height < map[h][w + 1].height)
                        newOffset.y = map[h][w - 1].height;
                    else if (map[h][w - 1].height > map[h][w + 1].height)
                        newOffset.y = map[h][w + 1].height;
                    else if (map[h + 1][w].height > map[h - 1][w].height)
                        newOffset.y = map[h - 1][w].height;
                    else if (map[h + 1][w].height < map[h - 1][w].height)
                        newOffset.y = map[h + 1][w].height;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Trees_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["tree"])
            {
                glm::vec4 newOffset;
                newOffset.x = w * 1.f;
                newOffset.y = map[h][w].height -0.4f;
                newOffset.z = h * -1.f;
                newOffset.w = 1.f;

                offsets.push_back(newOffset);
            }
        }
    }
}

void cMapLoader::Load_Water_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["water"])
            {
                glm::vec4 newOffset;
                newOffset.x = w * 1.f;
                newOffset.y = map[h][w].height;
                newOffset.z = h * -1.f;
                newOffset.w = 1.f;

                offsets.push_back(newOffset);
            }
            else if (map[h][w].color == colors["edge"])
            {
                if (map[h + 1][w].color == colors["water"]) // if up is water
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h + 1][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h - 1][w].color == colors["water"]) // if down is water
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h - 1][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h][w + 1].color == colors["water"]) // if right is water
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w + 1].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h][w - 1].color == colors["water"]) // if left is water
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w - 1].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h + 1][w + 1].color == colors["water"]) // if topright is water
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h + 1][w + 1].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h + 1][w - 1].color == colors["water"]) // if topleft is water
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h + 1][w - 1].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h - 1][w + 1].color == colors["water"]) // if bottomright is water
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h - 1][w + 1].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h - 1][w - 1].color == colors["water"]) // if bottomleft is water
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h - 1][w - 1].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Edge_T_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["edge"])
            {
                if (map[h][w + 1].color == colors["edge"] && // right
                    map[h][w - 1].color == colors["edge"] && // left
                    map[h][w].height == map[h - 1][w].height) // height of bottom
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h][w - 1].color == colors["edge"] &&
                    map[h][w + 1].color != colors["edge"] &&
                    map[h + 1][w].color != colors["edge"] &&
                    map[h - 1][w].color != colors["edge"] &&
                    map[h - 1][w].height == map[h][w].height) // coming from left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h][w + 1].color == colors["edge"] &&
                    map[h][w - 1].color != colors["edge"] &&
                    map[h + 1][w].color != colors["edge"] &&
                    map[h - 1][w].color != colors["edge"] &&
                    map[h - 1][w].height == map[h][w].height) // coming from right
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }            
        }
    }
}

void cMapLoader::Load_Edge_B_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["edge"])
            {
                if (map[h][w + 1].color == colors["edge"] && // right
                    map[h][w - 1].color == colors["edge"] && // left
                    map[h][w].height == map[h + 1][w].height) // height of top
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h][w - 1].color == colors["edge"] &&
                    map[h][w + 1].color != colors["edge"] &&
                    map[h + 1][w].color != colors["edge"] &&
                    map[h - 1][w].color != colors["edge"] &&
                    map[h + 1][w].height == map[h][w].height) // coming from left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h][w + 1].color == colors["edge"] &&
                    map[h][w - 1].color != colors["edge"] &&
                    map[h + 1][w].color != colors["edge"] &&
                    map[h - 1][w].color != colors["edge"] &&
                    map[h + 1][w].height == map[h][w].height) // coming from right
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Edge_L_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["edge"])
            {
                if (map[h + 1][w].color == colors["edge"] && // up
                    map[h - 1][w].color == colors["edge"] && // down
                    map[h][w].height == map[h][w + 1].height) // height of right
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h + 1][w].color == colors["edge"] &&
                    map[h - 1][w].color != colors["edge"] &&
                    map[h][w + 1].color != colors["edge"] &&
                    map[h][w - 1].color != colors["edge"] &&
                    map[h][w + 1].height == map[h][w].height) // coming from top
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h - 1][w].color == colors["edge"] &&
                    map[h + 1][w].color != colors["edge"] &&
                    map[h][w + 1].color != colors["edge"] &&
                    map[h][w - 1].color != colors["edge"] &&
                    map[h][w + 1].height == map[h][w].height) // coming from bottom
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Edge_R_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["edge"])
            {
                if (map[h + 1][w].color == colors["edge"] && // up
                    map[h - 1][w].color == colors["edge"] && // down
                    map[h][w].height == map[h][w - 1].height) // height of left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h + 1][w].color == colors["edge"] &&
                    map[h - 1][w].color != colors["edge"] &&
                    map[h][w + 1].color != colors["edge"] &&
                    map[h][w - 1].color != colors["edge"] &&
                    map[h][w - 1].height == map[h][w].height) // coming from top
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
                else if (map[h - 1][w].color == colors["edge"] &&
                    map[h + 1][w].color != colors["edge"] &&
                    map[h][w + 1].color != colors["edge"] &&
                    map[h][w - 1].color != colors["edge"] &&
                    map[h][w - 1].height == map[h][w].height) // coming from bottom
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Edge_BR_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["edge"])
            {
                if (map[h + 1][w].color == colors["edge"] && // top
                    map[h - 1][w].color != colors["edge"] && // bottom
                    map[h][w + 1].color != colors["edge"] && // right
                    map[h][w - 1].color == colors["edge"] && // left
                    map[h + 1][w - 1].height == map[h][w].height) // top left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Edge_BL_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["edge"])
            {
                if (map[h + 1][w].color == colors["edge"] && // top
                    map[h - 1][w].color != colors["edge"] && // bottom
                    map[h][w + 1].color == colors["edge"] && // right
                    map[h][w - 1].color != colors["edge"] && // left
                    map[h + 1][w + 1].height == map[h][w].height) // top right
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Edge_BRC_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["edge"])
            {
                if (map[h + 1][w].color == colors["edge"] && // top
                    map[h - 1][w].height == map[h][w].height && // bottom
                    map[h][w + 1].height == map[h][w].height && // right
                    map[h][w - 1].color == colors["edge"] && // left
                    map[h + 1][w - 1].height != map[h][w].height) // top left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Edge_BLC_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["edge"])
            {
                if (map[h + 1][w].color == colors["edge"] && // top
                    map[h - 1][w].height == map[h][w].height && // bottom
                    map[h][w + 1].color == colors["edge"] && // right
                    map[h][w - 1].height == map[h][w].height && // left
                    map[h + 1][w + 1].height != map[h][w].height) // top right
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Edge_TR_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["edge"])
            {
                if (map[h + 1][w].height != map[h][w].height && // top
                    map[h - 1][w].color == colors["edge"] && // bottom
                    map[h][w + 1].height != map[h][w].height && // right
                    map[h][w - 1].color == colors["edge"] && // left
                    map[h - 1][w - 1].height == map[h][w].height) // bottom left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Edge_TL_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["edge"])
            {
                if (map[h + 1][w].height != map[h][w].height && // top
                    map[h - 1][w].color == colors["edge"] && // bottom
                    map[h][w + 1].color == colors["edge"] && // right
                    map[h][w - 1].height != map[h][w].height && // left
                    map[h - 1][w + 1].height == map[h][w].height) // bottom right
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Edge_TRC_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["edge"])
            {
                if (map[h + 1][w].height == map[h][w].height && // top
                    map[h - 1][w].color == colors["edge"] && // bottom
                    map[h][w + 1].height == map[h][w].height && // right
                    map[h][w - 1].color == colors["edge"] && // left
                    map[h - 1][w - 1].height != map[h][w].height) // bottom left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Edge_TLC_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["edge"])
            {
                if (map[h + 1][w].height == map[h][w].height && // top
                    map[h - 1][w].color == colors["edge"] && // bottom
                    map[h][w + 1].color == colors["edge"] && // right
                    map[h][w - 1].height == map[h][w].height && // left
                    map[h - 1][w + 1].height != map[h][w].height) // bottom right
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_M_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color == colors["path"] &&
                    map[h - 1][w].color == colors["path"] &&
                    map[h][w + 1].color == colors["path"] &&
                    map[h][w - 1].color == colors["path"] &&
                    map[h + 1][w + 1].color == colors["path"] && // top right
                    map[h + 1][w - 1].color == colors["path"] &&
                    map[h - 1][w + 1].color == colors["path"] &&
                    map[h - 1][w - 1].color == colors["path"])
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_T_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color != colors["path"] && // up
                    map[h - 1][w].color == colors["path"] && // down
                    map[h][w + 1].color == colors["path"] && // right
                    map[h][w - 1].color == colors["path"])   // left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_B_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color == colors["path"] && // up
                    map[h - 1][w].color != colors["path"] && // down
                    map[h][w + 1].color == colors["path"] && // right
                    map[h][w - 1].color == colors["path"])   // left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_L_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color == colors["path"] && // up
                    map[h - 1][w].color == colors["path"] && // down
                    map[h][w + 1].color == colors["path"] && // right
                    map[h][w - 1].color != colors["path"])   // left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_R_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color == colors["path"] && // up
                    map[h - 1][w].color == colors["path"] && // down
                    map[h][w + 1].color != colors["path"] && // right
                    map[h][w - 1].color == colors["path"])   // left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_BR_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color == colors["path"] && // up
                    map[h - 1][w].color != colors["path"] && // down
                    map[h][w + 1].color != colors["path"] && // right
                    map[h][w - 1].color == colors["path"])   // left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_BL_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color == colors["path"] && // up
                    map[h - 1][w].color != colors["path"] && // down
                    map[h][w + 1].color == colors["path"] && // right
                    map[h][w - 1].color != colors["path"])   // left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_BRC_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color == colors["path"] &&
                    map[h - 1][w].color == colors["path"] &&
                    map[h][w + 1].color == colors["path"] &&
                    map[h][w - 1].color == colors["path"] &&
                    map[h + 1][w + 1].color == colors["path"] && // top right
                    map[h + 1][w - 1].color != colors["path"] &&
                    map[h - 1][w + 1].color == colors["path"] &&
                    map[h - 1][w - 1].color == colors["path"])
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_BLC_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color == colors["path"] &&
                    map[h - 1][w].color == colors["path"] &&
                    map[h][w + 1].color == colors["path"] &&
                    map[h][w - 1].color == colors["path"] &&
                    map[h + 1][w + 1].color != colors["path"] && // top right
                    map[h + 1][w - 1].color == colors["path"] &&
                    map[h - 1][w + 1].color == colors["path"] &&
                    map[h - 1][w - 1].color == colors["path"])
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_TR_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color != colors["path"] && // up
                    map[h - 1][w].color == colors["path"] && // down
                    map[h][w + 1].color != colors["path"] && // right
                    map[h][w - 1].color == colors["path"])   // left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_TL_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color != colors["path"] && // up
                    map[h - 1][w].color == colors["path"] && // down
                    map[h][w + 1].color == colors["path"] && // right
                    map[h][w - 1].color != colors["path"])   // left
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_TRC_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color == colors["path"] &&
                    map[h - 1][w].color == colors["path"] &&
                    map[h][w + 1].color == colors["path"] &&
                    map[h][w - 1].color == colors["path"] &&
                    map[h + 1][w + 1].color == colors["path"] && // top right
                    map[h + 1][w - 1].color == colors["path"] &&
                    map[h - 1][w + 1].color == colors["path"] &&
                    map[h - 1][w - 1].color != colors["path"])
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Path_TLC_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["path"])
            {
                if (map[h + 1][w].color == colors["path"] &&
                    map[h - 1][w].color == colors["path"] &&
                    map[h][w + 1].color == colors["path"] &&
                    map[h][w - 1].color == colors["path"] &&
                    map[h + 1][w + 1].color == colors["path"] && // top right
                    map[h + 1][w - 1].color == colors["path"] &&
                    map[h - 1][w + 1].color != colors["path"] &&
                    map[h - 1][w - 1].color == colors["path"])
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Wall_B_Offsets(std::vector<glm::vec4>& offsets,
                                     std::vector<glm::vec4>& edges_B_Offsets,
                                     std::vector<glm::vec4>& edges_TLC_Offsets,
                                     std::vector<glm::vec4>& edges_TRC_Offsets)
{
    for (int i = 0; i < edges_B_Offsets.size(); i++)
    {
        glm::vec4 newOffset;
        newOffset.x = edges_B_Offsets[i].x;
        newOffset.z = edges_B_Offsets[i].z;
        newOffset.w = 1.f;

        int topPosHeight = round(-edges_B_Offsets[i].z);
        int topPosWidth = round(edges_B_Offsets[i].x);

        for (int botHeight = round(map[topPosHeight - 1][topPosWidth].height); botHeight < round(map[topPosHeight][topPosWidth].height); botHeight++)
        {
            newOffset.y = botHeight;
            offsets.push_back(newOffset);
        }
    }

    for (int i = 0; i < edges_TLC_Offsets.size(); i++)
    {
        glm::vec4 newOffset;
        newOffset.x = edges_TLC_Offsets[i].x;
        newOffset.z = edges_TLC_Offsets[i].z;
        newOffset.w = 1.f;

        int topPosHeight = round(-edges_TLC_Offsets[i].z);
        int topPosWidth = round(edges_TLC_Offsets[i].x);

        for (int botHeight = round(map[topPosHeight - 1][topPosWidth + 1].height); botHeight < round(map[topPosHeight][topPosWidth].height); botHeight++)
        {
            newOffset.y = botHeight;
            offsets.push_back(newOffset);
        }
    }

    for (int i = 0; i < edges_TRC_Offsets.size(); i++)
    {
        glm::vec4 newOffset;
        newOffset.x = edges_TRC_Offsets[i].x;
        newOffset.z = edges_TRC_Offsets[i].z;
        newOffset.w = 1.f;

        int topPosHeight = round(-edges_TRC_Offsets[i].z);
        int topPosWidth = round(edges_TRC_Offsets[i].x);

        for (int botHeight = round(map[topPosHeight - 1][topPosWidth - 1].height); botHeight < round(map[topPosHeight][topPosWidth].height); botHeight++)
        {
            newOffset.y = botHeight;
            offsets.push_back(newOffset);
        }
    }
}

void cMapLoader::Load_Wall_R_Offsets(std::vector<glm::vec4>& offsets, std::vector<glm::vec4>& edges_R_Offsets, std::vector<glm::vec4>& edges_TLC_Offsets)
{
    for (int i = 0; i < edges_R_Offsets.size(); i++)
    {
        glm::vec4 newOffset;
        newOffset.x = edges_R_Offsets[i].x;
        newOffset.z = edges_R_Offsets[i].z;
        newOffset.w = 1.f;

        int topPosHeight = round(-edges_R_Offsets[i].z);
        int topPosWidth = round(edges_R_Offsets[i].x);

        for (int botHeight = round(map[topPosHeight][topPosWidth + 1].height); botHeight < round(map[topPosHeight][topPosWidth].height); botHeight++)
        {
            newOffset.y = botHeight;
            offsets.push_back(newOffset);
        }
    }

    for (int i = 0; i < edges_TLC_Offsets.size(); i++)
    {
        glm::vec4 newOffset;
        newOffset.x = edges_TLC_Offsets[i].x;
        newOffset.z = edges_TLC_Offsets[i].z;
        newOffset.w = 1.f;

        int topPosHeight = round(-edges_TLC_Offsets[i].z);
        int topPosWidth = round(edges_TLC_Offsets[i].x);

        for (int botHeight = round(map[topPosHeight - 1][topPosWidth + 1].height); botHeight < round(map[topPosHeight][topPosWidth].height); botHeight++)
        {
            newOffset.y = botHeight;
            offsets.push_back(newOffset);
        }
    }
}

void cMapLoader::Load_Wall_L_Offsets(std::vector<glm::vec4>& offsets, std::vector<glm::vec4>& edges_L_Offsets, std::vector<glm::vec4>& edges_TRC_Offsets)
{
    for (int i = 0; i < edges_L_Offsets.size(); i++)
    {
        glm::vec4 newOffset;
        newOffset.x = edges_L_Offsets[i].x;
        newOffset.z = edges_L_Offsets[i].z;
        newOffset.w = 1.f;

        int topPosHeight = round(-edges_L_Offsets[i].z);
        int topPosWidth = round(edges_L_Offsets[i].x);

        for (int botHeight = round(map[topPosHeight][topPosWidth - 1].height); botHeight < round(map[topPosHeight][topPosWidth].height); botHeight++)
        {
            newOffset.y = botHeight;
            offsets.push_back(newOffset);
        }
    }

    for (int i = 0; i < edges_TRC_Offsets.size(); i++)
    {
        glm::vec4 newOffset;
        newOffset.x = edges_TRC_Offsets[i].x;
        newOffset.z = edges_TRC_Offsets[i].z;
        newOffset.w = 1.f;

        int topPosHeight = round(-edges_TRC_Offsets[i].z);
        int topPosWidth = round(edges_TRC_Offsets[i].x);

        for (int botHeight = round(map[topPosHeight - 1][topPosWidth - 1].height); botHeight < round(map[topPosHeight][topPosWidth].height); botHeight++)
        {
            newOffset.y = botHeight;
            offsets.push_back(newOffset);
        }
    }
}

void cMapLoader::Load_Wall_BL_Offsets(std::vector<glm::vec4>& offsets, std::vector<glm::vec4>& edges_BL_Offsets)
{
    for (int i = 0; i < edges_BL_Offsets.size(); i++)
    {
        glm::vec4 newOffset;
        newOffset.x = edges_BL_Offsets[i].x;
        newOffset.z = edges_BL_Offsets[i].z;
        newOffset.w = 1.f;

        int topPosHeight = round(-edges_BL_Offsets[i].z);
        int topPosWidth = round(edges_BL_Offsets[i].x);

        for (int botHeight = round(map[topPosHeight][topPosWidth - 1].height); botHeight < round(map[topPosHeight][topPosWidth].height); botHeight++)
        {
            newOffset.y = botHeight;
            offsets.push_back(newOffset);
        }
    }
}

void cMapLoader::Load_Wall_BR_Offsets(std::vector<glm::vec4>& offsets, std::vector<glm::vec4>& edges_BR_Offsets)
{
    for (int i = 0; i < edges_BR_Offsets.size(); i++)
    {
        glm::vec4 newOffset;
        newOffset.x = edges_BR_Offsets[i].x;
        newOffset.z = edges_BR_Offsets[i].z;
        newOffset.w = 1.f;

        int topPosHeight = round(-edges_BR_Offsets[i].z);
        int topPosWidth = round(edges_BR_Offsets[i].x);

        for (int botHeight = round(map[topPosHeight][topPosWidth + 1].height); botHeight < round(map[topPosHeight][topPosWidth].height); botHeight++)
        {
            newOffset.y = botHeight;
            offsets.push_back(newOffset);
        }
    }
}

void cMapLoader::Load_Stair_R_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["stair"])
            {
                if ((map[h][w - 1].height < map[h][w].height && map[h][w - 1].walkable)||
                    map[h][w - 1].color == colors["stair"] && map[h][w - 1].height < map[h][w].height)
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height - 1;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Stair_L_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["stair"])
            {
                if ((map[h][w + 1].height < map[h][w].height && map[h][w + 1].walkable) ||
                    (map[h][w + 1].color == colors["stair"] && map[h][w + 1].height < map[h][w].height))
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height - 1;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Stair_U_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["stair"])
            {
                if ((map[h - 1][w].height < map[h][w].height && map[h - 1][w].walkable) ||
                    map[h - 1][w].color == colors["stair"] && map[h - 1][w].height < map[h][w].height)
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height - 1;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Stair_D_Offsets(std::vector<glm::vec4>& offsets)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["stair"])
            {
                if ((map[h + 1][w].height < map[h][w].height && map[h + 1][w].walkable) ||
                    map[h + 1][w].color == colors["stair"] && map[h + 1][w].height < map[h][w].height)
                {
                    glm::vec4 newOffset;
                    newOffset.x = w * 1.f;
                    newOffset.y = map[h][w].height - 1;
                    newOffset.z = h * -1.f;
                    newOffset.w = 1.f;

                    offsets.push_back(newOffset);
                }
            }
        }
    }
}

void cMapLoader::Load_Weed_Offsets(std::vector<glm::vec4>& offsets, std::vector<glm::vec4>& grassOffsets, float percent)
{
    for (unsigned int i = 0; i < grassOffsets.size(); i++)
    {
        // generate random float (0 - 1)
        float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        // if rand < percent
        if (random < percent)
        {
            // place weed
            glm::vec4 newOffset;
            newOffset.x = grassOffsets[i].x;
            newOffset.y = grassOffsets[i].y + 0.01;
            newOffset.z = grassOffsets[i].z;
            newOffset.w = 1.f;

            offsets.push_back(newOffset);
        }
    }
}

void cMapLoader::Load_Structure_Offsets(std::vector<glm::vec4>& offsets, std::string tileName)
{
    if (colors.find(tileName) == colors.end())
        return;

    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors[tileName])
            {
                glm::vec4 newOffset;
                newOffset.x = w * 1.f * 14.28f;
                newOffset.y = map[h][w].height * 14.28f; // maybe change this
                newOffset.z = h * -1.f * 14.28f;
                newOffset.w = 1.f;

                offsets.push_back(newOffset);
            }
        }
    }
}

void cMapLoader::PlacePlayer(glm::vec3& playerPosition)
{
    for (int h = 0; h < decorationHeight; h++)
    {
        for (int w = 0; w < decorationWidth; w++)
        {
            if (map[h][w].color == colors["player"])
            {
                playerPosition.x = w * 1.f;
                playerPosition.y = map[h][w].height;
                playerPosition.z = h * -1.f;
                return;
            }
        }
    }
}
