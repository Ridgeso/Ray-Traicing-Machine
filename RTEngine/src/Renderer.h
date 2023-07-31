#pragma once
#include <vector>
#include <cstdint>

#include "Image.h"

namespace RT::Render
{

	class Renderer
	{
	public:
		Renderer();

		void Render();
		void OnResize(const int32_t width, const int32_t height);

		const Image& GetRenderedImage() const { return m_MainView; }

	private:
		uint32_t m_FrameIndex;
		uint32_t m_Seed;
		Image m_MainView;
	};
	
	uint32_t FastRandom(uint32_t& input);

}
