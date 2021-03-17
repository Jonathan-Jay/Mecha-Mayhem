#include "FrameEffects.h"
#include "Components/Sprite.h"

FrameEffects::FrameEffects() { }

void FrameEffects::Init()
{
	PostEffect::Init("shaders/Post/passthrough_frag.glsl");

	//bloom shaders
	PostEffect::Init("shaders/Post/bloom_bright_pass.glsl");
	PostEffect::Init("shaders/Post/bloom_horizontal_blur.glsl");
	PostEffect::Init("shaders/Post/bloom_vertical_blur.glsl");
	PostEffect::Init("shaders/Post/bloom_combine_pass.glsl");

	//others
	//PostEffect::Init("shaders/Post/color_correction_frag.glsl");
	PostEffect::Init("shaders/Post/greyscale_frag.glsl");
	PostEffect::Init("shaders/Post/sepia_frag.glsl");
	PostEffect::Init("shaders/Post/toon_frag.glsl");

	PostEffect::Init("shaders/Post/dof_extract.glsl");
	PostEffect::Init("shaders/Post/dof_deletion.glsl");
	PostEffect::Init("shaders/Post/dof_horizontal_blur.glsl");
	PostEffect::Init("shaders/Post/dof_vertical_blur.glsl");
	PostEffect::Init("shaders/Post/dof_combine.glsl");

	//gbuffer
	PostEffect::Init("shaders/Post/gBuffer_directional_frag.glsl");
	PostEffect::Init("shaders/Post/gBuffer_ambient_frag.glsl");

}

void FrameEffects::Unload()
{
	PostEffect::UnloadShaders();
}

void FrameEffects::Init(unsigned width, unsigned height)
{
	RemoveAllEffects();

	baseEffect.Init(width, height);
	illumBuffer.Init(width, height);
	//transparencyLayer.Init(width, height);

	shadowMap.AddDepthTarget();
	shadowMap.SetWrapMode(WrapMode::ClampToBorder);
	shadowMap.Init(shadowWidth, shadowHeight);
	illumBuffer.SetLightSpaceViewProj(m_shadowVP);
}

void FrameEffects::Resize(unsigned width, unsigned height)
{
	baseEffect.Reshape(width, height);
	illumBuffer.Reshape(width, height);

	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->Reshape(width, height);
	}
	//transparencyLayer.Reshape(width, height);
}

void FrameEffects::AddEffect(PostEffect* effect)
{
	layersOfEffects.push_back(effect);
}

void FrameEffects::InsertEffect(PostEffect* effect, int index)
{
	if (index <= size()) {
		layersOfEffects.insert(layersOfEffects.begin() + index, effect);
	}
}

void FrameEffects::RemoveEffect(int slot)
{
	if (layersOfEffects.size() <= slot || slot < 0)	return;

	if (layersOfEffects[slot] != nullptr) {

		layersOfEffects[slot]->Unload();
		delete layersOfEffects[slot];
		layersOfEffects[slot] = nullptr;
	}
	layersOfEffects.erase(layersOfEffects.begin() + slot);
}

void FrameEffects::Clear()
{
	baseEffect.Clear();
	shadowMap.Clear();
	//transparencyLayer.Clear();
	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->Clear();
	}
	//pauseEffect.Clear();
	
	//clear the illum buffer with white instead of clear colour
	glClearColor(1.f, 1.f, 1.f, 0.3f);
	illumBuffer.Clear();
}

void FrameEffects::Bind()
{
	baseEffect.Bind();
}

void FrameEffects::UnBind()
{
	baseEffect.Unbind();
}

void FrameEffects::BindShadowMap(bool front)
{
	if (front) {
		shadowMap.Bind();
		glCullFace(GL_FRONT);
	}
	else {
		shadowMap.Bind();
		glCullFace(GL_BACK);
	}
}

void FrameEffects::UnBindShadowMap()
{
	shadowMap.Unbind();
}

void FrameEffects::BindTransparency()
{
	//transparencyLayer.BindBuffer(0);
	//usedTransparency = true;
}

void FrameEffects::UnBindTransparency()
{
	//transparencyLayer.UnbindBuffer();
}

void FrameEffects::Draw(/*bool paused*/)
{
	if (m_usingShadows)		shadowMap.BindDepthAsTexture(30);
	else		Sprite::m_textures[0].texture->Bind(30);
	
	illumBuffer.ApplyEffect(&baseEffect);

	Texture2D::Unbind(30);


	PostEffect* prev = &illumBuffer;

	/*if (usedTransparency) {
		usedTransparency = false;

		transparencyLayer.ApplyEffect(prev, &baseEffect);
		prev = &transparencyLayer;
	}*/

	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->ApplyEffect(prev);
		prev = layersOfEffects[i];
	}
	prev->DrawToScreen();
}

void FrameEffects::RemoveAllEffects()
{
	while (layersOfEffects.size()) {
		RemoveEffect(0);
		//RemoveEffect(layersOfEffects.size() - 1);
	}
}

void FrameEffects::SetShadowVP(const glm::mat4& VP)
{
	m_shadowVP = VP;
	illumBuffer.SetLightSpaceViewProj(m_shadowVP);
}

void FrameEffects::SetShadowVP(float left, float right, float nearZ, float farZ, const glm::vec3& position)
{
	//Projection
	m_shadowVP = glm::ortho(left, right, left, right, nearZ, farZ) *
		//view rotation
		glm::lookAt(position + glm::vec3(illumBuffer.GetSunRef()._lightDirection), position, BLM::GLMup);
	illumBuffer.SetLightSpaceViewProj(m_shadowVP);
}

FrameEffects& FrameEffects::Reattach()
{
	illumBuffer.SetLightSpaceViewProj(m_shadowVP);

	return *this;
}
