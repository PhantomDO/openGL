﻿#include <iostream>
#include <string>

#include "Entity.h"
#include "GameEntity.h"
#include "Helper.h"
#include "Input.h"
#include "Light.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Time.h"
#include "Window.h"

#define DEFAULT_LOADING 0

// for convenience
using namespace Engine;


//#ifdef WIN32
//This magic line is to force notebook computer that share NVidia and Intel graphics to use high performance GPU (NVidia).
//Intel graphics doesn't support OpenGL > 1.2
extern "C" _declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
//#endif

const GLfloat PI = 3.1415926535897932384626433832795f;

void ParametersLoading(	const std::string& meshesFolderPath, const std::string& texturesFolderPath, const Engine::Window& window,
						std::vector<GameEntity>& entities, std::vector<Light>& lights)
{
	auto meshesPaths = Tools::GetAssetFromDirectory(meshesFolderPath);
	auto texturesPaths = Tools::GetAssetFromDirectory(texturesFolderPath);

	auto meshList = Tools::ChoiceListFromDirectory(meshesFolderPath, meshesPaths);
	auto textureList = Tools::ChoiceListFromDirectory(texturesFolderPath, texturesPaths);

	int count;
	std::cout << "How many entities do you want ?";
	std::cin >> count;
	entities.reserve(count);
	for (int i = 0; i < count; ++i)
	{
		auto entity = GameEntity(Tools::StringFormat("Entity (%d)", i));

		if (!meshesPaths.empty())
		{
			int meshChoice = 0;
			if (meshesPaths.size() > 1)
			{
				std::string s;
				while (std::getline(std::cin, s))
				{
					if (std::stringstream(s) >> meshChoice) break;
					Debug::Log(meshList);
				}
			}

			Debug::Log(meshesPaths[meshChoice]);
			auto& mr = entity.AddComponent<MeshRenderer>();
			mr.SetMesh(Mesh(meshesPaths[meshChoice]));

			if (!texturesPaths.empty())
			{
				int texChoice = 0;
				if (texturesPaths.size() > 1)
				{
					std::string s;
					while (std::getline(std::cin, s))
					{
						if (std::stringstream(s) >> texChoice) break;
						Debug::Log(textureList);
					}
				}

				Debug::Log(texturesPaths[texChoice]);
				mr.AddTexture(Texture2D(texturesPaths[texChoice]));
			}
		}

		entities.emplace_back(std::move(entity));
	}

	count = 0;
	std::cout << "How many light do you want 0, 1, 2 ?";
	std::cin >> count;
	if (count > 2) count = 2;

	if (count > 0)
	{
		lights.reserve(count);
		for (int i = 0; i < count; ++i)
		{
			auto l = Light(glm::vec3(0), glm::vec3(1.0f),
			                glm::vec3(1.0f, 1.0f, 0.8f), glm::vec3(1.0f, 1.0f, 0.8f));
			l.parameters = LightParameters(window.GetMeshProgram().GetID(), i);
			l.meshParameters = LightParameters(window.GetLightProgram().GetID());
			auto& mr = l.AddComponent<MeshRenderer>();

			std::stringstream cubeObj;
			cubeObj << meshesFolderPath << "/cube.obj";
			mr.SetMesh(Mesh(cubeObj.str()));

			lights.emplace_back(std::move(l));
		}
	}
}

void DefaultLoading(const std::string& meshesFolderPath, const std::string& texturesFolderPath, const Engine::Window& window,
						std::vector<GameEntity>& entities, std::vector<Light>& lights)
{
	auto meshesPaths = Tools::GetAssetFromDirectory(meshesFolderPath);
	auto texturesPaths = Tools::GetAssetFromDirectory(texturesFolderPath);

	auto meshList = Tools::ChoiceListFromDirectory(meshesFolderPath, meshesPaths);
	auto textureList = Tools::ChoiceListFromDirectory(texturesFolderPath, texturesPaths);

	int count = 1;
	entities.reserve(count);
	if (!meshesPaths.empty())
	{
		auto entity = GameEntity(Tools::StringFormat("Entity (%d)", 0));
		Debug::Log(meshesPaths[0]);
		auto& mr = entity.AddComponent<MeshRenderer>();
		mr.SetMesh(Mesh(meshesPaths[0]));

		if (!texturesPaths.empty())
		{
			Debug::Log(texturesPaths[0]);
			mr.AddTexture(Texture2D(texturesPaths[0]));
		}

		entities.emplace_back(std::move(entity));
	}

	count = 2;
	lights.reserve(count);
	if (!meshesPaths.empty())
	{
		for (int i = 0; i < count; ++i)
		{
			auto light = Light(glm::vec3(0), glm::vec3(1.0f),
				glm::vec3(1.0f, 1.0f, 0.8f), glm::vec3(1.0f, 1.0f, 0.8f));
			light.parameters = LightParameters(window.GetMeshProgram().GetID(), i);
			light.meshParameters = LightParameters(window.GetLightProgram().GetID());
			auto& mr = light.AddComponent<MeshRenderer>();
			mr.SetMesh(Mesh(meshesPaths[0]));

			lights.emplace_back(std::move(light));
		}
	}
}


int main( int argc, char * argv[])
{
#ifdef WIN32
  //Passe la console en UTF-8, permettant d'afficher les accents.
  //Tous les fichiers de cette solution sont encodés en UTF-8.
	SetConsoleOutputCP(65001);
#endif
	constexpr int window_width = 1280;
	constexpr int window_height = 720;
	auto window = new Engine::Window(window_width, window_height, false);

	// COLOR
	auto background = glm::vec3(255.0f/255, 192.0f/255, 203.0f/255);
	auto orange = glm::vec3(0.39f, 1.0f, 1.0f);
	auto blue = glm::vec3(1.0f, 1.0f, 1.0f);
	glClearColor(background.r,	background.g,	background.b,	1.0f);

	const std::string meshesFolderPath = "../../Assets/models";
	const std::string texturesFolderPath = "../../Assets/textures";

	std::vector<GameEntity> entities;

	window->SetMeshProgram(ShaderProgram(
		Shader{ GL_VERTEX_SHADER, "MeshVertexShader.glsl" },
		Shader{ GL_FRAGMENT_SHADER, "MeshFragmentShader.glsl" }));

	// light
	std::vector<Light> lights;

	window->SetLightProgram(ShaderProgram(
		Shader{ GL_VERTEX_SHADER, "LightVertexShader.glsl" },
		Shader{ GL_FRAGMENT_SHADER, "LightFragmentShader.glsl" }));

#if DEFAULT_LOADING <= 0
	DefaultLoading(meshesFolderPath, texturesFolderPath, *window, entities, lights);
#else
	ParametersLoading(meshesFolderPath, texturesFolderPath, *window, entities, lights);
#endif

	GLuint usedLightCount = glGetUniformLocation(window->GetLightProgram().GetID(), "usedLightCount");
	GLuint usedLightMeshCount = glGetUniformLocation(window->GetMeshProgram().GetID(), "usedLightCount");

	window->GetLightProgram().Use();
	GLuint lightProjMatrix = glGetUniformLocation(window->GetLightProgram().GetID(), "proj");
	glUniformMatrix4fv(lightProjMatrix, 1, GL_FALSE, glm::value_ptr(window->camera.GetProjectionMatrix()));
	GLuint lightModelMatrix = glGetUniformLocation(window->GetLightProgram().GetID(), "model");

	glm::vec3 size = entities.empty() ? glm::vec3(1) : entities[0].GetComponent<MeshRenderer>().GetMesh().bounds.size;
		glUniformMatrix4fv(lightModelMatrix, 1, GL_FALSE,
		glm::value_ptr(glm::scale(glm::mat4(1), size / 40.0f)));

	//// utilise le programe creer precedement
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glfwSetTime(0);
	//// change la couleur de la fenetre
	glClearColor(background.r,	background.g,	background.b,	1.f);

	const GLint mvpID = glGetUniformLocation(window->GetMeshProgram().GetID(), "mvp");
	const GLint mvID = glGetUniformLocation(window->GetMeshProgram().GetID(), "mv");

	glm::vec3 min = entities.empty() ? glm::vec3(-1) : entities[0].GetComponent<MeshRenderer>().GetMesh().bounds.min;
	glm::vec3 max = entities.empty() ? glm::vec3(1) : entities[0].GetComponent<MeshRenderer>().GetMesh().bounds.max;
		auto meshSize = glm::distance(min, max);

	while (!glfwWindowShouldClose(window->GetWindowPtr()))
	{
		// per frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		Time::deltaTime = currentFrame - Time::lastFrame;
		Time::lastFrame = currentFrame;

		glfwPollEvents();
		// remet la couleur par default
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Mesh
		if (!entities.empty())
		{
			window->GetMeshProgram().Use();
			for (auto& entity : entities)
			{
				auto& renderer = entity.GetComponent<MeshRenderer>();
				glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(Engine::MeshRenderer::GetMVPMatrix(
					window->camera.GetProjectionMatrix(),
					window->camera.GetViewMatrix(),
					entity.GetTransform().GetModelMatrix()
				)));

				glUniformMatrix4fv(mvID, 1, GL_FALSE, glm::value_ptr(Engine::MeshRenderer::GetMVMatrix(
					window->camera.GetViewMatrix(),
					entity.GetTransform().GetModelMatrix()
				)));

				glUniform1ui(glGetUniformLocation(window->GetMeshProgram().GetID(), "texSample"),
					static_cast<GLuint>(renderer.GetTextures().size()));
				glUniform1i(glGetUniformLocation(window->GetMeshProgram().GetID(), "tex"), 0);

				renderer.Draw(window->GetMeshProgram());
			}
		}

		glUniform1ui(usedLightCount, static_cast<GLuint>(lights.size()));
		glUniform1ui(usedLightMeshCount, static_cast<GLuint>(lights.size()));

		// Lights
		if (!lights.empty())
		{
			window->GetLightProgram().Use();

			// Calcul position des lights;
			for (size_t i = 0; i < lights.size(); ++i)
			{
				float xorz = cos(currentFrame * 2.0f) * 0.5f * meshSize;
				float yorx = cos(currentFrame / 2.0f) * 0.5f * meshSize;
				float zory = sin(currentFrame * 2.0f) * 0.5f * meshSize;

				if (i % 2 == 0)
				{
					lights[i].position.x = xorz;
					lights[i].position.y = yorx;
					lights[i].position.z = zory;
				}
				else
				{
					lights[i].position.z = xorz;
					lights[i].position.x = yorx;
					lights[i].position.y = zory;
				}

				glm::vec4 lightPosition = window->camera.GetViewMatrix() * glm::vec4(lights[i].position, 1.0f);
				lights[i].position = glm::vec3(lightPosition) / lightPosition.w;

				glUniform3fv(lights[i].parameters.position, 1, glm::value_ptr(lights[i].position));
				glUniform3fv(lights[i].parameters.ambiant, 1, glm::value_ptr(lights[i].ambiant));
				glUniform3fv(lights[i].parameters.diffuse, 1, glm::value_ptr(lights[i].diffuse));
				glUniform3fv(lights[i].parameters.specular, 1, glm::value_ptr(lights[i].specular));

				glUniform3fv(lights[i].meshParameters.position, 1, glm::value_ptr(lights[i].position));
				glUniform3fv(lights[i].meshParameters.diffuse, 1, glm::value_ptr(lights[i].diffuse));

				lights[i].GetComponent<MeshRenderer>().Draw(window->GetLightProgram());
			}
		}

		glfwSwapBuffers(window->GetWindowPtr());
	}

	glfwDestroyWindow(window->GetWindowPtr());
	glfwTerminate();
	std::exit(EXIT_SUCCESS);
}