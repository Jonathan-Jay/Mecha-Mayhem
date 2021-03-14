#version 420
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColour;
layout(location = 3) in vec3 inSpec;

uniform vec3 addColour;

//multi render target
//we can render colour to all of these
layout(location = 0) out vec4 outColours;
layout(location = 1) out vec3 outNormals;
layout(location = 2) out vec3 outSpecs;
layout(location = 3) out vec3 outPositions;

void main() {
	//colours
	outColours = vec4(inColour + addColour, inSpec.z);
	
	//outputs normals as colour
	outNormals = (normalize(inNormal) * 0.5) + 0.5;

	//other classes store in material
	outSpecs = inSpec;
		
	//positions 
	outPositions = inPos;
}