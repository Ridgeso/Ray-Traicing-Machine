#pragma once
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

namespace RT::Render
{

	class Renderer
	{
	public:
		Renderer();

		void Render();
		void OnResize(const int32_t width, const int32_t height);
		glm::ivec2 GetSize() const { return m_Size; }

		const std::vector<uint32_t>& GetRenderedImage() const { return m_Image; }

	private:
		glm::ivec2 m_Size;
		std::vector<uint32_t> m_Image;
	};

}
