#version 420
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

uniform vec3 colour;
uniform vec3 addColour;

uniform float specularStrength;
uniform float shininess;

//multi render target
//we can render colour to all of these
layout(location = 0) out vec4 outColours;
layout(location = 1) out vec3 outNormals;
layout(location = 2) out vec3 outSpecs;
layout(location = 3) out vec3 outPositions;

void main() {
	//colours
	outColours = vec4(colour + addColour, 1.0);
	
	//outputs normals as colour
	outNormals = (normalize(inNormal) * 0.5) + 0.5;

	//other classes store in material
	outSpecs = vec3(specularStrength, shininess, 1.0);

	//positions 
	outPositions = inPos;
}