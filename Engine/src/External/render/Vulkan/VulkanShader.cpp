#include "VulkanShader.h"

namespace RT::Vulkan
{

	void VulkanShader::use() const
	{
	}

	void VulkanShader::unuse() const
	{
	}

	void VulkanShader::destroy()
	{
	}

	const uint32_t VulkanShader::getId() const
	{
		return 0;
	}

	void VulkanShader::load(const std::string& shaderPath)
	{
	}

	void VulkanShader::setUniformImpl(const std::string& uniName, const int32_t size, const void* value) const
	{
	}

}
