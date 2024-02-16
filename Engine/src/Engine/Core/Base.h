#pragma once
#include <memory>

namespace RT
{

	template <typename T>
	using Local = std::unique_ptr<T>;

	template <typename T, typename... Args>
	inline constexpr auto makeLocal(Args&&... args)
	{
		return std::make_unique<decltype(T(args...))>(std::forward<Args>(args)...);
	}

}
