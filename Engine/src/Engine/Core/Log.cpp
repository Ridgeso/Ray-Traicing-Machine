#include "Log.h"
#include <array>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace RT::Core
{

	Share<spdlog::logger> Log::engineLogger{};
	Share<spdlog::logger> Log::clientLogger{};

	void Log::init()
	{
        auto logSinks = std::array<spdlog::sink_ptr, 2>{
            makeShare<spdlog::sinks::stdout_color_sink_mt>(),
            makeShare<spdlog::sinks::basic_file_sink_mt>("FILE_EXPLORER.log", true)
        };

        logSinks[0]->set_pattern("%^[%T] %n: %v%$");
        logSinks[1]->set_pattern("[%T] [%l] %n: %v");

        engineLogger = makeShare<spdlog::logger>(
            "Explorer",
            logSinks.begin(),
            logSinks.end()
        );
        spdlog::register_logger(engineLogger);
        engineLogger->set_level(spdlog::level::trace);
        engineLogger->flush_on(spdlog::level::trace);


        clientLogger = makeShare<spdlog::logger>(
            "Application",
            logSinks.begin(),
            logSinks.end()
        );
        spdlog::register_logger(clientLogger);
        clientLogger->set_level(spdlog::level::trace);
        clientLogger->flush_on(spdlog::level::trace);
	}

	void Log::shutdown()
	{
        spdlog::shutdown();
	}

    const Share<spdlog::logger>& Log::getEngineLogger()
    {
        return engineLogger;
    }

    const Share<spdlog::logger>& Log::getClientLogger()
    {
        return clientLogger;
    }

}
