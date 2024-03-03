#include "FrameBuffer.h"

#include "External/Render/OpenGl/OpenGlFrameBuffer.h"

namespace RT
{

	Share<FrameBuffer> FrameBuffer::create(const glm::ivec2 size, const int32_t numOfAttachments)
	{
		return makeShare<OpenGl::OpenGlFrameBuffer>(size, numOfAttachments);
	}

	FrameBuffer::~FrameBuffer() { }

}
