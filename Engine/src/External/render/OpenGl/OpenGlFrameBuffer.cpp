#include "OpenGlFrameBuffer.h"

#include <glad/glad.h>


namespace RT::OpenGl
{

	OpenGlFrameBuffer::OpenGlFrameBuffer(const glm::ivec2 size, const int32_t numOfAttachments)
		: renderId{}, frameId{}, size{size}
	{
        glCreateFramebuffers(1, &frameId);
        glBindFramebuffer(GL_FRAMEBUFFER, frameId);

        std::vector<uint32_t> buffers;
        buffers.reserve(numOfAttachments);
        attachments.reserve(numOfAttachments);
        for (int32_t nrOfAttach = 0; nrOfAttach < numOfAttachments; nrOfAttach++)
        {
            buffers.push_back(GL_COLOR_ATTACHMENT0 + nrOfAttach);
            attachments.emplace_back(size, ImageFormat::RGBA32F);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + nrOfAttach, GL_TEXTURE_2D, attachments[nrOfAttach].getTexId(), 0);
        }

        glGenRenderbuffers(1, &renderId);
        glBindRenderbuffer(GL_RENDERBUFFER, renderId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderId);

        glDrawBuffers(numOfAttachments, buffers.data());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGlFrameBuffer::~OpenGlFrameBuffer()
	{
        glDeleteRenderbuffers(1, &renderId);
        glDeleteFramebuffers(1, &frameId);
	}

    void OpenGlFrameBuffer::bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameId);
        glViewport(0, 0, size.x, size.y);
    }

    void OpenGlFrameBuffer::unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    const Texture& OpenGlFrameBuffer::getAttachment(const uint32_t index) const
    {
        return attachments[index];
    }

}
