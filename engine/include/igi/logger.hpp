#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace igi::logging {

enum class LogLevel : std::uint8_t {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
};

struct LogEntry {
    LogLevel level{LogLevel::Info};
    std::string subsystem;
    std::string message;
    std::chrono::system_clock::time_point timestamp{};
};

class Logger final {
public:
    static Logger& instance() noexcept;

    void set_min_level(LogLevel level) noexcept;
    [[nodiscard]] LogLevel min_level() const noexcept;

    void set_log_file(const std::filesystem::path& file_path);
    void close_log_file() noexcept;

    void log(
        LogLevel level,
        std::string_view subsystem,
        std::string_view message) noexcept;

    [[nodiscard]] std::vector<LogEntry> snapshot_recent_entries(std::size_t max_count = 100) const;
    void clear_history() noexcept;

    [[nodiscard]] static const char* level_string(LogLevel level) noexcept;

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    mutable std::mutex mutex_;
    LogLevel min_level_{LogLevel::Info};
    std::ofstream file_stream_;
    std::vector<LogEntry> in_memory_log_;
    static constexpr std::size_t kMaxHistory = 1000;
};

// Exception base class with subsystem context and formatted message
class EngineException : public std::exception {
public:
    explicit EngineException(
        std::string subsystem,
        std::string message)
        : subsystem_(std::move(subsystem))
        , message_(std::move(message)) {
        formatted_ = "[" + subsystem_ + "] " + message_;
    }

    [[nodiscard]] const char* what() const noexcept override {
        return formatted_.c_str();
    }

    [[nodiscard]] const std::string& subsystem() const noexcept {
        return subsystem_;
    }

    [[nodiscard]] const std::string& raw_message() const noexcept {
        return message_;
    }

private:
    std::string subsystem_;
    std::string message_;
    std::string formatted_;
};

class AssetLoadException final : public EngineException {
public:
    explicit AssetLoadException(std::string message)
        : EngineException("Asset", std::move(message)) {}
};

class PhysicsException final : public EngineException {
public:
    explicit PhysicsException(std::string message)
        : EngineException("Physics", std::move(message)) {}
};

} // namespace igi::logging

#define IGI_LOG(level, subsystem, msg) \
    do { \
        std::ostringstream _igi_oss; \
        _igi_oss << msg; \
        ::igi::logging::Logger::instance().log(level, subsystem, _igi_oss.str()); \
    } while (false)

#define IGI_LOG_TRACE(subsystem, msg) IGI_LOG(::igi::logging::LogLevel::Trace, subsystem, msg)
#define IGI_LOG_DEBUG(subsystem, msg) IGI_LOG(::igi::logging::LogLevel::Debug, subsystem, msg)
#define IGI_LOG_INFO(subsystem, msg)  IGI_LOG(::igi::logging::LogLevel::Info, subsystem, msg)
#define IGI_LOG_WARN(subsystem, msg)  IGI_LOG(::igi::logging::LogLevel::Warning, subsystem, msg)
#define IGI_LOG_ERROR(subsystem, msg) IGI_LOG(::igi::logging::LogLevel::Error, subsystem, msg)
#define IGI_LOG_FATAL(subsystem, msg) IGI_LOG(::igi::logging::LogLevel::Fatal, subsystem, msg)
