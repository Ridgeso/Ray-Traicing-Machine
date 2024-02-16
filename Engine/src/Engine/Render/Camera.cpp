#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace RT
{
	const glm::vec3 Camera::Up = glm::vec3(0, 1, 0);

	Camera::Camera(float fov, float near, float far)
		: fov(fov), near(near), far(far), viewSize(0)
	{
		spec.position = glm::vec3(0, 0, 5);
		direction = glm::vec3(0, 0, -1);
		spec.invProjection = glm::mat4(0);

		RecalculateInvView();
	}

	void Camera::RecalculateInvProjection()
	{
		glm::mat4 projection = glm::perspectiveFov(glm::radians(45.0f), (float)viewSize.x, (float)viewSize.y, near, far);
		spec.invProjection = glm::inverse(projection);
	}

	void Camera::RecalculateInvView()
	{
		glm::mat4 view = glm::lookAt(spec.position, spec.position + direction, Up);
		spec.invView= glm::inverse(view);
	}

	void Camera::ResizeCamera(int32_t width, int32_t height)
	{
		if (viewSize.x == width && viewSize.y == height)
			return;

		viewSize = { width, height };
		RecalculateInvProjection();
	}

}
