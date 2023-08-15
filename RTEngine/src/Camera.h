#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

namespace RT::Render
{

	class Camera
	{
	public:
		#pragma pack(1)
		struct Spec
		{
			glm::mat4 InvProjection;
			glm::mat4 InvView;
			glm::vec3 Position;
			float padding[1];
		};

	public:
		Camera(float fov, float near, float far);

		const glm::vec3& GetPosition() const { return m_Spec.Position; }
		glm::vec3& GetPosition() { return m_Spec.Position; }
		void SetPosition(const glm::vec3& newPos) { m_Spec.Position = newPos; }

		const glm::vec3& GetDirection() const { return m_Direction; }
		glm::vec3& GetDirection() { return m_Direction; }
		void SetDirection(const glm::vec3& newDir) { m_Direction = newDir; }

		const glm::mat4& GetInvProjection() const { return m_Spec.InvProjection; }
		const glm::mat4& GetInvView() const { return m_Spec.InvView; }

		const Spec& GetSpec() const { return m_Spec; }

		void RecalculateInvProjection();
		void RecalculateInvView();

		void ResizeCamera(int32_t width, int32_t height);

	private:
		Spec m_Spec;
		glm::vec3 m_Direction;

		float m_Fov, m_Near, m_Far;
		glm::ivec2 m_ViewSize;

		static const glm::vec3 c_Up;
	};

}
