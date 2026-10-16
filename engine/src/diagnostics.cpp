#include "igi/diagnostics.hpp"

#include <filesystem>
#include <string_view>

namespace igi::diagnostics
{

void configure(bool verbose, const std::filesystem::path& log_file)
{
    auto& logger = logging::Logger::instance();
    logger.set_min_level(verbose ? logging::LogLevel::Debug : logging::LogLevel::Info);
    if (!logger.set_log_file(log_file))
    {
        logger.log(logging::LogLevel::Warning, "logging", "Cannot open log file: " + log_file.string());
    }
}

void log(Severity severity, std::string_view subsystem, std::string_view message)
{
    logging::LogLevel level = logging::LogLevel::Info;
    switch (severity)
    {
    case Severity::debug:
        level = logging::LogLevel::Debug;
        break;
    case Severity::information:
        level = logging::LogLevel::Info;
        break;
    case Severity::warning:
        level = logging::LogLevel::Warning;
        break;
    case Severity::error:
        level = logging::LogLevel::Error;
        break;
    }
    logging::log(level, subsystem, message);
}

} // namespace igi::diagnostics
