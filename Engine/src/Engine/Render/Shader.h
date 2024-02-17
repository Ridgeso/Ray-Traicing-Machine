#pragma once
#include <string>
#include <Engine/Core/Base.h>

#include <glm/glm.hpp>

namespace RT
{

	struct Shader
	{
		virtual void use() const = 0;
		virtual void unuse() const = 0;
		virtual void destroy() = 0;

		virtual void load(const std::string& shaderPath) = 0;

		virtual void setInt(const std::string uniName, const int32_t value) const = 0;
		virtual void setUint(const std::string uniName, const uint32_t value) const = 0;
		virtual void setFloat(const std::string uniName, const float value) const = 0;
		virtual void setFloat2(const std::string uniName, const glm::vec2 value) const = 0;

		virtual void setStorage(const uint32_t storageId, const int32_t pos, const size_t size, const void* data) const = 0;

		virtual const uint32_t getId() const = 0;
	};

	Local<Shader> createShader();

}
