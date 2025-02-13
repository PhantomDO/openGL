﻿#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "GameEntity.h"

namespace Engine
{
	class Camera : public GameEntity
	{
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		void UpdateView();

	public:
		float fov;
		float nearClip;
		float farClip;
		
		float yaw;
		float pitch;
		float speed;
		float sensitivity;

		Camera(const std::string& name = "Camera");

	public:
		void ProcessKeyboardEvent(const int& key, const float& dt);
		void ProcessMouseMouvement(glm::vec2& offset, GLboolean constrain = true);
		void ProcessMouseScroll(const float& yFov);

		glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
		void SetProjectionMatrix(const int& width, const int& height);

		glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }
		void SetViewMatrix();

	public:
		Camera& operator =(const Camera& camera) = default;
	};
}
