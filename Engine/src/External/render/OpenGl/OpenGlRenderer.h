#pragma once
#include <vector>
#include <cstdint>
#include <random>
#include <glm/glm.hpp>

#include "Engine/Render/Renderer.h"

namespace RT
{

	class OpenGlRenderer : public Renderer
	{
	public:
		OpenGlRenderer() = default;
		~OpenGlRenderer() = default;

		void init(const RenderSpecs& specs) override;
		void shutDown() override;

		bool recreateRenderer(const glm::ivec2 size) override;
		void render(const Camera& camera, const Scene& scene) override;

		void resetFrame() override { frameIndexUni.value = 0; }
		uint32_t getFrames() const override { return frameIndexUni.value; }

		int32_t getDescriptor() const override { return renderId; }

		bool& getAccumulation() override { return accumulation; }
		bool& drawEnvironment() override { return drawEnvironmentUni.value; }
		uint32_t& maxBounces() override { return maxBouncesUni.value; }
		uint32_t& maxFrames() override { return maxFramesUni.value; }

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
		void clear();
		void resize(const glm::ivec2 size);
		void compileShader(uint32_t shaderID, const std::string& source) const;
		static void loadOpenGlForGlfw();

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
