#include "MapEditorScene.h"

void MapEditor::Init(int windowWidth, int windowHeight)
{
	ECS::AttachRegistry(&m_reg);
	PhysBody::Init(m_world);
	ECS::AttachWorld(m_world);
	std::string input = "playtestmap";
										//if true and false, will append a number to a copy of an existing file
										//new file	ovewrite
	m_colliders.Init(m_world, input,	false,		false);
	/*std::cout << "filename: " + input + "\n";
	if (!m_colliders.Init(m_world, input, false, false))
		std::cout << input + " failed to load, no collision boxes loaded\n";*/
	
	width = windowWidth;
	height = windowHeight;

	m_camCount = 1;

	/// Creating Entities
	cameraEnt = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(cameraEnt).SetFovDegrees(60.f).ResizeWindow(width, height);
	ECS::GetComponent<Transform>(cameraEnt).SetPosition(glm::vec3(0, 0, 0));

	rayPosEnt = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(rayPosEnt).LoadMesh("maps/GodHimself.obj");
	ECS::GetComponent<Transform>(rayPosEnt).SetPosition(glm::vec3(0, 0.f, 0)).SetScale(0.5f);
	{
		auto entity = ECS::CreateEntity();
		ECS::AttachComponent<MultiTextObj>(entity).LoadMesh("maps/MapUntextured.obj");
		ECS::GetComponent<Transform>(entity).SetPosition(glm::vec3(0, -30.f, 0)).SetScale(4.f);
	}

	/// End of creating entities
	Rendering::DefaultColour = glm::vec4(1.f, 0.5f, 0.5f, 0.5f);
	/*Rendering::LightCount = 2;
	Rendering::LightsPos[0] = BLM::GLMzero;
	Rendering::LightsColour[0] = glm::vec3(20.f);*/

	Rendering::hitboxes = &m_colliders;
	Rendering::effects = &m_effects;
	Rendering::frameEffects = &m_frameEffects;

	m_frameEffects.Init();
	auto& lights = m_frameEffects.GetLights();
	lights.clear();
	lights.resize(2);
	lights[0]._lightCol = glm::vec4(10.f, 10.f, 10.f, 0.f);

	m_frameEffects.SetShadowVP(-70, 70, 40, -60, glm::vec3(20.5f - 0.f, 0, 21.8f + 5.f));
}

void MapEditor::Update()
{
	/// start of loop

	/*if (ControllerInput::GetButtonDown(BUTTON::SELECT, CONUSER::ONE)) {
		QueueSceneChange(0);
		return;
	}*/
	if (Input::GetKeyDown(KEY::ESC)) {
		QueueSceneChange(0);
		return;
	}

	//camera movement here
	{
		auto& lights = m_frameEffects.GetLights();
		auto& camTrans = ECS::GetComponent<Transform>(cameraEnt);
		glm::vec3 change = BLM::GLMzero;
		//moving
		if (Input::GetKey(KEY::W))		{ change.z -= speed * Time::dt; }
		if (Input::GetKey(KEY::S))		{ change.z += speed * Time::dt; }

		if (Input::GetKey(KEY::D))		{ change.x += speed * Time::dt; }
		if (Input::GetKey(KEY::A))		{ change.x -= speed * Time::dt; }

		if (Input::GetKey(KEY::SPACE))	{ change.y += speed * Time::dt; }
		if (Input::GetKey(KEY::LSHIFT))	{ change.y -= speed * Time::dt; }

		if (change != BLM::GLMzero) {
			change = glm::rotate(camTrans.GetLocalRotation(), change);
			camTrans.SetPosition(camTrans.GetLocalPosition() + change);
		}

		bool rotChanged = false;
		//rotation
		if (Input::GetKey(KEY::UP))		{ rot.x += rotSpeed * Time::dt; rotChanged = true; }
		if (Input::GetKey(KEY::DOWN))	{ rot.x -= rotSpeed * Time::dt; rotChanged = true; }
		if (Input::GetKey(KEY::LEFT))	{ rot.y += rotSpeed * Time::dt; rotChanged = true; }
		if (Input::GetKey(KEY::RIGHT))	{ rot.y -= rotSpeed * Time::dt; rotChanged = true; }

		if (rotChanged) {
			camTrans.SetRotation(glm::angleAxis(rot.y, BLM::GLMup) * glm::angleAxis(rot.x, glm::vec3(1, 0, 0)));
		}

		if (showRay) {
			btVector3 rayPos = PhysBody::GetRaycast(camTrans.GetLocalPosition(), camTrans.GetForwards() * -10000.f);
			if (rayPos != btVector3()) {
				lights[1]._lightPos = glm::vec4(ECS::GetComponent<Transform>(rayPosEnt).SetPosition(rayPos).GetLocalPosition(), 0.f);
			}
		}
		else {
			ECS::GetComponent<Transform>(rayPosEnt).SetPosition(glm::vec3(0, 1000, 0));
		}

		lights[0]._lightPos = glm::vec4(camTrans.GetLocalPosition(), 0.f);
	}

	/// End of loop
}

void MapEditor::Exit()
{
	if (saveOnExit) {
		if (m_colliders.SaveToFile())
			std::cout << (debugText = "file save success\n");
		else
			std::cout << (debugText = "file save failed\n");
	}
}

Scene* MapEditor::Reattach()
{
	Rendering::DefaultColour = glm::vec4(1.f, 0.5f, 0.5f, 0.5f);
	/*Rendering::LightCount = 2;
	Rendering::LightsPos[0] = BLM::GLMzero;
	Rendering::LightsColour[0] = glm::vec3(20.f);*/

	return Scene::Reattach();
}

void MapEditor::ImGuiFunc()
{
	ImGui::SliderFloat("Movement speed", &speed, 0, 15);
	ImGui::SliderFloat("Camera rotation speed", &rotSpeed, 0, 5);

	ImGui::Checkbox("Save on exit", &saveOnExit);
	if (ImGui::Checkbox("Shoot ray from camera", &showRay)) {
		m_frameEffects.GetLights().resize(1 + showRay);
	}

	if (ImGui::Button("Toggle render spawn(ers)(points)") || Input::GetKeyDown(KEY::FSLASH)) {
		debugText = m_colliders.ToggleDrawSpawns() ? "drawing spawn(ers)(points)" : "not drawing spawn(ers)(points)";
	}
	if (ImGui::Button("Toggle render hitboxes") || Input::GetKeyDown(KEY::FSLASH)) {
		debugText = m_colliders.ToggleDraw() ? "drawing hitboxes" : "not drawing hitboxes";
	}

	m_colliders.Update(cameraEnt);


	if (ImGui::Button("Overwrite save")) {
		if (m_colliders.SaveToFile())
			std::cout << (debugText = "manual file overwrite success\n");
		else
			std::cout << (debugText = "manual file overwrite failed\n");
	}
	if (ImGui::Button("Save as new file")) {
		if (m_colliders.SaveToFile(false))
			std::cout << (debugText = "manual file save success\n");
		else
			std::cout << (debugText = "manual file save failed\n");
	}
	if (ImGui::Button("Load last save")) {
		if (m_colliders.LoadFromFile())
			std::cout << (debugText = "manual file load success\n");
		else
			std::cout << (debugText = "manual file load failed\n");
	}
	ImGui::Text(debugText.c_str());
}

