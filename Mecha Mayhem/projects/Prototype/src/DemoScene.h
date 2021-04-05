#pragma once
#include "Utilities/Scene.h"

class DemoScene : public Scene
{
public:
	DemoScene(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f)) :
		Scene(name, gravity, true) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void DrawOverlay() override;
	virtual Scene* Reattach() override;
	virtual void ImGuiFunc() override {
		Scene::ImGuiFunc();

		ImGui::SliderFloat("timerPos", &m_yPos, -10, 10);

		for (int i(0); i < 4; ++i) {
			if (bodyEnt[i] != entt::null)
				if (ImGui::Button(("Give player " + std::to_string(i + 1) + " 1 kill").c_str()))
					ECS::GetComponent<Player>(bodyEnt[i]).GivePoints(1);
		}
	}
	virtual void Exit() override {
		m_gameTimer = -1.f;
		//impossible goal (if it does happen, then the game jsut ends)
		killGoal = 100;
	}

	static LUT3D gameEndCube;
private:
	static const float winWaitTime;
	float m_yPos = 8.5f;
	size_t killGoal = 100;

	float camDistance = 2.5f;
	float m_timer = 0.f;
	float m_gameTimer = -1.f;

	entt::entity bodyEnt[4] = { entt::null, entt::null, entt::null, entt::null };
	entt::entity Head[4] = { entt::null, entt::null, entt::null, entt::null };
	entt::entity cameraEnt[4] = { entt::null, entt::null, entt::null, entt::null };

	Sprite m_pauseSprite;
	Sprite m_colonSprite;
};

inline const float DemoScene::winWaitTime = 5.f;
inline LUT3D DemoScene::gameEndCube;

