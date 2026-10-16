#include "igi/logger.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>

namespace igi::logging {

Logger& Logger::instance() noexcept {
    static Logger s_instance;
    return s_instance;
}

void Logger::set_min_level(LogLevel level) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    min_level_ = level;
}

LogLevel Logger::min_level() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    return min_level_;
}

void Logger::set_log_file(const std::filesystem::path& file_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
    std::error_code ec;
    std::filesystem::create_directories(file_path.parent_path(), ec);
    file_stream_.open(file_path, std::ios::out | std::ios::app);
}

void Logger::close_log_file() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
}

const char* Logger::level_string(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::Trace:   return "TRACE";
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Fatal:   return "FATAL";
    }
    return "UNKNOWN";
}

void Logger::log(
    LogLevel level,
    std::string_view subsystem,
    std::string_view message) noexcept {
    const auto now = std::chrono::system_clock::now();

    std::lock_guard<std::mutex> lock(mutex_);
    if (static_cast<std::uint8_t>(level) < static_cast<std::uint8_t>(min_level_)) {
        return;
    }

    // Format ISO-8601 timestamp with milliseconds
    const std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    std::tm tm_buf{};
#if defined(_WIN32)
    localtime_s(&tm_buf, &now_c);
#else
    localtime_r(&now_c, &tm_buf);
#endif

    std::ostringstream ss;
    ss << std::put_time(&tm_buf, "%Y-%m-%dT%H:%M:%S") << '.'
       << std::setw(3) << std::setfill('0') << ms.count()
       << " [" << level_string(level) << "] [" << subsystem << "] "
       << message << '\n';

    const std::string line = ss.str();

    // Write to stderr/stdout
    if (level == LogLevel::Error || level == LogLevel::Fatal || level == LogLevel::Warning) {
        std::cerr << line;
    } else {
        std::cout << line;
    }

    // Write to file stream if open
    if (file_stream_.is_open()) {
        file_stream_ << line;
        file_stream_.flush();
    }

    // Record in memory history for debug inspection
    if (in_memory_log_.size() >= kMaxHistory) {
        in_memory_log_.erase(in_memory_log_.begin(), in_memory_log_.begin() + 100);
    }
    in_memory_log_.push_back({
        level,
        std::string(subsystem),
        std::string(message),
        now
    });
}

std::vector<LogEntry> Logger::snapshot_recent_entries(std::size_t max_count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (in_memory_log_.size() <= max_count) {
        return in_memory_log_;
    }
    return std::vector<LogEntry>(
        in_memory_log_.end() - static_cast<std::ptrdiff_t>(max_count),
        in_memory_log_.end());
}

void Logger::clear_history() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    in_memory_log_.clear();
}

} // namespace igi::logging
