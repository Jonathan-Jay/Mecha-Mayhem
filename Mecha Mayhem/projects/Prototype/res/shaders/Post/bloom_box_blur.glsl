#version 420

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_screenTex;

uniform vec2 u_TexelSize;
uniform float u_Spread;

//done with the guidance of https://learnopengl.com/Advanced-Lighting/Bloom and spritelib bloom shaders
//also https://pingpoli.medium.com/the-bloom-post-processing-effect-9352fa800caf for the spread adjustability
void main() 
{
	vec4 result = vec4(0);

	//10 passes
	for (float x = -1; x <= 1; x += 0.2) {
		//another 10 passes
		for (float y = -1; y <= 1; y += 0.2) {
			result += texture(s_screenTex, inUV + vec2(x, y) * u_TexelSize * u_Spread);
		}
	}

	frag_color = result / result.a;
}