#pragma once
#include "Engine/Render/Shader.h"

namespace RT::Vulkan
{

	class VulkanShader : public Shader
	{
	public:
		void use() const final;
		void unuse() const final;
		void destroy() final;
		const uint32_t getId() const final;

		void load(const std::string& shaderPath)  final;
	private:
		void setUniformImpl(const std::string& uniName, const int32_t size, const void* value) const final;
	};

}
