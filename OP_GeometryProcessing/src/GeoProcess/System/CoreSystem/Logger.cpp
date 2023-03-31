#include <Precomp.h>
#include <GeoProcess/System/CoreSystem/Logger.h>


namespace GP
{
	Ref<spdlog::logger> Logger::m_Logger;

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%T] %n(ThreadID %t) : %v%$");

		m_Logger = spdlog::stdout_color_mt("GP", spdlog::color_mode::always);
		m_Logger->set_level(spdlog::level::trace);
	}
}