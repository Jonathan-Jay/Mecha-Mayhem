#version 420

out vec4 frag_color;

uniform vec3 colour;

void main()
{
	frag_color = vec4(colour, 1.0);
}
