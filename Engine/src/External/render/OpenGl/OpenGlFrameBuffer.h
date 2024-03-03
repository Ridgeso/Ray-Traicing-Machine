#pragma once
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include "Engine/Render/FrameBuffer.h"
#include "OpenGlTexture.h"

namespace RT::OpenGl
{

	class OpenGlFrameBuffer : public FrameBuffer
	{
	public:
		OpenGlFrameBuffer(const glm::ivec2 size, const int32_t numOfAttachments);
		~OpenGlFrameBuffer() final;

		void bind() const final;
		void unbind() const final;

		const Texture& getAttachment(const uint32_t index = 0) const final;

	private:
		uint32_t renderId;
		uint32_t frameId;
		glm::ivec2 size;
		std::vector<OpenGlTexture> attachments;
	};

}
