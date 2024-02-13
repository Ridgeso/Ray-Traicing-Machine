#pragma once
#include <vector>
#include <cstdint>
#include <random>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Scene.h"

namespace RT::Render
{

	struct RenderSpecs
	{
		int32_t width, height;
		bool accumulate;
	};

	class Renderer
	{
	public:
		Renderer() = default;

		void Init(const RenderSpecs& specs);
		void ShutDown();

		bool RecreateRenderer(int32_t width, int32_t height);
		void Render(const Camera& camera, const Scene& scene);

		void ResetFrame() { frameIndexUni.value = 0; }
		uint32_t GetFrames() const { return frameIndexUni.value; }

		int32_t GetDescriptor() const { return renderId; }

		bool& Accumulation() { return accumulation; }
		bool& DrawEnvironment() { return drawEnvironmentUni.value; }
		uint32_t& MaxBounces() { return maxBouncesUni.value; }
		uint32_t& MaxFrames() { return maxFramesUni.value; }

	private:
		template <typename Data = void>
		struct UniformBase
		{
			uint32_t ID;
			const std::string name;
		};

		template <typename Data = void>
		struct Uniform : UniformBase<Data>
		{
			Data value;
		};

		template <>
		struct Uniform<void> : UniformBase<> { };

	private:
		void Resize(int32_t width, int32_t height);
		void CompileShader(uint32_t shaderID, const std::string& source) const;

	private:
		bool accumulation;

		Uniform<const int32_t> accumulationSamplerUni = { 0, "AccumulationTexture", 0 };
		Uniform<const int32_t> renderSamplerUni = { 0, "RenderTexture", 1 };
		Uniform<bool> drawEnvironmentUni = { 0, "DrawEnvironment", false };
		Uniform<uint32_t> maxBouncesUni = { 0, "MaxBounces", 5 };
		Uniform<uint32_t> maxFramesUni = { 0, "MaxFrames", 1 };
		Uniform<int32_t> frameIndexUni = { 0, "FrameIndex", 0 };
		Uniform<glm::ivec2> resolutionUni = { 0, "Resolution", glm::ivec2(0) };
		Uniform<> materialsCountUni = { 0, "MaterialsCount" };
		Uniform<> spheresCountUni = { 0, "SpheresCount" };

		uint32_t accumulationId = 0, renderId = 0;
		uint32_t screenBufferId = 0, frameBufferId = 0, renderBufferId = 0, programId = 0;
		uint32_t cameraStorage = 0, materialsStorage = 0, spheresStorage= 0;

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
