#pragma once
#include "Utilities/Scene.h"
#include "LeaderBoard.h"
//#include "Utilities/Catmull.h"

class MainMenu : public Scene
{
public:
	MainMenu(const std::string& name);

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;

	virtual Scene* Reattach() override;
	virtual void ImGuiFunc() override {
		Scene::ImGuiFunc();
		if (m_scenePos != 1)	return;

		for (int i(0); i < 4; ++i) {
			if (!ECS::GetComponent<Player>(models[i]).IsPlayer()) {
				if (ImGui::Button(("Add player " + std::to_string(i + 1)).c_str())) {
					if (LeaderBoard::players[i].model == 0)
						LeaderBoard::players[i].model = 1;
					LeaderBoard::players[i].user = CONUSER(i);
					ECS::GetComponent<Player>(models[i]).Init(CONUSER::FOUR, LeaderBoard::players[i].model);
					playerSwapped[i] = true;
					m_confirmTimer = 1.f;
				}
			}
			else {
				if (ImGui::Button(("Remove player " + std::to_string(i + 1)).c_str())) {
					ECS::GetComponent<Player>(models[i]).Init(LeaderBoard::players[i].user = CONUSER::NONE, 0);
					playerSwapped[i] = true;
				}
			}
		}
	}

private:
	void FixDigits(int number);
	void FixColourUp(int currIndex);
	void FixColourDown(int currIndex);

	bool m_exit = false;
	bool playerSwapped[4] = {};

	int m_scenePos = 0;
	static const int maxSelect = 5;

	float zoomTime = 1.f;
	float m_exitHoldTimer = 1.f;
	float m_confirmTimer = 1.f;

	CatmullFollower cameraPath{ 1, {
		glm::vec3(-2.5f, 2.5f, 12.5f),
		glm::vec3(12.5f, 1.f, 12.5f),
		glm::vec3(12.5f, 2.5f, -2.5f),
		glm::vec3(-2.5f, 0.f, -2.5f)
	}, 8 };

	entt::entity charSelectParent = entt::null;

	//entt::entity arena = entt::null;

	entt::entity timerText = entt::null;
	entt::entity camera = entt::null;
	entt::entity title = entt::null;
	entt::entity text = entt::null;
	entt::entity charSelect = entt::null;
	entt::entity backGround = entt::null;
	entt::entity confirm = entt::null;
	entt::entity digit1 = entt::null;
	entt::entity digit2 = entt::null;
	entt::entity models[4] = {
		entt::null,
		entt::null,
		entt::null,
		entt::null
	};
	entt::entity popup[4] = {
		entt::null,
		entt::null,
		entt::null,
		entt::null
	};

	int colourIndex[4] = {
		0, 0, 0, 0
	};

	static const int mm_colourCount = 5;

	glm::vec3 colourChoices[mm_colourCount] = {
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, -0.1f, -0.1f),
		glm::vec3(-0.1f, -0.1f, 0.75f),
		glm::vec3(0.25f, 0.25f, 0.f),
		glm::vec3(0.5f, 0.1f, 0.15f)
	};
};

