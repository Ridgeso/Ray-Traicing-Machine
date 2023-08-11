#pragma once
#include <vector>
#include <cstdint>
#include <random>
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
		void ResetFrame() { m_FrameIndex = 0; }

	private:
		struct Payload
		{
			glm::vec4 Color;
			glm::vec3 HitPosition;
			glm::vec3 HitNormal;
			float HitDistance;
			int32_t ObjectId;
		};

		glm::vec4 PixelColor(glm::ivec2 pixel) const;
		Payload TraceRay(const Ray& ray) const;
		Payload ClosestHit(const Ray& ray, int32_t objectId, float hitDistance) const;
		Payload Miss(const Ray& ray) const;

	public:
		bool Accumulate;

	private:
		Image m_MainView;
		uint32_t m_FrameIndex;
		std::vector<glm::vec4> m_ViewCash;
		std::vector<glm::vec4> m_AccumulatedView;

		const Camera* m_ActiveCamera;
		const Scene* m_ActiveScene;

		std::vector<int32_t> m_CashedCoord;
	};
	
	float FastRandom(uint32_t& input);

}
