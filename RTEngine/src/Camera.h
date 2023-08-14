#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

namespace RT::Render
{

	class Camera
	{
	public:
		Camera(float fov, float near, float far);
	
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::vec3& GetPosition() { return m_Position; }
		void SetPosition(const glm::vec3& newPos) { m_Position = newPos; }

		const glm::vec3& GetDirection() const { return m_Direction; }
		glm::vec3& GetDirection() { return m_Direction; }
		void SetDirection(const glm::vec3& newDir) { m_Direction = newDir; }

		const glm::mat4& GetInvProjection() const { return m_InvProjection; }
		const glm::mat4& GetInvView() const { return m_InvView; }

		void RecalculateInvProjection();
		void RecalculateInvView();

		void ResizeCamera(int32_t width, int32_t height);

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Direction;

		glm::mat4 m_InvProjection;
		glm::mat4 m_InvView;

		float m_Fov, m_Near, m_Far;
		glm::ivec2 m_ViewSize;

		static const glm::vec3 c_Up;
	};

}
