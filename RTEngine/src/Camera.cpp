#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace RT::Render
{
	const glm::vec3 Camera::c_Up = glm::vec3(0, 1, 0);

	Camera::Camera(float fov, float near, float far)
		: m_Fov(fov), m_Near(near), m_Far(far), m_ViewWidth(0), m_ViewHeight(0)
		, m_InvProjection(0), m_InvView(0)
	{
		m_Position = glm::vec3(0, 0, 5);
		m_Direction = glm::vec3(0, 0, -1);

		RecalculateInvView();
	}

	void Camera::RecalculateInvProjection()
	{
		glm::mat4 projection = glm::perspectiveFov(glm::radians(45.0f), (float)m_ViewWidth, (float)m_ViewHeight, m_Near, m_Far);
		m_InvProjection = glm::inverse(projection);
	}

	void Camera::RecalculateInvView()
	{
		glm::mat4 view = glm::lookAt(m_Position, m_Position + m_Direction, c_Up);
		m_InvView = glm::inverse(view);
	}

	void Camera::ResizeCamera(float width, float height)
	{
		if (m_ViewWidth == width && m_ViewHeight == height)
			return;

		m_ViewWidth = width;
		m_ViewHeight = height;

		RecalculateInvProjection();
		RecalculateCashedCoords();
	}

	void Camera::RecalculateCashedCoords()
	{
		m_CashedCoord.resize(m_ViewWidth * m_ViewHeight);
		for (int32_t y = 0; y < m_ViewHeight; y++)
		{
			for (int32_t x = 0; x < m_ViewWidth; x++)
			{
				glm::vec4 coord = 2.0f * glm::vec4((float)x / m_ViewWidth, (float)y / m_ViewHeight, 1, 1) - 1.0f;
				coord = m_InvProjection * coord;

				glm::vec3 rayDirection = glm::vec3(m_InvView * glm::vec4(glm::normalize(glm::vec3(coord) / coord.w), 0));

				m_CashedCoord[y * m_ViewWidth + x] = rayDirection;
			}
		}
	}

}
