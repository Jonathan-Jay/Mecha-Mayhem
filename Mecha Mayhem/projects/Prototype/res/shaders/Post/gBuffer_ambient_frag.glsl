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
	DirectionalLight light;
};

layout(binding = 0) uniform sampler2D s_albedoTex;
layout(binding = 4) uniform sampler2D s_lightAccumTex;

//uniform vec3 for clear colour?

out vec4 frag_color;

void main()
{
	//Albdeo
	vec4 textureColour = texture(s_albedoTex, inUV);
	//lights
	vec4 lightAccum = texture(s_lightAccumTex, inUV);

	//Ambient math
	vec3 ambient = light._lightAmbientPow * light._ambientCol.rgb;

	//result of all lighting
	vec3 result = (ambient + lightAccum.rgb) * textureColour.rgb;

	//Add clear colour?
	//result = result * skybox.rgb;

	//light accumulation
	frag_color = vec4(result, 1.0);
}