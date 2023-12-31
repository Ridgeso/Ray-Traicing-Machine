#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace RT::Render
{
	const glm::vec3 Camera::c_Up = glm::vec3(0, 1, 0);

	Camera::Camera(float fov, float near, float far)
		: m_Fov(fov), m_Near(near), m_Far(far), m_ViewSize(0)
	{
		m_Spec.Position = glm::vec3(0, 0, 5);
		m_Direction = glm::vec3(0, 0, -1);
		m_Spec.InvProjection = glm::mat4(0);

		RecalculateInvView();
	}

	void Camera::RecalculateInvProjection()
	{
		glm::mat4 projection = glm::perspectiveFov(glm::radians(45.0f), (float)m_ViewSize.x, (float)m_ViewSize.y, m_Near, m_Far);
		m_Spec.InvProjection = glm::inverse(projection);
	}

	void Camera::RecalculateInvView()
	{
		glm::mat4 view = glm::lookAt(m_Spec.Position, m_Spec.Position + m_Direction, c_Up);
		m_Spec.InvView= glm::inverse(view);
	}

	void Camera::ResizeCamera(int32_t width, int32_t height)
	{
		if (m_ViewSize.x == width && m_ViewSize.y == height)
			return;

		m_ViewSize = { width, height };
		RecalculateInvProjection();
	}

}
