#include "FrameEffects.h"

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
}

void FrameEffects::Resize(unsigned width, unsigned height)
{
	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->Reshape(width, height);
	}

	baseEffect.Reshape(width, height);
	illumBuffer.Reshape(width, height);
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
	illumBuffer.Clear();
	//transparencyLayer.Clear();
	for (int i(0); i < layersOfEffects.size(); ++i) {
		layersOfEffects[i]->Clear();
	}
	//pauseEffect.Clear();
}

void FrameEffects::Bind()
{
	baseEffect.Bind();
}

void FrameEffects::UnBind()
{
	baseEffect.Unbind();
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
	illumBuffer.ApplyEffect(&baseEffect);

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
	}
}
