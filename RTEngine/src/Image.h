#pragma once
#include <cstdint>
#include <glad/glad.h>

namespace RT::Render
{

	class Image
	{
	public:
		enum class Format : uint32_t
		{
			None = 0,
			RGBA32U = GL_UNSIGNED_BYTE,
			RGBA32F = GL_FLOAT
		};

	public:
		Image();
		Image(int32_t width, int32_t height, Format format, const void* data = nullptr, int32_t slot = 0);
		~Image();

		void Update(const void* data);
		void Resize(int32_t width, int32_t height);
		int32_t GetWidth() const { return m_Width; }
		int32_t GetHeight() const { return m_Height; }
		uint32_t GetTexId() const;
		void SetFormat(Format format) { m_Format = format; }

	private:
		void InitTexture(const void* data);

	private:
		int32_t m_Width, m_Height;
		int32_t m_TextureSlot;
		uint32_t m_TextureId;

		Format m_Format;
	};

}