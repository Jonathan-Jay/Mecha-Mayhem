#pragma once
#include "GBuffer.h"
#include "Utilities/Lighting/UniformBuffer.h"
#include "Utilities/Lighting/PointLight.h"
#include "Utilities/Lighting/DirectionalLight.h"


//This is a post effect to make our job easier
class IlluminationBuffer : public PostEffect
{
public:
	enum Lights
	{
		DIRECTIONAL = 1,
		POINT,
		AMBIENT
	};
	//Initializes framebuffer
	//Overrides post effect Init
	void Init(unsigned width, unsigned height) override;
	
	//Makes it so apply effect with a PostEffect does nothing for this object
	void ApplyEffect(PostEffect* buffer) override { };
	//Can only apply effect using GBuffer object
	void ApplyEffect(GBuffer* gBuffer);

	//get rid of sphere mesh
	void Unload() override;

	//basically useless in our version
	void DrawIllumBuffer();

	void SetLightSpaceViewProj(glm::mat4 lightSpaceViewProj);
	void SetViewProj(const glm::mat4& ViewProj) { heldVP = ViewProj; }
	void SetCamPos(glm::vec3 camPos, int camNum);
	void SetCamCount(int camNum);
	void SendLights(std::array<glm::vec3, MAX_LIGHTS>& lightsPos,
					std::array<glm::vec3, MAX_LIGHTS>& lightsColour, int LightCount) {
		_shaders[Lights::DIRECTIONAL]->SetUniform("lightsPos", *lightsPos.data(), LightCount);
		_shaders[Lights::DIRECTIONAL]->SetUniform("lightsColour", *lightsColour.data(), LightCount);
		_shaders[Lights::DIRECTIONAL]->SetUniform("lightCount", LightCount);
	}

	DirectionalLight& GetSunRef();
	
	//Sets the sun in the scene
	void SetSun(DirectionalLight newSun);
	void SetSun(glm::vec4 lightDir, glm::vec4 lightCol);

	void EnableSun(bool enabled);
	bool GetSunEnabled() const;

	static float GetPointLightRadius(const PointLight& light)
	{
		float lightMax = glm::max(glm::max(light._lightCol.r, light._lightCol.g), light._lightCol.b);
		return (-light._lightLinearFalloff + std::sqrtf(
			light._lightLinearFalloff * light._lightLinearFalloff - 4 * light._lightQuadraticFalloff * (light._lightConstantFalloff - (256.0 / 5.0) * lightMax)
		)) / (2 * light._lightQuadraticFalloff);
	}

	float power = 1;
	int meshChoice = 0;
	int currentLight = 0;
	bool drawVolumes = true;
	std::vector<PointLight> _lights = {};
private:
	glm::mat4 _lightSpaceViewProj;
	glm::mat4 heldVP;
	std::array<glm::vec3, 4> _camPos = {
		glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec3(0)
	};

	int _camCount = 1;

	bool _sunEnabled = true;

	UniformBuffer _sunBuffer;
	UniformBuffer _lightBuffer;
	
	DirectionalLight _sun;
};