#pragma once
#include <vector>
#include <cstdint>
#include <random>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Scene.h"

namespace RT::Render
{

	class Renderer
	{
	public:
		Renderer();

		bool Invalidate(int32_t width, int32_t height);
		void Devalidate();

		void OnResize(int32_t width, int32_t height);
		bool RecreateRenderer(int32_t width, int32_t height);
		void Render(const Camera& camera, const Scene& scene);

		void ResetFrame() { m_FrameIndex = 0; }

		int32_t GetWidth() const { return m_RenderSize.x; }
		int32_t GetHeight() const { return m_RenderSize.y; }
		int32_t GetDescriptor() const { return m_RenderId; }

	public:
		bool Accumulate;

	private:
		void CompileShader(uint32_t shaderID, const std::string& source) const;

	private:
		uint32_t m_FrameIndex;

		glm::ivec2 m_SpecSize;
		glm::ivec2 m_RenderSize;
		uint32_t m_RenderId;
		uint32_t m_ScreenBuffer, m_FrameBufferId, m_RenderBuffer, m_Program;
		
		struct Vertices
		{
			float Coords[2];
			float TexCoords[2];
		} static constexpr s_Screen[] = {
			{ { -1.0f, -1.0f }, { -1.0f, -1.0f } },
			{ {  1.0f, -1.0f }, {  1.0f, -1.0f } },
			{ {  1.0f,  1.0f }, {  1.0f,  1.0f } },
			{ {  1.0f,  1.0f }, {  1.0f,  1.0f } },
			{ { -1.0f,  1.0f }, { -1.0f,  1.0f } },
			{ { -1.0f, -1.0f }, { -1.0f, -1.0f } }
		};
	};

}
