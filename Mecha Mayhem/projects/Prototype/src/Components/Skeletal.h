#pragma once

#include <string>

#include <tiny_gltf.h>

#include "Utilities/BLM.h"
#include "Engine/VertexArrayObject.h"
#include "Engine/Shader.h"

#define MAX_BONES 26

class SkeletalAnim
{
public:
	SkeletalAnim(const std::string& filename, const std::string& initialAnim);
	SkeletalAnim();

	//initialize the shader
	static void Init();

	//clear the vector
	static void Unload();

	//loads GLTF or GLB file can either (we dont do textures tho), with teh initial animation
	void Init(const std::string& filename, const std::string& initialAnim);

	//blend to a different animation, with time offset
	void BlendTo(const std::string& animName, bool looping, float time = 0, float duration = 0.1f);

	void Update(float dt);
private:
	//calculate the current pose
	void CalcPose();

	//skin data and all stored in one, sure it's not flexible, but it works since all our models use unique skeletons
	struct JointData {
		int index;

		std::vector<float> rotTimes = {};
		std::vector<glm::quat> rots = {};

		std::vector<float> posTimes = {};
		std::vector<glm::vec3> positions = {};
	};

	struct Animation {
		std::string animName;
		float duration;
		std::vector<JointData> data = {};
	};

	struct Model {
		std::string filename;
		VertexArrayObject::sptr mesh = VertexArrayObject::Create();
		std::vector<Animation> animations = {};
	};

	static std::vector<Model> m_models;

	static Shader::sptr m_shader;

	//current joint matrices
	std::array<glm::mat4, MAX_BONES> m_pose = {};
	//for blending
	std::array<glm::mat4, MAX_BONES> m_tempPose = {};

	//animation keys
	std::vector<int> rotKey = {};
	std::vector<int> posKey = {};

	glm::vec3 m_additiveColour = BLM::GLMzero;

	bool m_receiveShadows = true;
	bool m_looping = true;
	int m_index = -1;
	int m_currentAnim = -1;
	float m_timer = 0;
	float m_blendTimer = 0;
};