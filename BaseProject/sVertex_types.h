#pragma once

struct sVertex_XYZW_RGBA_N_UV_T_B
{
    float x, y, z, w;   // Same as vec4 vPosition
    float r, g, b, a;   // Same as vec4 vColour
    float nx, ny, nz, nw;   // in vec4 vNormal;	Vertex normal X,Y,Z (W ignored)
    float u0, v0, u1, v1;   //in vec4 vUVx2;					// 2 x Texture coords (vec4) UV0, UV1
    float tx, ty, tz, tw;   //in vec4 vTangent;				// For bump mapping X,Y,Z (W ignored)
    float bx, by, bz, bw;   //in vec4 vBiNormal;				// For bump mapping X,Y,Z (W ignored)
};

struct sModelDrawInfo
{
	sModelDrawInfo();

	std::string meshName;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	unsigned int instanceBufferId;

	sVertex_XYZW_RGBA_N_UV_T_B* pVertices;	//  = 0;

	unsigned int* pIndices;

	sVertex_XYZW_RGBA_N_UV_T_B maxValues;
	sVertex_XYZW_RGBA_N_UV_T_B minValues;

	sVertex_XYZW_RGBA_N_UV_T_B maxExtents;
	float maxExtent;

	void calculateExtents(void);
};