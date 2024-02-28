#pragma once
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include "OpenGlTexture.h"

namespace RT::OpenGl
{

	class OpenGlFrameBuffer
	{
	public:
		OpenGlFrameBuffer(const glm::ivec2 size_, const int32_t numOfAttachments);
		~OpenGlFrameBuffer();

		void bind() const;
		void unbind() const;

		const OpenGlTexture& getAttachment(const uint32_t index = 0) const;

	private:
		uint32_t renderId;
		uint32_t frameId;
		glm::ivec2 size;
		std::vector<OpenGlTexture> attachments;
	};

}
