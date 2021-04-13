#version 420

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_screenTex;

uniform float u_Spread;

//with teh help of https://xorshaders.weebly.com/tutorials/blur-shaders-5-part-1
void main() 
{
	vec4 result = vec4(0);

	//10 passes
	for (float i = 0.9; i <= 1.0; i += 0.01) {
		result += texture(s_screenTex, inUV * i * u_Spread + 0.5 - 0.5 * i * u_Spread);
	}

	frag_color = result / result.a;
}