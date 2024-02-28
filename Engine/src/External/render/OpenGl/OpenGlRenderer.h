#pragma once
#include <Engine/Core/Base.h>
#include <vector>
#include <cstdint>
#include <random>
#include <glm/glm.hpp>

#include "Engine/Render/Renderer.h"
#include "OpenGlShader.h"
#include "OpenGlTexture.h"

namespace RT::OpenGl
{

	class OpenGlRenderer : public Renderer
	{
	public:
		OpenGlRenderer() = default;
		~OpenGlRenderer() = default;

		void init(const RenderSpecs& specs) override;
		void shutDown() override;

		bool recreateRenderer(const glm::ivec2 size) override;
		void render(const Camera& camera, const Shader& shader, const Scene& scene) override;

		void resetFrame() override { frameIndexUni.value = 0; }
		uint32_t getFrames() const override { return frameIndexUni.value; }

		int32_t getDescriptor() const override { return renderTex->getTexId(); }

		bool& getAccumulation() override { return accumulation; }
		bool& drawEnvironment() override { return drawEnvironmentUni.value; }
		uint32_t& maxBounces() override { return maxBouncesUni.value; }
		uint32_t& maxFrames() override { return maxFramesUni.value; }

	private:
		template <typename Data = void>
		struct UniformBase
		{
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
		static void loadOpenGlForGlfw();

	private:
		bool accumulation;

		Uniform<const int32_t> accumulationSamplerUni = { "AccumulationTexture", 0 };
		Uniform<const int32_t> renderSamplerUni = { "RenderTexture", 1 };
		Uniform<bool> drawEnvironmentUni = { "DrawEnvironment", false };
		Uniform<uint32_t> maxBouncesUni = { "MaxBounces", 5 };
		Uniform<uint32_t> maxFramesUni = { "MaxFrames", 1 };
		Uniform<int32_t> frameIndexUni = { "FrameIndex", 0 };
		Uniform<glm::ivec2> resolutionUni = { "Resolution", glm::ivec2(0) };
		Uniform<> materialsCountUni = { "MaterialsCount" };
		Uniform<> spheresCountUni = { "SpheresCount" };
		Uniform<uint32_t> cameraStorage = { "CameraBuffer", 0 };
		Uniform<uint32_t> materialsStorage = { "MaterialsBuffer", 0 };
		Uniform<uint32_t> spheresStorage = { "SpheresBuffer", 0 };

		Local<OpenGlTexture> accumulationTex, renderTex;
		uint32_t screenBufferId = 0, frameBufferId = 0, renderBufferId = 0;

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
