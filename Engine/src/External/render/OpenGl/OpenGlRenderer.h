#pragma once
#include <Engine/Core/Base.h>
#include <vector>
#include <cstdint>
#include <random>
#include <glm/glm.hpp>

#include "Engine/Render/Renderer.h"
#include "OpenGlShader.h"
#include "OpenGlTexture.h"
#include "OpenGlBuffer.h"
#include "OpenGlFrameBuffer.h"

namespace RT::OpenGl
{

	class OpenGlRenderer : public Renderer
	{
	public:
		OpenGlRenderer() = default;
		~OpenGlRenderer() = default;

		void init(const RenderSpecs& specs) final;
		void shutDown() final;

		bool recreateRenderer(const glm::ivec2 size) final;
		void render(const Camera& camera, const Shader& shader, const VertexBuffer& vbuffer, const Scene& scene) final;

		int32_t getDescriptor() const final { return frameBuffer->getAttachment(1).getTexId(); }
		const RenderSpecs& getSpecs() const { return specs; }

	private:
		void resize(const glm::ivec2 size);
		static void loadOpenGlForGlfw();

	private:
		RenderSpecs specs;
		Local<OpenGlFrameBuffer> frameBuffer;
	};

}
