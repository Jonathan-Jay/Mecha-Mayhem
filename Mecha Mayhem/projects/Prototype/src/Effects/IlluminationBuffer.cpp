#include "IlluminationBuffer.h"
#include "Components/ObjLoader.h"

VertexArrayObject::sptr __sphere = nullptr;
VertexArrayObject::sptr __cone = nullptr;
VertexArrayObject::sptr __cube = nullptr;

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
		_lightBuffer.AllocateMemory(sizeof(PointLight));

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
		Shader::sptr point = Shader::Create();
		point->LoadShaderPartFromFile("shaders/Post/gBuffer_point_vert.glsl", GL_VERTEX_SHADER);
		point->LoadShaderPartFromFile("shaders/Post/gBuffer_point_frag.glsl", GL_FRAGMENT_SHADER);
		point->Link();
		_shaders.push_back(point);
		_shaders.push_back(GetShader("shaders/Post/gBuffer_ambient_frag.glsl"));
		Shader::sptr point2 = Shader::Create();
		point2->LoadShaderPartFromFile("shaders/Post/gBuffer_point_vert.glsl", GL_VERTEX_SHADER);
		point2->LoadShaderPartFromFile("shaders/Post/volume_point_frag.glsl", GL_FRAGMENT_SHADER);
		point2->Link();
		_shaders.push_back(point2);
	}

	if (__sphere == nullptr) {
		__sphere = ObjLoader("maps/sphere.obj").GetVAO();
		__cone = ObjLoader("maps/cone.obj").GetVAO();
		__cube = ObjLoader("maps/cube.obj").GetVAO();
	}
}

void IlluminationBuffer::ApplyEffect(GBuffer* gBuffer)
{
	if (_sunEnabled) {
		//bind directional light shader
		_shaders[Lights::DIRECTIONAL]->Bind();

		//the shadow texture is sent elsewhere
		_shaders[Lights::DIRECTIONAL]->SetUniformMatrix("u_LightSpaceMatrix", _lightSpaceViewProj);

		_shaders[Lights::DIRECTIONAL]->SetUniform("u_camPos", *_camPos.data(), _camCount);
		_shaders[Lights::DIRECTIONAL]->SetUniform("camCount", _camCount);

		_sunBuffer.Bind(0);

		gBuffer->BindLighting();
		
		_buffers[1]->RenderToFSQ();

		gBuffer->UnbindLighting();

		_sunBuffer.Unbind(0);

		_shaders[Lights::DIRECTIONAL]->UnBind();
	}

	//insert all other lighting here
	//*
	VertexArrayObject::sptr mesh = nullptr;
	if (meshChoice == 0)
		mesh = __sphere;
	else if (meshChoice == 1)
		mesh = __cone;
	else
		mesh = __cube;

	if (_lights.size()) {
		//bind the light buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetGBuffer().GetHandle());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _buffers[1]->GetHandle());
		glBlitFramebuffer(0, 0, _buffers[1]->GetWidth(), _buffers[1]->GetHeight(),
			0, 0, _buffers[1]->GetWidth(), _buffers[1]->GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		_buffers[1]->Bind();
		glDisable(GL_CULL_FACE);

		//bind point light shader
		_shaders[Lights::POINT]->Bind();
		_shaders[Lights::POINT]->SetUniform("u_camPos", _camPos[0]);
		_shaders[Lights::POINT]->SetUniform("power", power);
		_shaders[Lights::POINT]->SetUniform("windowSize", glm::vec2(_buffers[0]->GetWidth(), _buffers[0]->GetHeight()));
		for (int i(0); i < _lights.size(); ++i) {
			_lightBuffer.SendData(reinterpret_cast<void*>(&_lights[i]), sizeof(PointLight));

			_lightBuffer.Bind(0);

			gBuffer->BindLighting();
			//bind the light accum buffer to keep adding to it
			_buffers[1]->BindColorAsTexture(0, 5);
			//draw light volume
			glm::mat4 mvp = glm::mat4(_lights[i]._radius);
			mvp[3] = glm::vec4(glm::vec3(_lights[i]._lightPos), 1.f);
			_shaders[Lights::POINT]->SetUniformMatrix("MVP", heldVP * mvp);
			mesh->Render();

			_buffers[1]->UnbindTexture(5);

			gBuffer->UnbindLighting();

			_lightBuffer.Unbind(0);
		}
		_shaders[Lights::POINT]->UnBind();
		glEnable(GL_CULL_FACE);
		_buffers[1]->Unbind();
	}
	//*/


	_shaders[Lights::AMBIENT]->Bind();
	//for rim lighting
	_shaders[Lights::AMBIENT]->SetUniform("u_camPos", *_camPos.data(), _camCount);
	_shaders[Lights::AMBIENT]->SetUniform("camCount", _camCount);

	_sunBuffer.Bind(0);

	gBuffer->BindLighting();
	_buffers[1]->BindColorAsTexture(0, 5);

	_buffers[0]->RenderToFSQ();

	_buffers[1]->UnbindTexture(5);
	gBuffer->UnbindLighting();

	_sunBuffer.Unbind(0);

	_shaders[Lights::AMBIENT]->UnBind();


	//draw lines
	if (drawVolumes && _lights.size()) {
		//bind the volume buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetGBuffer().GetHandle());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _buffers[0]->GetHandle());
		glBlitFramebuffer(0, 0, _buffers[0]->GetWidth(), _buffers[0]->GetHeight(),
			0, 0, _buffers[0]->GetWidth(), _buffers[0]->GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		_buffers[0]->Bind();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		_shaders[4]->Bind();
		for (int i(0); i < _lights.size(); ++i) {
			//draw light volume
			glm::mat4 mvp = glm::mat4(_lights[i]._radius);
			mvp[3] = glm::vec4(glm::vec3(_lights[i]._lightPos), 1.f);
			_shaders[4]->SetUniformMatrix("MVP", heldVP * mvp);
			glm::vec3 col = glm::vec3(1.f);
			if (currentLight == i)
				col.r = col.b = 0.5f;
			_shaders[4]->SetUniform("colour", col);
			mesh->Render();
		}
		_shaders[4]->UnBind();
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		_buffers[0]->Unbind();
	}
}
void IlluminationBuffer::Unload()
{
	PostEffect::Unload();
	__sphere = nullptr;
}
//*
void IlluminationBuffer::DrawIllumBuffer()
{
	_shaders[0]->Bind();

	_buffers[1]->BindColorAsTexture(0, 0);

	Framebuffer::DrawFullscreenQuad();

	_buffers[1]->UnbindTexture(0);

	_shaders[0]->UnBind();
}
//*/

void IlluminationBuffer::SetLightSpaceViewProj(glm::mat4 lightSpaceViewProj)
{
	_lightSpaceViewProj = lightSpaceViewProj;
}

void IlluminationBuffer::SetCamPos(glm::vec3 camPos, int camNum)
{
	if (camNum > 3 || camNum < 0)	return;

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

	//send the directional light data and bind it
	_sunBuffer.SendData(reinterpret_cast<void*>(&_sun), sizeof(DirectionalLight));
}

void IlluminationBuffer::SetSun(glm::vec4 lightDir, glm::vec4 lightCol)
{
	_sun._lightDirection = lightDir;
	_sun._lightCol = lightCol;

	//send the directional light data and bind it
	_sunBuffer.SendData(reinterpret_cast<void*>(&_sun), sizeof(DirectionalLight));
}

void IlluminationBuffer::EnableSun(bool enabled)
{
	_sunEnabled = enabled;
}

bool IlluminationBuffer::GetSunEnabled() const
{
	return _sunEnabled;
}
