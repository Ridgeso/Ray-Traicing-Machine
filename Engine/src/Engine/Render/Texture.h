#pragma once
#include <Engine/Core/Base.h>
#include <glm/glm.hpp>

namespace RT
{

	enum class ImageFormat
	{
		R8, RGB8, RGBA8, RGBA32F
	};

	struct Texture
	{
		virtual ~Texture() = 0;

		virtual void setBuff(const void* data) = 0;

		virtual void bind(const uint32_t slot = 0) const = 0;

		virtual const uint32_t getTexId() const = 0;
		virtual const int32_t getWidth() const = 0;
		virtual const int32_t getHeight() const = 0;

		static Local<Texture> create(const glm::ivec2 size, const ImageFormat imageFormat);
	};

}
