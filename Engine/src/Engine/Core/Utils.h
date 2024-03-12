#pragma once
#include <cstdint>

#define RT_FILE __builtin_FILE() 
#define RT_LINE __builtin_LINE()

namespace RT::Utils
{

	struct FileInfo
	{
		constexpr explicit FileInfo(
			const char* file = RT_FILE,
			const uint32_t line = RT_LINE)
			: file{file}, line{line}
		{}

		const char* file;
		const uint32_t line;
	};

}
