#pragma once
#include "Rendering.h"
#include "Time.h"
#include "Input.h"
#include <AudioEngine.h>
#include "Effects/Post/SepiaEffect.h"

class Scene
{
public:
	Scene(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f), bool physics = true);
	~Scene();

	entt::registry* GetRegistry();
	std::string GetName();

	virtual void Init(int windowWidth, int windowHeight);

	virtual Scene* Reattach();

	virtual void Update() {}
	virtual void Exit() {
		AudioEngine::Instance().StopAllSounds();
	}

	virtual void ImGuiFunc() {
		ImGui::SetWindowSize(ImVec2(150, 50));
		ImGui::Text("Empty");
	};

	virtual void BackEndUpdate() final;
	static bool GetExit() {
		return m_exitGame;
	}

	static void QueueSceneChange(size_t index);
	static void doSceneChange(GLFWwindow* window);

	static void UnloadScenes();

	static std::vector<Scene*> m_scenes;
	static Scene* m_activeScene;

protected:
	std::string m_name;
	short m_camCount = 1;
	int maxEffectCount = 1;
	bool m_paused = false;

	static size_t m_nextScene;
	static bool m_doSceneChange;
	static bool m_exitGame;

	entt::registry m_reg;
	btDiscreteDynamicsWorld *m_world = nullptr;
	HitboxGen m_colliders;
	Effects m_effects;
	Sprite m_pauseSprite;
	FrameEffects m_frameEffects;

	btDbvtBroadphase *_broadphase = nullptr;
	btCollisionConfiguration* _collisionConfiguration = nullptr;
	btCollisionDispatcher* _dispatcher = nullptr;
	btSequentialImpulseConstraintSolver* _solver = nullptr;
};

