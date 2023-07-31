#include <glad/glad.h>

#include "Image.h"

namespace RT::Render
{

	Image::Image(int32_t width, int32_t height, const uint32_t* data, int32_t slot)
		: m_Width(width), m_Height(height), m_TextureSlot(slot), m_TextureId(0)
	{
		if (data != nullptr)
			InitTexture(data);
	}

	void Image::Update(const uint32_t* data)
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Image::Resize(int32_t width, int32_t height)
	{
		m_Width = width;
		m_Height = height;
		glDeleteTextures(1, &m_TextureId);
		InitTexture(nullptr);
	}

	uint32_t Image::GetTexId() const
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureId);
		return m_TextureId;
	}

	void Image::InitTexture(const uint32_t* data)
	{
		glActiveTexture(GL_TEXTURE0 + m_TextureSlot);
		glGenTextures(1, &m_TextureId);
		glBindTexture(GL_TEXTURE_2D, m_TextureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

}
