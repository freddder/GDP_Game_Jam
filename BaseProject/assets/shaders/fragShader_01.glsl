// Fragment shader
#version 420

in vec4 fVertexColour;			// The vertex colour from the original model
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fUVx2;
in vec4 fVertPosLightSpace;

// Replaces gl_FragColor
out vec4 pixelColour;			// RGB Alpha   (0 to 1)
layout (location = 5) out vec4 pixelDepthOutput;

// The "whole object" colour (diffuse and specular)
uniform vec4 wholeObjectDiffuseColour;	// Whole object diffuse colour
uniform bool bUseWholeObjectDiffuseColour;	// If true, the whole object colour is used (instead of vertex colour)
uniform vec4 wholeObjectSpecularColour;	// Colour of the specular highlight (optional)

// Alpha transparency value
uniform float wholeObjectAlphaTransparency;

uniform bool bUseDebugColour;	
uniform vec4 objectDebugColour;		

uniform bool bDontLightObject;

uniform sampler2D depthMap;

// This is the camera eye location (update every frame)
uniform vec4 eyeLocation;

struct sLight
{
	vec4 position;			
	vec4 diffuse;	
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};


const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;


const int NUMBEROFLIGHTS = 20;
uniform sLight theLights[NUMBEROFLIGHTS];  	// 80 uniforms

uniform sampler2D texture_00;		// GL_TEXTURE_2D
uniform sampler2D texture_01;		// GL_TEXTURE_2D
uniform sampler2D texture_02;		// GL_TEXTURE_2D
uniform sampler2D texture_03;		// GL_TEXTURE_2D
uniform sampler2D texture_04;		// GL_TEXTURE_2D
uniform sampler2D texture_05;		// GL_TEXTURE_2D
uniform sampler2D texture_06;		// GL_TEXTURE_2D
uniform sampler2D texture_07;		// GL_TEXTURE_2D

uniform vec4 texture2D_Ratios0to3;		//  = vec4( 1.0f, 0.0f, 0.0f, 0.0f );
uniform vec4 texture2D_Ratios4to7;		//  = vec4( 1.0f, 0.0f, 0.0f, 0.0f );

// Cube maps for skybox, etc.
uniform samplerCube cubeMap_00;			// Tropical day time
uniform samplerCube cubeMap_01;			// Tropical night time
uniform samplerCube cubeMap_02;
uniform samplerCube cubeMap_03;
uniform vec4 cubeMap_Ratios0to3;		//  = vec4( 1.0f, 0.0f, 0.0f, 0.0f );

// if true, then we only sample from the cubeMaps (skyboxes)
uniform bool bIsSkyBox;

uniform bool bDiscardTexture;
uniform vec4 bDiscardColor;
uniform bool bDiscardBlack;

// Skybox or reflection or light probe
//uniform samplerCube skyBox;			// GL_TEXTURE_CUBE_MAP

float ShadowCalculation(vec4 fragPosLightSpace);

void main()
{
	pixelDepthOutput.x = gl_FragDepth;

	pixelColour.rgba = fVertexColour;	
	
	if ( bIsSkyBox )
	{
		if ( cubeMap_Ratios0to3.x > 0.0f )
		{
			pixelColour.rgb = texture( cubeMap_00, fNormal.xyz ).rgb * cubeMap_Ratios0to3.x;
		}
		if ( cubeMap_Ratios0to3.y > 0.0f )
		{
			pixelColour.rgb = texture( cubeMap_00, fNormal.xyz ).rgb * cubeMap_Ratios0to3.y;
		}

		return;	
	}//if ( bIsSkyBox )	

	// Use model vertex colours or not?
	if ( bUseWholeObjectDiffuseColour )
	{
		pixelColour = wholeObjectDiffuseColour;
	}
	else if (texture2D_Ratios0to3.x >= 0.1)
	{
		pixelColour = texture( texture_00, fUVx2.xy ) * texture2D_Ratios0to3.x;

		if(bDiscardTexture)
		{
			if((bDiscardColor.r - pixelColour.r < 0.001) && (bDiscardColor.g - pixelColour.g < 0.001) && (bDiscardColor.b - pixelColour.b < 0.001))
			{
				discard;
			}
		}

		if(bDiscardBlack)
		{
			// Take average of this RGB sample
			float discardSample = (pixelColour.r + pixelColour.g + pixelColour.b)/3.0f;
			if (discardSample < 0.1f )
			{	// "black enough"
				discard;
			}
		}
	}

	if(bDontLightObject)
	{
		return;
	}
	
	vec3 color = pixelColour.rgb;
    vec3 normal = normalize(fNormal.xyz);
    vec3 lightColor = theLights[0].diffuse.rgb;

    // ambient
    vec3 ambient = 0.3 * lightColor;

    // diffuse
    //vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 lightDir = normalize(-theLights[0].direction.xyz);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
	//vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	//float spec = 0.0;
	//vec3 halfwayDir = normalize(lightDir + viewDir);  
	//spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	//vec3 specular = spec * lightColor;    

    // calculate shadow
    float shadow = ShadowCalculation(fVertPosLightSpace);


    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse)) * color;

	//vec3 lightContrib = theLights[0].diffuse.rgb;
	//// Get the dot product of the light and normalize
	//float dotProduct = dot( -theLights[0].direction.xyz, normalize(normal) );	// -1 to 1
	//dotProduct = max( 0.0f, dotProduct );		// 0 to 1
	//lightContrib *= dotProduct;		
	////finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[index].diffuse.rgb * lightContrib); 
	//vec3 lighting = (pixelColour.rgb * lightContrib);
    
    pixelColour = vec4(lighting, 1.0);	
};

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
    float bias = 0.002;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
	    for(int y = -1; y <= 1; ++y)
	    {
	        float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
	        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
	    }    
	}
	shadow /= 9.0;

    return shadow;
}