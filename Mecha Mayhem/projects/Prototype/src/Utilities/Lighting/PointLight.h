#pragma once
#include <GLM/common.hpp>

//Copy this struct into your shader and you can create Uniform Buffers that include this type (see example including DirectionalLight)
struct PointLight
{
	//***SAME TYPES SHOULD BE GROUPED TOGETHER***\\
	//VEC4s 
	//SHOULD ALWAYS USE VEC4s (Vec3s get upscaled to Vec4s anyways, using anything less is a waste of memory)
	glm::vec4 _lightPos = glm::vec4(0.f, 0.f, 0.f, 0.f);
	glm::vec4 _lightCol = glm::vec4(2.5f, 2.5f, 2.5f, 0.f);

	//FLOATS
	float _lightConstantFalloff = 0.1f;
	float _lightLinearFalloff = 0.9f;
	float _lightQuadraticFalloff = 0.32f;
	float _lightSpecularPow = 1.0f;
	float _radius = 1.f;
};