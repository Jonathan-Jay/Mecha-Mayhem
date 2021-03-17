#version 420
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColour;
layout(location = 3) in vec3 inSpec;
layout(location = 4) in vec2 inUV;

uniform vec3 addColour;
uniform int receiveShadows;

uniform sampler2D s_texture;

//multi render target
//we can render colour to all of these
layout(location = 0) out vec4 outColours;
layout(location = 1) out vec3 outNormals;
layout(location = 2) out vec3 outSpecs;
layout(location = 3) out vec3 outPositions;

void main() {
	//colours
	vec4 textureColour = texture(s_texture, inUV);
	outColours = vec4(inColour * textureColour.rgb + addColour, textureColour.a * inSpec.z);
	
	//outputs normals as colour
	outNormals = (normalize(inNormal) * 0.5) + 0.5;

	//other classes store in material
	outSpecs = inSpec;
	outSpecs.z = receiveShadows;

	//positions 
	outPositions = inPos;
}