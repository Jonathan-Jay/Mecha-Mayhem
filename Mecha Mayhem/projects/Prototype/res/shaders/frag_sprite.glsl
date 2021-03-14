#version 410
layout(location = 0) in vec2 inUV;

uniform sampler2D s_texture;

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

	outColours = result;

	outNormals = vec3(0);
	outSpecs = vec3(0);
	outPositions = vec3(0);
}