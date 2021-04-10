#version 420

struct PointLight
{
	//***SAME TYPES SHOULD BE GROUPED TOGETHER***\\
	//VEC4s 
	//SHOULD ALWAYS USE VEC4s (Vec3s get upscaled to Vec4s anyways, using anything less is a waste of memory)
	vec4 _lightPos;
	vec4 _lightCol;

	//FLOATS
	float _lightLinearFalloff;
	float _lightQuadraticFalloff;
	float _lightSpecularPow;
	float _radius;
};

layout (std140, binding = 0) uniform u_Light
{
	PointLight point;
};

layout(binding = 0) uniform sampler2D s_albedoTex;
layout(binding = 1) uniform sampler2D s_normalsTex;
layout(binding = 2) uniform sampler2D s_specularTex;
layout(binding = 3) uniform sampler2D s_positionTex;
layout(binding = 4) uniform sampler2D s_emissiveTex;
layout(binding = 5) uniform sampler2D s_lightAccumTex;

uniform vec3 u_camPos;

out vec4 frag_color;

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
// as well as https://learnopengl.com/Advanced-Lighting/Deferred-Shading
void main() {
	vec2 inUV = gl_FragCoord.xy;

	//	inNormal = (normalize(inNormal) * 2.0) - 1.0;
	vec3 inNormal = (normalize(texture(s_normalsTex, inUV).xyz) * 2.0) - 1.0;
	//Albedo
	vec4 textureColor = texture(s_albedoTex, inUV);
	//Specular
	vec3 texSpec = texture(s_specularTex, inUV).rgb;
	//Positions
	vec3 fragPos = texture(s_positionTex, inUV).rgb;

	// Diffuse
	vec3 lightDir = point._lightPos.xyz - fragPos;
	float lightDistSq = dot(lightDir, lightDir);
	lightDir = normalize(lightDir);
	float diffuse = max(dot(inNormal, lightDir), 0.0);// add diffuse intensity

	// Specular
	vec3 viewDir  = normalize(u_camPos - fragPos);
	// Get the specular from the specular map
	float specular = point._lightSpecularPow * pow(max(dot(inNormal, normalize(viewDir + lightDir)), 0.0), texSpec.y);

	vec3 result = ((diffuse + specular)	// light factors from our single light
		/ lightDistSq * texSpec.x)		// don't render when emissive
		* point._lightCol.rgb;

	//emissive added in directional light
	//result += texture(s_emissiveTex, inUV).rgb;

	if (textureColor.a < 0.31)
		result = vec3(0.0, 0.0, 0.0);

	frag_color = vec4(result + texture(s_lightAccumTex, inUV).rgb, 1.0);
}
