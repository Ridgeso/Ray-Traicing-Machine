#pragma once
#include <type_traits>
#include <string>
#include <cstdint>
#include <Engine/Core/Base.h>

#include <glm/glm.hpp>

namespace RT
{

	struct Shader
	{
		virtual void use() const = 0;
		virtual void unuse() const = 0;
		virtual void destroy() = 0;
		virtual const uint32_t getId() const = 0;

		virtual void load(const std::string& shaderPath) = 0;

		template <typename T>
		void setUniform(const std::string& uniName, const int32_t size, const T& value) const
		{
			if constexpr (std::is_pointer<T>::value)
			{
				setUniformImpl(uniName, size, value);
			}
			else
			{
				setUniformImpl(uniName, size, &value);
			}
		}

	private:
		virtual void setUniformImpl(const std::string& uniName, const int32_t size, const void* value) const = 0;
	};

	Local<Shader> createShader();

}
