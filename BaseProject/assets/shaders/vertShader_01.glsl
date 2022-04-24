// Vertex shader
#version 420

//uniform mat4 MVP;
uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModelInverseTranspose;	// For normal calculation
uniform mat4 matLightSpace;	// For normal calculation

in vec4 vColour;
in vec4 vPosition;
in vec4 vNormal;				// Vertex normal X,Y,Z (W ignored)
in vec4 vUVx2;					// 2 x Texture coords (vec4) UV0, UV1
in vec4 vTangent;				// For bump mapping X,Y,Z (W ignored)
in vec4 vBiNormal;				// For bump mapping X,Y,Z (W ignored)
in vec4 oOffset;

out vec4 fVertexColour;			// used to be "out vec3 color"
out vec4 fVertWorldLocation;
out vec4 fNormal;
out vec4 fUVx2;
out vec4 fVertPosLightSpace;


void main()
{
	// Order of these is important
	//mvp = p * v * matModel; from C++ code
	
	mat4 MVP = matProjection * matView * matModel;

//	float offsetX = (gl_InstanceID % 20) * 2;
//	float offsetZ = (gl_InstanceID / 50) * 2;
	
	vec4 vertPosition = vPosition;
	vertPosition.x += oOffset.x;
	vertPosition.y += oOffset.y;
	vertPosition.z += oOffset.z;

    gl_Position = MVP * vertPosition;
	
	// The location of the vertex in "world" space (not screen space)
	fVertWorldLocation = matModel * vertPosition;
    fVertexColour = vColour;		// Used to be vCol
	fNormal = matModelInverseTranspose * normalize(vNormal);
	fNormal = normalize(fNormal);
	
	fUVx2 = vUVx2;

	fVertPosLightSpace = matLightSpace * fVertWorldLocation;
};
