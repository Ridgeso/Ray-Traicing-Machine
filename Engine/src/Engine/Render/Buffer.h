#pragma once
#include <vector>
#include <cstdint>
#include <Engine/Core/Base.h>

#include <glm/glm.hpp>

namespace RT
{

	enum class VertexElement : uint8_t
	{
		None = 0,
		Int, Int2, Int3, Int4,
		Float, Float2, Float3, Float4
	};

	using VertexElements = std::vector<VertexElement>;

	struct VertexBuffer
	{
		virtual ~VertexBuffer() = 0;

		virtual void registerAttributes(const VertexElements& elements) const = 0;
		virtual void setData(const uint32_t size, const void* data) const = 0;
		virtual const int32_t getCount() const = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		
		static Local<VertexBuffer> create(const uint32_t size);
		static Local<VertexBuffer> create(const uint32_t size, const void* data);
	};

	namespace Utils
	{

		int32_t getNrOfComponents(const VertexElement element);

	}

}
