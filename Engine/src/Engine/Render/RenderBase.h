#pragma once

namespace RT
{

	enum class RenderAPI
	{
		None,
		OpenGL,
		Vulkan
	};

	extern RenderAPI GlobalRenderAPI;

}
