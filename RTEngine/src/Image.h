#pragma once
#include <cstdint>

namespace RT::Render
{

	class Image
	{
	public:
		Image(int32_t width, int32_t height, const uint32_t* data, int32_t slot = 0);

		void Update(const uint32_t* data);
		void Resize(int32_t width, int32_t height);
		int32_t GetWidth() const { return m_Width; }
		int32_t GetHeight() const { return m_Height; }
		uint32_t GetTexId() const;

	private:
		void InitTexture(const uint32_t* data);

	private:
		int32_t m_Width, m_Height;
		int32_t m_TextureSlot;
		uint32_t m_TextureId;
	};

}
