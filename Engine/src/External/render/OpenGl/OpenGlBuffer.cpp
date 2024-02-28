#include "OpenGlBuffer.h"

#include <glad/glad.h>

namespace RT::OpenGl
{

	OpenGlVertexBuffer::OpenGlVertexBuffer(const uint32_t size)
		: bufferId{}
	{
		glCreateBuffers(1, &bufferId);
		glBindBuffer(GL_ARRAY_BUFFER, bufferId);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGlVertexBuffer::~OpenGlVertexBuffer()
	{
		glDeleteBuffers(1, &bufferId);
	}

	void OpenGlVertexBuffer::setData(const uint32_t size, const void* data) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufferId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void OpenGlVertexBuffer::addVertexAttribute(
		const int32_t pos,
		const int32_t size,
		const int32_t attribSize,
		const int32_t offset) const
	{
		bind();
		glVertexAttribPointer(pos, size, GL_FLOAT, GL_FALSE, attribSize, (void*)offset);
		glEnableVertexAttribArray(pos);
		unbind();
	}

	void OpenGlVertexBuffer::bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	}
	
	void OpenGlVertexBuffer::unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}
