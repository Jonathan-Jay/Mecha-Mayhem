#version 420

layout(location = 0) in vec2 inUV;

struct DirectionalLight
{
	//Light direction (defaults to down, to the left, and a little forward)
	vec4 _lightDirection;

	//Generic Light controls
	vec4 _lightCol;

	//Ambience controls
	vec4 _ambientCol;
	float _ambientPow;
	
	//Power controls
	float _lightAmbientPow;
	float _lightSpecularPow;
	
	float _shadowBias;
};

layout (std140, binding = 0) uniform u_Lights
{
	DirectionalLight sun;
};


//layout(binding = 30) uniform sampler2D s_ShadowMap;

layout(binding = 0) uniform sampler2D s_albedoTex;
layout(binding = 1) uniform sampler2D s_normalsTex;
layout(binding = 2) uniform sampler2D s_specularTex;
layout(binding = 3) uniform sampler2D s_positionTex;

//uniform mat4 u_LightSpaceMatrix;
uniform vec3 u_CamPos[4];
uniform int camCount;

out vec4 frag_color;

// float ShadowCalculation(vec4 fragPosLightSpace, float bias)
// {
// 	//Perspective division
// 	vec3 projectionCoordinates = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
// 	//Transform into a [0,1] range
// 	projectionCoordinates = projectionCoordinates * 0.5 + 0.5;
	
// 	//Get the closest depth value from light's perspective (using our 0-1 range)
// 	float closestDepth = texture(s_ShadowMap, projectionCoordinates.xy).r;

// 	//Get the current depth according to the light
// 	float currentDepth = projectionCoordinates.z;

// 	//Check whether there's a shadow
// 	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

// 	//Return the value
// 	return shadow;
// }

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	//Albedo
	vec4 textureColor = texture(s_albedoTex, inUV);
	//Normals
	vec3 inNormal = (normalize(texture(s_normalsTex, inUV).rgb) * 2.0) - 1.0;
	//Specular
	vec2 texSpec = texture(s_specularTex, inUV).rg;
	//Positions
	vec3 fragPos = texture(s_positionTex, inUV).rgb;


	// Diffuse
	vec3 N = inNormal;
	vec3 lightDir = normalize(-sun._lightDirection.xyz);
	float dif = max(dot(N, lightDir), 0.0);
	vec3 diffuse = dif * sun._lightCol.xyz;// add diffuse intensity

	int camNum = 0;
	if (camCount > 2) {
		camNum = int(inUV.x > 0.5) + int(inUV.y < 0.5) * 2;
	}
	else if (camCount > 1) {
		camNum = int(inUV.x > 0.5);
	}

	// Specular
	vec3 viewDir  = normalize(u_CamPos[camNum] - fragPos);
	vec3 h        = normalize(lightDir + viewDir);


	// Get the specular power from the specular map
	float spec = pow(max(dot(N, h), 0.0), texSpec.y); // Shininess coefficient (can be a uniform)
	vec3 specular = sun._lightSpecularPow * texSpec.x * spec * sun._lightCol.xyz; // Can also use a specular color

	// Get the albedo from the diffuse / albedo map	

	//Lighting
	// vec4 inFragPosLightSpace = u_LightSpaceMatrix * vec4(fragPos, 1.0);


	// float shadow = ShadowCalculation(inFragPosLightSpace, sun._shadowBias);

	vec3 result = (
		(sun._ambientPow * sun._ambientCol.xyz) + // global ambient light
		//(1.0 - shadow) * //Shadow value
		(diffuse + specular) // light factors from our single light
		); // Object color, no

	if (textureColor.a < 0.31)
		result = vec3(1.0, 1.0, 1.0);

	frag_color = vec4(result, 1.0);
}
