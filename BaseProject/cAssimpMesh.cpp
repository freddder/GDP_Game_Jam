#include "cAssimpMesh.h"
#include "globalThings.h"

#include <sstream>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

cAssimpMesh::cAssimpMesh()
{
    position = glm::vec3(0);
    orientation = glm::vec3(0);
    scale = glm::vec3(1);
    isInstanced = false;
}

cAssimpMesh::~cAssimpMesh()
{
    //delete allMeshesInfo;
}

void cAssimpMesh::LoadModel(std::string fileName)
{
    const aiScene* scene = m_AssimpImporter.ReadFile("assets/models/" + fileName,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_PopulateArmatureData |
        aiProcess_FixInfacingNormals |
        aiProcess_LimitBoneWeights);

    if (scene == nullptr)
    {
        printf("MeshManager::LoadMeshWithAssimp: ERROR: Failed to load file %s\n", fileName.c_str());
        return;
    }

    if (scene->HasMeshes())
    {
        //allMeshesInfo = new sModelDrawInfo[scene->mNumMeshes];
        numMeshes = scene->mNumMeshes;

        for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
        {
            aiMesh* currMesh = scene->mMeshes[meshIndex];

            sModelDrawInfo modelInfo;
            modelInfo.pVertices = new sVertex_XYZW_RGBA_N_UV_T_B[currMesh->mNumVertices];
            modelInfo.numberOfVertices = currMesh->mNumVertices;

            for (unsigned int vertexIndex = 0; vertexIndex < currMesh->mNumVertices; vertexIndex++)
            {
                sVertex_XYZW_RGBA_N_UV_T_B vertexInfo;

                vertexInfo.x = currMesh->mVertices[vertexIndex].x;
                vertexInfo.y = currMesh->mVertices[vertexIndex].y;
                vertexInfo.z = currMesh->mVertices[vertexIndex].z;
                vertexInfo.w = 1;

                vertexInfo.nx = currMesh->mNormals[vertexIndex].x;
                vertexInfo.ny = currMesh->mNormals[vertexIndex].y;
                vertexInfo.nz = currMesh->mNormals[vertexIndex].z;
                vertexInfo.nw = 1;

                if (currMesh->mTextureCoords[0])
                {
                    vertexInfo.u0 = currMesh->mTextureCoords[0][vertexIndex].x;
                    vertexInfo.v0 = currMesh->mTextureCoords[0][vertexIndex].y;
                    vertexInfo.u1 = 0;
                    vertexInfo.v1 = 0;
                }

                vertexInfo.r = 1.f;
                vertexInfo.g = 1.f;
                vertexInfo.b = 1.f;
                vertexInfo.a = 1.f;

                modelInfo.pVertices[vertexIndex] = vertexInfo;
            }

            modelInfo.numberOfIndices = currMesh->mNumFaces * 3;
            modelInfo.numberOfTriangles = currMesh->mNumFaces;

            modelInfo.pIndices = new unsigned int[modelInfo.numberOfIndices];

            unsigned int indiceIndex = 0;
            for (unsigned int i = 0; i < currMesh->mNumFaces; i++)
            {
                aiFace currFace = currMesh->mFaces[i];
                for (unsigned int j = 0; j < currFace.mNumIndices; j++)
                {
                    modelInfo.pIndices[indiceIndex + j] = currFace.mIndices[j];
                }
                indiceIndex += currFace.mNumIndices;
            }

            if (currMesh->mMaterialIndex >= 0)
            {
                aiMaterial* material = scene->mMaterials[currMesh->mMaterialIndex];

                aiString path;
                material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

                std::string pathS = path.C_Str();
                pathS = pathS.substr(0, pathS.length() - 4);

                texturesUsed.push_back(pathS);
            }

            allMeshesInfo.push_back(modelInfo);
        }
    }
}

void cAssimpMesh::LoadModelIntoVAO(GLuint shaderProgramID, std::vector<glm::vec4>& offsets)
{
    for (unsigned int i = 0; i < allMeshesInfo.size(); i++)
    {
        sModelDrawInfo& drawInfo = allMeshesInfo[i];

        // Create a VAO (Vertex Array Object), which will 
        //	keep track of all the 'state' needed to draw 
        //	from this buffer...

        // Ask OpenGL for a new buffer ID...
        glGenVertexArrays(1, &(drawInfo.VAO_ID));
        // "Bind" this buffer:
        // - aka "make this the 'current' VAO buffer
        glBindVertexArray(drawInfo.VAO_ID);

        // Now ANY state that is related to vertex or index buffer
        //	and vertex attribute layout, is stored in the 'state' 
        //	of the VAO... 


        // NOTE: OpenGL error checks have been omitted for brevity
    //	glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &(drawInfo.VertexBufferID));

        //	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
        // sVert vertices[3]
        glBufferData(GL_ARRAY_BUFFER,
            sizeof(sVertex_XYZW_RGBA_N_UV_T_B) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
            (GLvoid*)drawInfo.pVertices,							// pVertices,			//vertices, 
            GL_STATIC_DRAW);
        //glBufferData( GL_ARRAY_BUFFER, 
        //			  sizeof(sVertex_XYZW_RGBA) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
        //			  (GLvoid*) drawInfo.pVertices,							// pVertices,			//vertices, 
        //			  GL_STATIC_DRAW );
        //glBufferData( GL_ARRAY_BUFFER, 
        //			  sizeof(sVertex_XYZ_RGB) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
        //			  (GLvoid*) drawInfo.pVertices,							// pVertices,			//vertices, 
        //			  GL_STATIC_DRAW );


        // Copy the index buffer into the video card, too
        // Create an index buffer.
        glGenBuffers(1, &(drawInfo.IndexBufferID));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
            sizeof(unsigned int) * drawInfo.numberOfIndices,
            (GLvoid*)drawInfo.pIndices,
            GL_STATIC_DRAW);

        // ****************************************************************
        // Set the vertex attributes.


        //struct sVertex_XYZW_RGBA
        //{
        //    float x, y, z, w;   // Same as vec4 vPosition
        //    float r, g, b, a;   // Same as vec4 vColour
        //};

        // Set the vertex attributes for this shader
        GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPosition");	    // program
        glEnableVertexAttribArray(vpos_location);	    // vPosition
        glVertexAttribPointer(vpos_location, 4,		// vPosition
            GL_FLOAT, GL_FALSE,
            sizeof(sVertex_XYZW_RGBA_N_UV_T_B),     // Stride // sizeof(float) * 6,      
            (void*)offsetof(sVertex_XYZW_RGBA_N_UV_T_B, x));

        GLint vcol_location = glGetAttribLocation(shaderProgramID, "vColour");	// program;
        glEnableVertexAttribArray(vcol_location);	    // vColour
        glVertexAttribPointer(vcol_location, 4,		// vColour
            GL_FLOAT, GL_FALSE,
            sizeof(sVertex_XYZW_RGBA_N_UV_T_B),     // Stride // sizeof(float) * 6,   
            (void*)offsetof(sVertex_XYZW_RGBA_N_UV_T_B, r));

        // And all these, too:
        //in vec4 vNormal;				// Vertex normal X,Y,Z (W ignored)
        GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;
        glEnableVertexAttribArray(vNormal_location);	    // vNormal
        glVertexAttribPointer(vNormal_location, 4,		    // vNormal
            GL_FLOAT, GL_FALSE,
            sizeof(sVertex_XYZW_RGBA_N_UV_T_B),
            (void*)offsetof(sVertex_XYZW_RGBA_N_UV_T_B, nx));


        //in vec4 vUVx2;					// 2 x Texture coords (vec4) UV0, UV1
        GLint vUVx2_location = glGetAttribLocation(shaderProgramID, "vUVx2");	// program;
        glEnableVertexAttribArray(vUVx2_location);	        // vUVx2
        glVertexAttribPointer(vUVx2_location, 4,		    // vUVx2
            GL_FLOAT, GL_FALSE,
            sizeof(sVertex_XYZW_RGBA_N_UV_T_B),
            (void*)offsetof(sVertex_XYZW_RGBA_N_UV_T_B, u0));


        //in vec4 vTangent;				// For bump mapping X,Y,Z (W ignored)
        GLint vTangent_location = glGetAttribLocation(shaderProgramID, "vTangent");	// program;
        glEnableVertexAttribArray(vTangent_location);	    // vTangent
        glVertexAttribPointer(vTangent_location, 4,		    // vTangent
            GL_FLOAT, GL_FALSE,
            sizeof(sVertex_XYZW_RGBA_N_UV_T_B),
            (void*)offsetof(sVertex_XYZW_RGBA_N_UV_T_B, tx));


        //in vec4 vBiNormal;				// For bump mapping X,Y,Z (W ignored)
        GLint vBiNormal_location = glGetAttribLocation(shaderProgramID, "vBiNormal");	// program;
        glEnableVertexAttribArray(vBiNormal_location);	        // vBiNormal
        glVertexAttribPointer(vBiNormal_location, 4,		    // vBiNormal
            GL_FLOAT, GL_FALSE,
            sizeof(sVertex_XYZW_RGBA_N_UV_T_B),
            (void*)offsetof(sVertex_XYZW_RGBA_N_UV_T_B, bx));


        // ****************************************************************
        //glm::vec3 origin = glm::vec3(0.f);

        glGenBuffers(1, &(drawInfo.instanceBufferId));

        glBindBuffer(GL_ARRAY_BUFFER, drawInfo.instanceBufferId);

        glBufferData(GL_ARRAY_BUFFER,
            sizeof(glm::vec4)* offsets.size(),	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
            (GLvoid*)&offsets[0],							// pVertices,			//vertices, 
            GL_STATIC_DRAW);

        GLint offset_location = glGetAttribLocation(shaderProgramID, "oOffset");	    // program
        glEnableVertexAttribArray(offset_location);	    // vPosition
        glVertexAttribPointer(offset_location, 4,		// vPosition
            GL_FLOAT, GL_FALSE,
            sizeof(glm::vec4),     // Stride // sizeof(float) * 6,      
            (void*)0);
        glVertexAttribDivisor(offset_location, 1);

        // Now that all the parts are set up, set the VAO to zero
        glBindVertexArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(vpos_location);
        glDisableVertexAttribArray(vcol_location);
        // And the newer ones:
        glDisableVertexAttribArray(vNormal_location);	    // vNormal
        glDisableVertexAttribArray(vUVx2_location);	        // vUVx2
        glDisableVertexAttribArray(vTangent_location);	    // vTangent
        glDisableVertexAttribArray(vBiNormal_location);	        // vBiNormal
    }
}

void cAssimpMesh::DrawObject(GLuint shaderProgramID)
{
    for (unsigned int i = 0; i < allMeshesInfo.size(); i++)
    {
        glUseProgram(shaderProgramID);

        GLuint TextureNumber = ::g_pTextureManager->getTextureIDFromName(texturesUsed[i] + ".bmp");

        GLuint textureUnit = 0;			// Texture unit go from 0 to 79
        glActiveTexture(textureUnit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984
        glBindTexture(GL_TEXTURE_2D, TextureNumber);

        GLint texture_00_LocID = glGetUniformLocation(shaderProgramID, "texture_00");
        glUniform1i(texture_00_LocID, textureUnit);

        // Alpha transparency
        glEnable(GL_BLEND);
        // Basic "alpha transparency"
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        // Get the uniform (should be outside of the draw call)
        GLint wholeObjectAlphaTransparency_LocID = glGetUniformLocation(shaderProgramID, "wholeObjectAlphaTransparency");
        // Set this value here
        glUniform1f(wholeObjectAlphaTransparency_LocID, 1.f);


        // *****************************************************
        // Translate or "move" the object somewhere
        glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
            position);
        //matModel = matModel * matTranslate;
        // *****************************************************


        // *****************************************************
        // Rotation around the Z axis
        glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
            orientation.z,//(float)glfwGetTime(),
            glm::vec3(0.0f, 0.0f, 1.0f));
        //matModel = matModel * rotateZ;
        // *****************************************************

        // *****************************************************
        // Rotation around the Y axis
        glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
            orientation.y,
            glm::vec3(0.0f, 1.0f, 0.0f));
        //matModel = matModel * rotateY;
        // *****************************************************

        // *****************************************************
        // Rotation around the X axis
        glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
            orientation.x,
            glm::vec3(1.0f, 0.0f, 0.0f));
        //matModel = matModel * rotateX;
        // *****************************************************


        // *****************************************************
        // Scale the model
        glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);
        //matModel = matModel * matScale;
        // *****************************************************

        // *****************************************************
        glm::mat4 matModel(1);
        matModel = matModel * matTranslate;
        matModel = matModel * rotateZ;
        matModel = matModel * rotateY;
        matModel = matModel * rotateX;
        matModel = matModel * matScale;     // <-- mathematically, this is 1st

        // Now the matModel ("Model" or "World") matrix
        //  represents ALL the transformations we want, in ONE matrix.

        // Moved view and projection ("v" and "p") to outside draw scene loop.
        // (because they are the same for all objects)

        // Set all the texture ratios in the shader
        GLint textureRatios0to3_LocID = glGetUniformLocation(shaderProgramID, "texture2D_Ratios0to3");
        glUniform4f(textureRatios0to3_LocID,
            1.f,
            0.f,
            0.f,
            0.f);

        GLint matModel_Location = glGetUniformLocation(shaderProgramID, "matModel");
        glUniformMatrix4fv(matModel_Location, 1, GL_FALSE, glm::value_ptr(matModel));

        // Inverse transpose of the model matrix
        // (Used to calculate the normal location in vertex space, using only rotation)
        glm::mat4 matInvTransposeModel = glm::inverse(glm::transpose(matModel));
        GLint matModelInverseTranspose_Location = glGetUniformLocation(shaderProgramID, "matModelInverseTranspose");
        glUniformMatrix4fv(matModelInverseTranspose_Location, 1, GL_FALSE, glm::value_ptr(matInvTransposeModel));


        // Copy the whole object colour information to the sahder               

        // This is used for wireframe or whole object colour. 
        // If bUseDebugColour is TRUE, then the fragment colour is "objectDebugColour".
        GLint bUseDebugColour_Location = glGetUniformLocation(shaderProgramID, "bUseDebugColour");
        GLint objectDebugColour_Location = glGetUniformLocation(shaderProgramID, "objectDebugColour");

        // If true, then the lighting contribution is NOT used. 
        // This is useful for wireframe object
        GLint bDontLightObject_Location = glGetUniformLocation(shaderProgramID, "bDontLightObject");

        // The "whole object" colour (diffuse and specular)
        GLint wholeObjectDiffuseColour_Location = glGetUniformLocation(shaderProgramID, "wholeObjectDiffuseColour");
        GLint bUseWholeObjectDiffuseColour_Location = glGetUniformLocation(shaderProgramID, "bUseWholeObjectDiffuseColour");
        GLint wholeObjectSpecularColour_Location = glGetUniformLocation(shaderProgramID, "wholeObjectSpecularColour");

        //if (pCurrentMesh->bUseWholeObjectDiffuseColour)
        //{
        //    glUniform1f(bUseWholeObjectDiffuseColour_Location, (float)GL_TRUE);
        //    glUniform4f(wholeObjectDiffuseColour_Location,
        //        1.f,
        //        1.f,
        //        1.f,
        //        1.f);
        //}
        //else
        //{
        glUniform1f(bUseWholeObjectDiffuseColour_Location, (float)GL_FALSE);
        //}

        glUniform4f(wholeObjectSpecularColour_Location,
            1.f,
            1.f,
            1.f,
            1.f);


        // See if mesh is wanting the vertex colour override (HACK) to be used?
        //if (pCurrentMesh->bUseObjectDebugColour)
        //{
        //    // Override the colour...
        //    glUniform1f(bUseDebugColour_Location, (float)GL_TRUE);
        //    glUniform4f(objectDebugColour_Location,
        //        pCurrentMesh->objectDebugColourRGBA.r,
        //        pCurrentMesh->objectDebugColourRGBA.g,
        //        pCurrentMesh->objectDebugColourRGBA.b,
        //        pCurrentMesh->objectDebugColourRGBA.a);
        //}
        //else
        {
            // DON'T override the colour
            glUniform1f(bUseDebugColour_Location, (float)GL_FALSE);
        }


        // See if mesh is wanting the vertex colour override (HACK) to be used?
        //if (pCurrentMesh->bDontLight)
        {
            // Override the colour...
            glUniform1f(bDontLightObject_Location, (float)GL_FALSE);
        }
        //else
        //{
        //    // DON'T override the colour
        //    glUniform1f(bDontLightObject_Location, (float)GL_FALSE);
        //}


        // Wireframe
        //if (pCurrentMesh->bIsWireframe)                // GL_POINT, GL_LINE, and GL_FILL)
        //{
        //    // Draw everything with only lines
        //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //}
        //else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Rigged
        //GLint IsRigged_Location = glGetUniformLocation(shaderProgramID, "isRigged");
        //if (pCurrentMesh->isRigged)
        //{
        //    glUniform1f(IsRigged_Location, (float)GL_TRUE);

        //    std::vector<GLuint> mBoneMatrixULs;
        //    mBoneMatrixULs.resize(4);
        //    mBoneMatrixULs[0] = glGetUniformLocation(program, "boneMatrices[0]");
        //    mBoneMatrixULs[1] = glGetUniformLocation(program, "boneMatrices[1]");
        //    mBoneMatrixULs[2] = glGetUniformLocation(program, "boneMatrices[2]");
        //    mBoneMatrixULs[3] = glGetUniformLocation(program, "boneMatrices[3]");

        //    for (int i = 0; i < 4; i++)
        //    {
        //        glUniformMatrix4fv(mBoneMatrixULs[i], 1, GL_FALSE, glm::value_ptr(pCurrentMesh->boneHierarchy->bones[i]->GetModelMatrix()));
        //    }
        //}
        //else
        //{
        //    glUniform1f(IsRigged_Location, (float)GL_FALSE);
        //}

        GLint discardTexture_Location = glGetUniformLocation(shaderProgramID, "bDiscardTexture");
        GLint discardBlack_Location = glGetUniformLocation(shaderProgramID, "bDiscardBlack");

        glUniform1f(discardTexture_Location, (float)GL_FALSE);
        glUniform1f(discardBlack_Location, (float)GL_FALSE);

        sModelDrawInfo modelInfo = allMeshesInfo[i];

        glBindVertexArray(modelInfo.VAO_ID);

        //glDrawElements(GL_TRIANGLES,
        //    modelInfo.numberOfIndices,
        //    GL_UNSIGNED_INT,
        //    (void*)0);

        if (isInstanced)
        {
            glDrawElementsInstanced(GL_TRIANGLES,
                modelInfo.numberOfIndices,
                GL_UNSIGNED_INT,
                (void*)0,
                instancedNumber);
        }
        else
        {
            glDrawElements(GL_TRIANGLES,
                modelInfo.numberOfIndices,
                GL_UNSIGNED_INT,
                (void*)0);
        }

        glBindVertexArray(0);

    }
}