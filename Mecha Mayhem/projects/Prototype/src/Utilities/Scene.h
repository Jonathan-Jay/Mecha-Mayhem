#pragma once
#include "Rendering.h"
#include "Time.h"
#include "Input.h"
#include <AudioEngine.h>

//post effect stuff
#include "Effects/Post/BloomEffect.h"
#include "Effects/Post/PixelEffect.h"
#include "Effects/Post/SepiaEffect.h"
#include "Effects/Post/GreyscaleEffect.h"
#include "Effects/Post/DepthOfField.h"
#include "Effects/Post/ColourCorrectionEffect.h"

class Scene
{
public:
	Scene(const std::string& name, const glm::vec3& gravity = glm::vec3(0.f), bool physics = true);
	~Scene();

	entt::registry* GetRegistry();
	std::string GetName();

	virtual void Init(int windowWidth, int windowHeight);

	//called on scene change
	virtual Scene* Reattach();

	//everyframe
	virtual void Update() {}
	//after physics update, mainly for lights and stuff
	virtual void LateUpdate() {}

	//drawing pauseScrenes and stuff
	virtual void DrawOverlay() {
		//Gbuffer fixes this
		//glViewport(0, 0, BackEnd::GetWidth(), BackEnd::GetHeight());
	}

	//called on scene change
	virtual void Exit() {
		AudioEngine::Instance().StopAllSounds();
	}

	//put ImGui stuff here
	virtual void ImGuiFunc() {
		/*ImGui::SetWindowSize(ImVec2(150, 50));
		ImGui::Text("Empty");*/

		if (ImGui::CollapsingHeader("Post Processing Effects"))
		{
			ImGui::Text(("Number of effects: " + std::to_string(m_frameEffects.size())).c_str());
			ImGui::SliderInt("max amt of effects", &maxEffectCount, 0, 5);
			for (int i(0); i < m_frameEffects.size(); ++i) {
				std::string name = m_frameEffects[i]->Info();
				if (ImGui::TreeNode((std::to_string(i + 1) + ": " + name).c_str())) {
					if (name == "Bloom") {
						BloomEffect* effect = (BloomEffect*)(m_frameEffects[i]);
						float threshold = effect->GetTreshold();
						if (ImGui::SliderFloat("Treshold", &threshold, 0.f, 1.f)) {
							effect->SetThreshold(threshold);
						}
						float radius = effect->GetRadius();
						if (ImGui::SliderFloat("Radius", &radius, 0.f, 15.f)) {
							effect->SetRadius(radius);
						}
						int blur = effect->GetBlurCount();
						if (ImGui::SliderInt("Blur Passes", &blur, 1, 15)) {
							effect->SetBlurCount(blur);
						}
					}
					else if (name == "Greyscale") {
						GreyscaleEffect* effect = (GreyscaleEffect*)(m_frameEffects[i]);
						float intensity = effect->GetIntensity();
						if (ImGui::SliderFloat("Intensity", &intensity, 0.f, 1.f)) {
							effect->SetIntensity(intensity);
						}
					}
					else if (name == "Sepia") {
						SepiaEffect* effect = (SepiaEffect*)(m_frameEffects[i]);
						float intensity = effect->GetIntensity();
						if (ImGui::SliderFloat("Intensity", &intensity, 0.f, 1.f)) {
							effect->SetIntensity(intensity);
						}
					}
					else if (name == "Pixel") {
						ImGui::Text("Pixels done by drawing to a smaller buffer, which means no new shader!");
						PixelEffect* effect = (PixelEffect*)(m_frameEffects[i]);
						int pixels = effect->GetPixelCount();
						if (ImGui::SliderInt("PixelCount", &pixels, 4, BackEnd::GetHeight())) {
							effect->SetPixelCount(pixels);
						}
					}
					else if (name == "DepthOfField") {
						ImGui::Text("We got the depth buffer somehow");
						DepthOfFieldEffect* effect = (DepthOfFieldEffect*)(m_frameEffects[i]);
						float depthLimit = effect->GetDepthLimit();
						if (ImGui::SliderFloat("Depth Limit", &depthLimit, 0.f, 1.f)) {
							effect->SetDepthLimit(depthLimit);
						}
						int blur = effect->GetBlurPasses();
						if (ImGui::SliderInt("Blur Passes", &blur, 1, 15)) {
							effect->SetBlurPasses(blur);
						}
					}
					if (name != "N/A")
					if (ImGui::Button("Remove")) {
						m_frameEffects.RemoveEffect(i);
					}
					ImGui::TreePop();
				}
			}
			if (m_frameEffects.size() < maxEffectCount) {
				if (ImGui::Button("Greyscale")) {
					GreyscaleEffect* effect = new GreyscaleEffect();
					effect->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
					effect->SetInfo("Greyscale");
					m_frameEffects.AddEffect(effect);
					effect = nullptr;
				}
				ImGui::SameLine();
				if (ImGui::Button("Sepia")) {
					SepiaEffect* effect = new SepiaEffect();
					effect->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
					effect->SetInfo("Sepia");
					m_frameEffects.AddEffect(effect);
					effect = nullptr;
				}
				if (ImGui::Button("Bloom")) {
					BloomEffect* effect = new BloomEffect();
					effect->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
					effect->SetInfo("Bloom");
					m_frameEffects.AddEffect(effect);
					effect = nullptr;
				}
				ImGui::SameLine();
				if (ImGui::Button("Pixelataion")) {
					PixelEffect* effect = new PixelEffect();
					effect->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
					effect->SetInfo("Pixel");
					m_frameEffects.AddEffect(effect);
					effect = nullptr;
				}
				ImGui::SameLine();
				if (ImGui::Button("Depth Of Field")) {
					DepthOfFieldEffect* effect = new DepthOfFieldEffect();
					effect->Init(BackEnd::GetWidth(), BackEnd::GetHeight());
					effect->SetInfo("DepthOfField");
					m_frameEffects.AddEffect(effect);
					effect->SetDrawBuffer(m_frameEffects.GetDrawBuffer());
					effect = nullptr;
				}
			}
			else {
				ImGui::Text("Max effects added");
			}
		}

	};

	//can't override
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
	bool m_paused = false;
	short m_camCount = 1;
	int maxEffectCount = 1;

	static size_t m_nextScene;
	static bool m_doSceneChange;
	static bool m_exitGame;

	entt::registry m_reg;
	btDiscreteDynamicsWorld *m_world = nullptr;
	HitboxGen m_colliders;
	Effects m_effects;
	FrameEffects m_frameEffects;

	btDbvtBroadphase *_broadphase = nullptr;
	btCollisionConfiguration* _collisionConfiguration = nullptr;
	btCollisionDispatcher* _dispatcher = nullptr;
	btSequentialImpulseConstraintSolver* _solver = nullptr;
};