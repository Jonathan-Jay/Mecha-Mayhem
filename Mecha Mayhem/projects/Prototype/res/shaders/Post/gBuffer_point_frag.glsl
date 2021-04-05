#version 420

layout(location = 0) in vec2 inUV;

struct PointLight
{
	//VEC4s 
	vec4 _lightPos;
	vec4 _lightCol;

	//FLOATS
	float _lightLinearFalloff;
	float _lightQuadraticFalloff;
	float _lightSpecularPow;
};

layout (std140, binding = 0) uniform u_Lights
{
	PointLight light;
};

layout(binding = 0) uniform sampler2D s_albedoTex;
layout(binding = 1) uniform sampler2D s_normalsTex;
layout(binding = 2) uniform sampler2D s_specularTex;
layout(binding = 3) uniform sampler2D s_positionTex;
layout(binding = 4) uniform sampler2D s_emissiveTex;
layout(binding = 5) uniform sampler2D s_lightAccumTex;

uniform vec3 u_camPos[4];
uniform int camCount;

out vec4 frag_color;

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	//Normals
	vec3 inNormal = (normalize(texture(s_normalsTex, inUV).xyz) * 2.0) - 1.0;

	//Albedo
	vec4 textureColor = texture(s_albedoTex, inUV);

	//Specular
	vec3 texSpec = texture(s_specularTex, inUV).rgb;
	//Positions
	vec3 fragPos = texture(s_positionTex, inUV).rgb;

	vec3 lightDir = light._lightPos.xyz - fragPos;
	float lightDistSq = 1.0 / (
			light._lightLinearFalloff * length(lightDir) +
			light._lightQuadraticFalloff * dot(lightDir, lightDir)
		); 
	lightDir = normalize(lightDir);
 
	//diffuse
	float diffuse = max(dot(inNormal, lightDir), 0.0);

	int camNum = int(camCount > 1) * int(inUV.x > 0.5) + int(camCount > 2) * int(inUV.y < 0.5) * 2;

	// Specular
	float specular = pow(max(dot(inNormal, normalize(normalize(u_camPos[camNum] - fragPos) + lightDir)), 0.0), texSpec.y) * light._lightSpecularPow;

	vec3 result = texture(s_lightAccumTex, inUV).rgb + (
		//(light._ambientPow * light._ambientCol.rgb) + // no ambient, sun does it for us
		(diffuse + specular) * lightDistSq				// light factors from our single light
		* texSpec.x										// don't render when emissive
		) * light._lightCol.rgb;

		//emissive added already
		//result += texture(s_emissiveTex, inUV).rgb;

	frag_color = vec4(result, 1.0);
}
