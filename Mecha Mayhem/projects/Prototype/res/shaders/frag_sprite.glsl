#version 410
layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inPosition;

uniform sampler2D s_texture;
uniform int receiveShadows;
uniform vec3 addColour;

const float specularPow = 0.1;
const float shininess = 4.0;

//multi render target
//we can render colour to all of these
layout(location = 0) out vec4 outColours;
layout(location = 1) out vec3 outNormals;
layout(location = 2) out vec3 outSpecs;
layout(location = 3) out vec3 outPositions;

void main() {
	vec4 result = texture(s_texture, inUV);
	
	if (result.a < 0.5)
		discard;

	outColours = vec4(result.rgb + addColour, 1.0);

	outNormals = (normalize(inNormal) * 0.5) + 0.5;

	outSpecs = vec3(specularPow, shininess, receiveShadows);

	outPositions = inPosition;
}