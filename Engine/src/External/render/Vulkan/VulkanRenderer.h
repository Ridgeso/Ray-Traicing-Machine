#pragma once
#include "Engine/Render/Renderer.h"

namespace RT::Vulkan
{
	class VulkanRenderer : public Renderer
	{
	public:
		void init(const RenderSpecs& specs) final;
		void shutDown() final;

		void render(const Camera& camera, const Shader& shader, const VertexBuffer& vbuffer, const Scene& scene) final;
	private:

	};
}
