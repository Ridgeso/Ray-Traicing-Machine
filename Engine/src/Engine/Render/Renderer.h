#pragma once
#include <cstdint>
#include <Engine/Core/Base.h>
#include <glm/glm.hpp>

#include "Engine/Render/Camera.h"
#include "Engine/Render/Scene.h"
#include "Engine/Render/Shader.h"

namespace RT
{

	struct RenderSpecs
	{
		int32_t width, height;
		bool accumulate;
	};

	struct Renderer
	{
		virtual void init(const RenderSpecs& specs) = 0;
		virtual void shutDown() = 0;

		virtual bool recreateRenderer(const glm::ivec2 size) = 0;
		virtual void render(const Camera& camera, const Shader& shader, const Scene& scene) = 0;

		virtual void resetFrame() = 0;
		virtual uint32_t getFrames() const = 0;

		virtual int32_t getDescriptor() const = 0;

		virtual bool& getAccumulation() = 0;
		virtual bool& drawEnvironment() = 0;
		virtual uint32_t& maxBounces() = 0;
		virtual uint32_t& maxFrames() = 0;
	};

	Local<Renderer> createRenderer();

}
