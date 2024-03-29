#pragma once
#include <cstdint>
#include <filesystem>

#include <glm/glm.hpp>

#include "Engine/Render/Texture.h"

namespace RT::OpenGl
{

	class OpenGlTexture : public Texture
	{
		using path = std::filesystem::path;

	public:
		OpenGlTexture(const glm::ivec2 size, const ImageFormat imageFormat);
		~OpenGlTexture() final;

		void setBuff(const void* data) final;

		void bind(const uint32_t slot = 0) const final;

		const uint32_t getTexId() const final { return texId; }
		const int32_t getWidth() const final { return size.x; }
		const int32_t getHeight() const final { return size.y; }

	private:
		constexpr int32_t imageFormat2GlFormat(const ImageFormat imageFormat);

	private:
		uint32_t texId;
		glm::ivec2 size;
		ImageFormat imageFormat;
	};

}
