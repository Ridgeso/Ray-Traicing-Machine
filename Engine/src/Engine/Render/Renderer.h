#pragma once
#include <cstdint>
#include <Engine/Core/Base.h>
#include <glm/glm.hpp>

#include "Engine/Render/Camera.h"
#include "Engine/Render/Scene.h"
#include "Engine/Render/Shader.h"
#include "Engine/Render/Buffer.h"

namespace RT
{

	struct RenderSpecs
	{
		glm::ivec2 size;
	};

	struct Renderer
	{
		virtual void init(const RenderSpecs& specs) = 0;
		virtual void shutDown() = 0;

		virtual bool recreateRenderer(const glm::ivec2 size) = 0;
		virtual void render(const Camera& camera, const Shader& shader, const VertexBuffer& vbuffer, const Scene& scene) = 0;

		virtual int32_t getDescriptor() const = 0;
	};

	Local<Renderer> createRenderer();

}
