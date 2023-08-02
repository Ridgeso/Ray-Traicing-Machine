#pragma once
#include <vector>
#include <cstdint>

#include "Image.h"
#include "Scene.h"

namespace RT::Render
{

	class Renderer
	{
	public:
		Renderer();

		void Render(const Scene& scene);
		void OnResize(int32_t width, int32_t height);

		const Image& GetRenderedImage() const { return m_MainView; }

	private:
		uint32_t PixelColor(glm::vec3 coord, const Sphere& sphere);

	private:
		uint32_t m_FrameIndex;
		uint32_t m_Seed;
		Image m_MainView;
		uint32_t* m_ViewCash;

		Sphere m_MainShpere;
	};
	
	uint32_t FastRandom(uint32_t& input);

}
