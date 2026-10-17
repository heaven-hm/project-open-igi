#include "igi/retail/diagnostics/log_record.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>

namespace igi::retail::diagnostics
{
namespace
{

[[nodiscard]] std::string_view Log_LevelName(LogLevel log_level) noexcept
{
    switch (log_level)
    {
    case LogLevel::info:
        return "INFO";
    case LogLevel::warning:
        return "WARN";
    case LogLevel::error:
        return "ERROR";
    }

    return "UNKNOWN";
}

} // namespace

std::string Log_FormatTimestampedRecord(
    LogLevel log_level,
    std::chrono::system_clock::time_point timestamp,
    std::string_view subsystem,
    std::string_view message)
{
    using namespace std::chrono;

    const auto day_start = floor<days>(timestamp);
    const year_month_day calendar_day{day_start};
    const hh_mm_ss time_of_day{timestamp - day_start};
    const auto milliseconds = duration_cast<std::chrono::milliseconds>(time_of_day.subseconds()).count();

    std::ostringstream formatted_record;
    formatted_record << '['
                    << std::setfill('0') << std::setw(4) << static_cast<int>(calendar_day.year()) << '-'
                    << std::setw(2) << static_cast<unsigned int>(calendar_day.month()) << '-'
                    << std::setw(2) << static_cast<unsigned int>(calendar_day.day()) << 'T'
                    << std::setw(2) << time_of_day.hours().count() << ':'
                    << std::setw(2) << time_of_day.minutes().count() << ':'
                    << std::setw(2) << time_of_day.seconds().count() << '.'
                    << std::setw(3) << milliseconds << "Z] ["
                    << Log_LevelName(log_level) << "] ["
                    << subsystem << "] " << message << '\n';
    return formatted_record.str();
}

} // namespace igi::retail::diagnostics
