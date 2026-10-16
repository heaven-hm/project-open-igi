#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

namespace igi::logging
{

enum class LogLevel : int
{
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Fatal = 5,
};

struct LogEntry final
{
    LogLevel level{LogLevel::Info};
    std::string subsystem;
    std::string message;
    std::string timestamp;
};

class Logger final
{
  public:
    // Bounded in-memory ring so long headless sessions cannot grow without limit.
    static constexpr std::size_t kHistoryCapacity = 4096;
    static constexpr std::size_t kDefaultSnapshotLimit = 256;

    static Logger& instance();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    void set_min_level(LogLevel level) noexcept;
    [[nodiscard]] bool set_log_file(const std::filesystem::path& path) noexcept;
    [[nodiscard]] LogLevel min_level() const noexcept;
    void clear_history();
    void log(LogLevel level, std::string_view subsystem, std::string_view message);
    [[nodiscard]] std::vector<LogEntry>
    snapshot_recent_entries(std::size_t limit = kDefaultSnapshotLimit) const;

  private:
    Logger() = default;
    mutable std::mutex mutex_;
    LogLevel min_level_{LogLevel::Info};
    std::ofstream log_file_;
    std::vector<LogEntry> entries_;
};

void log(LogLevel level, std::string_view subsystem, std::string_view message);
} // namespace igi::logging
