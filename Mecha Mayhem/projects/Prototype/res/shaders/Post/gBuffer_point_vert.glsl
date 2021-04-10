#version 420
layout(location = 0) in vec3 inPos;

uniform mat4 MVP;

//just use MVP
void main()
{
	gl_Position = MVP * vec4(inPos, 1.0);
}
