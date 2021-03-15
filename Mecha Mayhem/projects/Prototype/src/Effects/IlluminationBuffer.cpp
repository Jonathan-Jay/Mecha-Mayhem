#include "IlluminationBuffer.h"

void IlluminationBuffer::Init(unsigned width, unsigned height)
{
	//composite buffer
	int index = int(_buffers.size());
	if (index == 0) {
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->AddDepthTarget();
		_buffers[index]->Init(width, height);

		//light accum buffer (stored in a RGBA16F for floating point numbers)
		index = int(_buffers.size());
		_buffers.push_back(new Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA16F);
		_buffers[index]->AddDepthTarget();
		_buffers[index]->Init(width, height);

		//allocates sun buffer
		_sunBuffer.AllocateMemory(sizeof(DirectionalLight));

		//if sun enabled, send data
		if (_sunEnabled) {
			_sunBuffer.SendData(reinterpret_cast<void*>(&_sun), sizeof(DirectionalLight));
		}
	}

	//Loads the shaders
	index = int(_shaders.size());
	if (index == 0) {
		_shaders.push_back(GetShader("shaders/Post/passthrough_frag.glsl"));
		_shaders.push_back(GetShader("shaders/Post/gBuffer_directional_frag.glsl"));
		_shaders.push_back(GetShader("shaders/Post/gBuffer_ambient_frag.glsl"));
	}
}

void IlluminationBuffer::ApplyEffect(GBuffer* gBuffer)
{
	//send the directional light data and bind it
	_sunBuffer.SendData(reinterpret_cast<void*>(&_sun), sizeof(DirectionalLight));

	if (_sunEnabled) {
		//bind directional light shader
		_shaders[Lights::DIRECTIONAL]->Bind();
		//_shaders[Lights::DIRECTIONAL]->SetUniformMatrix("u_LightSpaceMatrix", _lightSpaceViewProj);
		_shaders[Lights::DIRECTIONAL]->SetUniform("u_camPos", *_camPos.data(), 4);
		_shaders[Lights::DIRECTIONAL]->SetUniform("camCount", _camCount);

		_sunBuffer.Bind(0);

		gBuffer->BindLighting();
		
		_buffers[1]->RenderToFSQ();

		gBuffer->UnbindLighting();

		_sunBuffer.Unbind(0);

		_shaders[Lights::DIRECTIONAL]->UnBind();
	}

	_shaders[Lights::AMBIENT]->Bind();

	_sunBuffer.Bind(0);

	gBuffer->BindLighting();
	_buffers[1]->BindColorAsTexture(0, 4);

	_buffers[0]->RenderToFSQ();

	_buffers[1]->UnbindTexture(4);
	gBuffer->UnbindLighting();

	_sunBuffer.Unbind(0);

	_shaders[Lights::AMBIENT]->UnBind();
}

void IlluminationBuffer::DrawIllumBuffer()
{
	_shaders[0]->Bind();

	_buffers[1]->BindColorAsTexture(0, 0);

	Framebuffer::DrawFullscreenQuad();

	_buffers[1]->UnbindTexture(0);

	_shaders[0]->UnBind();
}

void IlluminationBuffer::SetLightSpaceViewProj(glm::mat4 lightSpaceViewProj)
{
	_lightSpaceViewProj = lightSpaceViewProj;
}

void IlluminationBuffer::SetCamPos(glm::vec3 camPos, int camNum)
{
	if (camNum >= 4 && camNum < 0)	return;

	_camPos[camNum] = camPos;
}

void IlluminationBuffer::SetCamCount(int camNum)
{
	_camCount = camNum;
}

DirectionalLight& IlluminationBuffer::GetSunRef()
{
	return _sun;
}

void IlluminationBuffer::SetSun(DirectionalLight newSun)
{
	_sun = newSun;
}

void IlluminationBuffer::SetSun(glm::vec4 lightDir, glm::vec4 lightCol)
{
	_sun._lightDirection = lightDir;
	_sun._lightCol = lightCol;
}

void IlluminationBuffer::EnableSun(bool enabled)
{
	_sunEnabled = enabled;
}