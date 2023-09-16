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

		bool RecreateRenderer(int32_t width, int32_t height);
		void Render(const Camera& camera, const Scene& scene);

		void ResetFrame() { m_FrameIndex = 0; }
		uint32_t GetFrames() const { return m_FrameIndex; }

		int32_t GetWidth() const { return m_RenderSize.x; }
		int32_t GetHeight() const { return m_RenderSize.y; }
		int32_t GetDescriptor() const { return m_RenderId; }

	public:
		bool Accumulate, DrawEnvironment;
		uint32_t MaxBounces, MaxFrames;

	private:
		void CompileShader(uint32_t shaderID, const std::string& source) const;

	private:
		uint32_t m_FrameIndex;

		glm::ivec2 m_SpecSize;
		glm::ivec2 m_RenderSize;
		uint32_t m_AccumulationId = 0, m_RenderId = 0;
		uint32_t m_ScreenBuffer = 0, m_FrameBufferId = 0, m_RenderBuffer = 0, m_Program = 0;

		int32_t u_AccumulationTexture = 0, u_ScreenTexture = 0;
		int32_t u_DrawEnvironment = 0, u_MaxBounces = 0, u_MaxFrames = 0, u_FrameIndex = 0, u_Resolution = 0, u_MaterialsCount = 0, u_SpheresCount = 0;
		uint32_t u_CameraStorage = 0, u_MaterialsStorage = 1, u_SpheresStorage= 2;

		struct Vertices
		{
			float Coords[2];
			float TexCoords[2];
		} static constexpr s_Screen[] = {
			{ { -1.0f, -1.0f }, { 0.0f, 0.0f } },
			{ {  1.0f, -1.0f }, { 1.0f, 0.0f } },
			{ {  1.0f,  1.0f }, { 1.0f, 1.0f } },
			{ {  1.0f,  1.0f }, { 1.0f, 1.0f } },
			{ { -1.0f,  1.0f }, { 0.0f, 1.0f } },
			{ { -1.0f, -1.0f }, { 0.0f, 0.0f } }
		};
	};

}
