#pragma once
#include <Engine/Core/Base.h>
#include <glm/glm.hpp>

#include "Engine/Render/Texture.h"

namespace RT
{

	struct FrameBuffer
	{
		virtual ~FrameBuffer() = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		
		virtual const Texture& getAttachment(const uint32_t index = 0) const = 0;

		static Share<FrameBuffer> create(const glm::ivec2 size, const int32_t numOfAttachments);
	};

}
