#version 420

in vec2 inUV;

layout(binding = 0) uniform sampler2D s_lightAccumTex;

uniform float exposure;
uniform float gamma;
uniform float HDRusage;

out vec4 frag_color;

//HDR tone mapping, courtesy of: https://learnopengl.com/Advanced-Lighting/HDR
void main() {

	vec3 accum = texture(s_lightAccumTex, inUV).rgb;

	vec3 mapped = vec3(1.0) - exp(-accum * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));

	frag_color = vec4(mix(accum, mapped, HDRusage), 1.0);
}
