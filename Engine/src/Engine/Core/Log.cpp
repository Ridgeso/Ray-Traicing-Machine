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
            makeShare<spdlog::sinks::basic_file_sink_mt>("backlog.log", true)
        };

        logSinks[0]->set_pattern("%^[%T:%e][%L] %n: %v%$");
        logSinks[1]->set_pattern("[%d-%m-%C %T:%e][%l] %n: %v");

        engineLogger = makeShare<spdlog::logger>(
            "ENG",
            logSinks.begin(),
            logSinks.end()
        );
        spdlog::register_logger(engineLogger);
        engineLogger->set_level(spdlog::level::trace);
        engineLogger->flush_on(spdlog::level::trace);


        clientLogger = makeShare<spdlog::logger>(
            "APP",
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
