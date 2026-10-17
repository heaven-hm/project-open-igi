#pragma once

#include <chrono>
#include <string>
#include <string_view>

namespace igi::retail::diagnostics
{

enum class LogLevel
{
    info,
    warning,
    error,
};

[[nodiscard]] std::string Log_FormatTimestampedRecord(
    LogLevel log_level,
    std::chrono::system_clock::time_point timestamp,
    std::string_view subsystem,
    std::string_view message);

} // namespace igi::retail::diagnostics
