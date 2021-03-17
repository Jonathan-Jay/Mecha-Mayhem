#pragma once
#include "IlluminationBuffer.h"
#include <GLM/gtc/matrix_transform.hpp>
#include "Utilities/BLM.h"

//aka the PP wrapper
class FrameEffects
{
public:
	FrameEffects();
	~FrameEffects() { RemoveAllEffects(); }

	//ensure you can't copy it, because of Framebuffer
	FrameEffects(const FrameEffects& other) = delete;
	FrameEffects(FrameEffects&& other) = delete;
	FrameEffects& operator=(const FrameEffects& other) = delete;
	FrameEffects& operator=(FrameEffects&& other) = delete;

	//for all effects (put static inits here)
	static void Init();
	//same as the init, but for unloading
	static void Unload();

	//change the buffers
	void Init(unsigned width, unsigned height);
	void Resize(unsigned width, unsigned height);

	//in order of added
	void AddEffect(PostEffect* effect);
	void InsertEffect(PostEffect* effect, int index);
	void RemoveEffect(int slot);

	//binds gBuffer
	void Bind();
	void UnBind();

	//binds shadow buffer
	void BindShadowMap(bool front);
	void UnBindShadowMap();

	//binds transparency buffer
	void BindTransparency();
	void UnBindTransparency();

	//for drawing purposes
	void Clear();
	void Draw(/*bool paused*/);

	//removes all effects
	void RemoveAllEffects();

	//Get amount of effects
	size_t size() { return layersOfEffects.size(); }

	//get the effect at the index, and nullptr if invalid
	PostEffect* operator[](size_t index) {
		if (layersOfEffects.size() <= index)	return nullptr;
		return layersOfEffects[index];
	}

	Framebuffer* GetDrawBuffer() {
		return &baseEffect.GetGBuffer();
	}

	void SetCamPos(glm::vec3 pos, int camNum) {
		illumBuffer.SetCamPos(pos, camNum);
	}

	void SetCamCount(int camNum) {
		illumBuffer.SetCamCount(camNum);
	}

	void SetShadowVP(const glm::mat4& VP) {
		m_shadowVP = VP;
		illumBuffer.SetLightSpaceViewProj(m_shadowVP);
	}

	void SetShadowVP(float left, float right, float nearZ, float farZ, const glm::vec3& position) {
					//Projection
		m_shadowVP = glm::ortho(left, right, left, right, nearZ, farZ) *
					//view rotation
					glm::lookAt(position + glm::vec3(illumBuffer.GetSunRef()._lightDirection), position, BLM::GLMup);
		illumBuffer.SetLightSpaceViewProj(m_shadowVP);
	}

	FrameEffects& Reattach() {
		illumBuffer.SetLightSpaceViewProj(m_shadowVP);
		return *this;
	}

	glm::mat4 GetShadowVP() const { return m_shadowVP; }

	static const unsigned shadowWidth	= 8192;//4096;
	static const unsigned shadowHeight	= 8192;//4096;
private:
	GBuffer baseEffect;
	IlluminationBuffer illumBuffer;
	//TransparencyLayer transparencyLayer;

	Framebuffer shadowMap;

	std::vector<PostEffect*> layersOfEffects = {};

	glm::mat4 m_shadowVP =
		//Projection
		glm::ortho(-10.f, 10.f, -10.f, 10.f, 10.f, -10.f) *
		//View
		glm::lookAt(glm::vec3(illumBuffer.GetSunRef()._lightDirection), BLM::GLMzero, BLM::GLMup);
};