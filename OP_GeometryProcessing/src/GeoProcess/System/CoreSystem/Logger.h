#pragma once

#include <GeoProcess/System/CoreSystem/Core.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

// Log macros
#define GP_FATAL(...) ::GP::Logger::GetLogger()->critical(__VA_ARGS__);
#define GP_ERROR(...) ::GP::Logger::GetLogger()->error(__VA_ARGS__);
#define GP_WARN(...)  ::GP::Logger::GetLogger()->warn(__VA_ARGS__);
#define GP_INFO(...)  ::GP::Logger::GetLogger()->info(__VA_ARGS__);
#define GP_TRACE(...) ::GP::Logger::GetLogger()->trace(__VA_ARGS__);


namespace GP
{
	class Logger
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetLogger()
		{
			return m_Logger;
		}

	private:
		static Ref<spdlog::logger> m_Logger;

	};
}