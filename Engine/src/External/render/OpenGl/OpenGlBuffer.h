#pragma once
#include <cstdint>

namespace RT::OpenGl
{

	class OpenGlVertexBuffer
	{
	public:
		OpenGlVertexBuffer(const uint32_t size);
		~OpenGlVertexBuffer();

		void setData(const uint32_t size, const void* data) const;

		void addVertexAttribute(
			const int32_t pos,
			const int32_t size,
			const int32_t attribSize,
			const int32_t offset) const;

		void bind() const;
		void unbind() const;

	private:
		uint32_t bufferId;
	};

}
