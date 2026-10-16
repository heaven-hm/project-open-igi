#include "igi/diagnostics.hpp"

#include <filesystem>
#include <string_view>

namespace igi::diagnostics
{

void configure(bool verbose, const std::filesystem::path& log_file)
{
    (void) log_file;
    logging::Logger::instance().set_min_level(verbose ? logging::LogLevel::Debug : logging::LogLevel::Info);
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
