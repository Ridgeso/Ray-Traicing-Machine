#pragma once
#include <cstdint>
#include <tuple>

#include "Engine/Render/Buffer.h"

namespace RT::OpenGl
{

	class OpenGlVertexBuffer : public VertexBuffer
	{
	public:
		OpenGlVertexBuffer(const uint32_t size);
		OpenGlVertexBuffer(const uint32_t size, const void* data);
		~OpenGlVertexBuffer() final;

		void registerAttributes(const VertexElements& elements) const final;
		void setData(const uint32_t size, const void* data) const final;
		const int32_t getCount() const final { return count; }

		void bind() const final;
		void unbind() const final;

	private:
		int32_t calculateStride(const VertexElements& elements) const;

		constexpr static int32_t elementType2Size(const VertexElement element);
		constexpr static uint32_t elementType2GlType(const VertexElement element);
	private:
		uint32_t bufferId;
		uint32_t size;
		mutable int32_t count;
	};

}
