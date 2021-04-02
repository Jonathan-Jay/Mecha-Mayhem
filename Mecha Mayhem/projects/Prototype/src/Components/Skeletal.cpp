#include "Skeletal.h"

std::vector<SkeletalAnim::Model> SkeletalAnim::m_models = {};

Shader::sptr SkeletalAnim::m_shader = nullptr;

SkeletalAnim::SkeletalAnim(const std::string& filename, const std::string& initialAnim)
{
	Init(filename, initialAnim);
}

SkeletalAnim::SkeletalAnim()
{
}

void SkeletalAnim::Init()
{
	m_shader = Shader::Create();
	m_shader->LoadShaderPartFromFile("shaders/vert_skeletal.glsl", GL_VERTEX_SHADER);
	m_shader->LoadShaderPartFromFile("shaders/frag_mat.glsl", GL_FRAGMENT_SHADER);
	m_shader->Link();
}

void SkeletalAnim::Unload()
{
	m_shader = nullptr;
}

void SkeletalAnim::Init(const std::string& filename, const std::string& initialAnim)
{
	m_blendTimer = 0;
	m_looping = true;

	//first try to find it:
	for (int i(0); i < m_models.size(); ++i) {
		if (m_models[i].filename == filename) {
			m_index = i;

			auto& data = m_models[i].animations;
			bool valid = false;
			//check if it exists
			for (int x(0); x < data.size(); ++x) {
				if (data[x].animName == initialAnim) {
					valid = true;
					m_currentAnim = x;
					break;
				}
			}

			//if not valid, throw error or smt
			if (!valid)
				throw std::runtime_error("didn't find anim");

			CalcPose();
			return;
		}
	}

	//do the loading
}

void SkeletalAnim::BlendTo(const std::string& animName, bool looping, float time, float duration)
{
	auto& data = m_models[m_index].animations;

	//ignore if same as current
	if (data[m_currentAnim].animName == animName)	return;

	bool valid = false;
	//check if it exists
	for (int i(0); i < data.size(); ++i) {
		if (data[i].animName == animName) {
			valid = true;
			m_currentAnim = i;
			break;
		}
	}

	//if not valid, stop
	if (!valid)		return;

	//update some info
	m_timer = time;
	m_blendTimer = duration;
	m_looping = looping;

	//store the current data into a temp one
	m_tempPose = m_pose;

	//calculate the pose
	CalcPose();
}

void SkeletalAnim::Update(float dt)
{

	auto& data = m_models[m_index].animations[m_currentAnim];

	if (m_blendTimer > 0) {
		//do the blending
		m_blendTimer -= dt;
		if (m_blendTimer <= 0) {
			m_blendTimer = 0;
		}
		else {
			//calcuate blended pose
			CalcPose();
			return;
		}
	}

	//deal with the timer
	if (m_looping) {
		m_timer += dt;
		while (m_timer >= data.duration)
			m_timer -= data.duration;
	}
	else if (m_timer < data.duration) {
		m_timer += dt;

		if (m_timer >= data.duration)
			m_timer = data.duration;
	}
	//means not looping and timer finished, so no need to update
	else return;


}

void SkeletalAnim::CalcPose()
{
	//blend
	if (m_blendTimer > 0) {


		return;
	}


}
