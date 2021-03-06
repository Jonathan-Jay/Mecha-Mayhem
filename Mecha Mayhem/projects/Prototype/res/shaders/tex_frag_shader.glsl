#version 410
#define MAX_LIGHTS 10
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColour;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inSpec;

uniform sampler2D s_texture;

uniform vec3 camPos;

uniform vec3  lightsPos[MAX_LIGHTS];
uniform vec3  lightsColour[MAX_LIGHTS];
uniform int lightCount;

uniform float ambientLightStrength;
uniform vec3  ambientColour;
uniform float ambientStrength;

out vec4 frag_color;

void main() {
	//optimized for less garbage collection and memory mangement
	vec3 N = normalize(inNormal);
	vec3 camDir = normalize(camPos - inPos);
	vec3 lightDir = vec3(0.0, 0.0, 0.0);
	vec3 total = vec3(0.0, 0.0, 0.0);

	for(int i = 0; i < lightCount; ++i) {
		lightDir = normalize(lightsPos[i] - inPos);

		//								 diffuse with attenuation									 SpecStrength													ShininessCoefficient
		total += (ambientLightStrength + max(dot(N, lightDir), 0.0) / length(lightsPos[i] - inPos) + inSpec.x * pow(max(dot(N, normalize(camDir + lightDir)), 0.0), inSpec.y)) * lightsColour[i];
	}

	vec4 textureColour = texture(s_texture, inUV);
	frag_color = vec4((ambientColour * ambientStrength + total) * inColour * textureColour.rgb, textureColour.a * inSpec.z);
}