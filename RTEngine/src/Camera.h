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

		void ResizeCamera(float width, float height);
		void RecalculateCashedCoords();
		const std::vector<glm::vec3>& GetCoords() const { return m_CashedCoord; }

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Direction;

		glm::mat4 m_InvProjection;
		glm::mat4 m_InvView;

		float m_Fov, m_Near, m_Far;
		int32_t m_ViewWidth, m_ViewHeight;

		std::vector<glm::vec3> m_CashedCoord;

		static const glm::vec3 c_Up;
	};

}
