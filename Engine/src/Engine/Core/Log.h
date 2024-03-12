#pragma once
#include <Engine/Core/Base.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace RT::Core
{

	class Log
	{
	public:
		static void init();
		static void shutdown();

		static const Share<spdlog::logger>& getEngineLogger();
		static const Share<spdlog::logger>& getClientLogger();

	private:
		static Share<spdlog::logger> engineLogger;
		static Share<spdlog::logger> clientLogger;
	};

}

// Engine Logs Macros
#define RT_LOG_CRITICAL(...) ::RT::Core::Log::getEngineLogger()->critical(__VA_ARGS__)
#define RT_LOG_ERROR(...)    ::RT::Core::Log::getEngineLogger()->error(__VA_ARGS__)
#define RT_LOG_WARN(...)     ::RT::Core::Log::getEngineLogger()->warn(__VA_ARGS__)
#define RT_LOG_INFO(...)     ::RT::Core::Log::getEngineLogger()->info(__VA_ARGS__)
#define RT_LOG_DEBUG(...)    ::RT::Core::Log::getEngineLogger()->debug(__VA_ARGS__)
#define RT_LOG_TRACE(...)    ::RT::Core::Log::getEngineLogger()->trace(__VA_ARGS__)

// Clients Logs Macros
#define LOG_CRITICAL(...)    ::RT::Core::Log::getClientLogger()->critical(__VA_ARGS__)
#define LOG_ERROR(...)       ::RT::Core::Log::getClientLogger()->error(__VA_ARGS__)
#define LOG_WARN(...)        ::RT::Core::Log::getClientLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...)        ::RT::Core::Log::getClientLogger()->info(__VA_ARGS__)
#define LOG_DEBUG(...)       ::RT::Core::Log::getClientLogger()->debug(__VA_ARGS__)
#define LOG_TRACE(...)       ::RT::Core::Log::getClientLogger()->trace(__VA_ARGS__)
