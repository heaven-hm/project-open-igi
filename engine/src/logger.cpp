#include "igi/logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace igi::logging
{
namespace
{

const char* label(LogLevel level) noexcept
{
    switch (level)
    {
    case LogLevel::Trace:
        return "TRACE";
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARN";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Fatal:
        return "FATAL";
    }
    return "INFO";
}

std::string timestamp_now()
{
    const auto now = std::chrono::system_clock::now();
    const auto raw = std::chrono::system_clock::to_time_t(now);
    const auto* fields = std::gmtime(&raw);
    std::ostringstream out;
    out << '[';
    if (fields != nullptr)
    {
        out << std::put_time(fields, "%Y-%m-%d %H:%M:%S UTC");
    }
    else
    {
        out << "time unavailable";
    }
    out << ']';
    return out.str();
}

} // namespace

Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::set_min_level(LogLevel level) noexcept
{
    std::lock_guard lock(mutex_);
    min_level_ = level;
}

bool Logger::set_log_file(const std::filesystem::path& path) noexcept
{
    std::lock_guard lock(mutex_);
    log_file_.close();
    if (path.empty())
    {
        return true;
    }
    try
    {
        if (path.has_parent_path())
        {
            std::filesystem::create_directories(path.parent_path());
        }
        log_file_.open(path, std::ios::out | std::ios::app);
        return static_cast<bool>(log_file_);
    }
    catch (const std::filesystem::filesystem_error&)
    {
        return false;
    }
}

LogLevel Logger::min_level() const noexcept
{
    std::lock_guard lock(mutex_);
    return min_level_;
}

void Logger::clear_history()
{
    std::lock_guard lock(mutex_);
    entries_.clear();
}

void Logger::log(LogLevel level, std::string_view subsystem, std::string_view message)
{
    std::lock_guard lock(mutex_);
    if (static_cast<int>(level) < static_cast<int>(min_level_))
    {
        return;
    }
    LogEntry entry{level, std::string(subsystem), std::string(message), timestamp_now()};
    std::cerr << entry.timestamp << ' ' << label(entry.level) << " [" << entry.subsystem << "] "
              << entry.message << '\n';
    if (log_file_)
    {
        log_file_ << entry.timestamp << ' ' << label(entry.level) << " [" << entry.subsystem << "] "
                  << entry.message << '\n';
    }
    if (entries_.size() < kHistoryCapacity)
    {
        entries_.push_back(std::move(entry));
    }
    else
    {
        entries_.erase(entries_.begin());
        entries_.push_back(std::move(entry));
    }
}

std::vector<LogEntry> Logger::snapshot_recent_entries(std::size_t limit) const
{
    std::lock_guard lock(mutex_);
    if (limit >= entries_.size())
    {
        return entries_;
    }
    const auto start = entries_.size() - limit;
    return std::vector<LogEntry>(entries_.begin() + static_cast<std::ptrdiff_t>(start), entries_.end());
}

void log(LogLevel level, std::string_view subsystem, std::string_view message)
{
    Logger::instance().log(level, subsystem, message);
}

} // namespace igi::logging
