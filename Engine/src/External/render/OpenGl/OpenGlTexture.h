#pragma once
#include <cstdint>
#include <filesystem>

#include <glm/glm.hpp>

namespace RT::OpenGl
{

	enum ImageFormat
	{
		R8, RGB8, RGBA8, RGBA32F
	};

	class OpenGlTexture
	{
		using path = std::filesystem::path;

	public:
		OpenGlTexture(const glm::ivec2 size_, const ImageFormat imageFormat_);
		~OpenGlTexture();

		void setBuff(const void* data);

		void bind(const uint32_t slot = 0) const;

		const uint32_t getTexId() const { return texId; }
		const int32_t getWidth() const { return size.x; }
		const int32_t getHeight() const { return size.y; }

	private:
		constexpr int32_t imageFormat2GlFormat(const ImageFormat imageFormat);

	private:
		uint32_t texId;
		glm::ivec2 size;
		ImageFormat imageFormat;
	};

}
