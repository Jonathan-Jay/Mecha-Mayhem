#pragma once
#include <Logging.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <json.hpp>
#include <fstream>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <entt/entt.hpp>
#include "Engine/VertexArrayObject.h"
#include "Engine/IndexBuffer.h"
#include "Engine/Shader.h"

class BackEnd abstract
{
public:
	//returns
	static GLFWwindow* Init(std::string name, int width, int height);
	static void Unload();

	static void SetAspect(int width, int height);

	static void SetFullscreen();
	static void SetTabbed(int width, int height);

private:
	static void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height);
	static GLFWwindow* window;
	static float _aspect;
	static float _aspect2;
	static int _lastHeight;
};