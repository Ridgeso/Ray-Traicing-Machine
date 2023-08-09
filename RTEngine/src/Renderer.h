#pragma once
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Image.h"
#include "Scene.h"

namespace RT::Render
{

	class Renderer
	{
	public:
		Renderer();

		void OnResize(int32_t width, int32_t height);
		void Render(const Camera& camera, const Scene& scene);

		const Image& GetRenderedImage() const { return m_MainView; }

	private:
		glm::vec4 PixelColor(glm::vec2 pixel);
		Payload TraceRay(const Ray& ray);
		Payload ClosestHit(const Ray& ray, int32_t objectId);
		Payload Miss(const Ray& ray);

	private:
		uint32_t m_FrameIndex;
		uint32_t m_Seed;
		const Camera* m_ActiveCamera;
		const Scene* m_ActiveScene;

		Image m_MainView;
		std::vector<glm::vec4> m_ViewCash;

		std::vector<int32_t> m_CashedCoord;
	};
	
	float FastRandom(uint32_t& input);

}
