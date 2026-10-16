#pragma once
// Compatibility shim: legacy format parsers call Logger::Get().Log().
// Routes into the dedicated igi::logging system (no editor.log default).
#include "igi/logger.hpp"

#include <string>
#include <vector>

enum class LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERR,
    FATAL
};

struct LogEntry
{
    LogLevel level;
    std::string message;
    std::string timestamp;
};

class Logger
{
  public:
    static Logger& Get()
    {
        static Logger instance;
        return instance;
    }

    void Init(const std::string& = {})
    {
    }

    void Log(LogLevel level, const std::string& message)
    {
        using LL = ::igi::logging::LogLevel;
        LL mapped = LL::Info;
        switch (level)
        {
        case LogLevel::DEBUG:
            mapped = LL::Debug;
            break;
        case LogLevel::INFO:
            mapped = LL::Info;
            break;
        case LogLevel::WARNING:
            mapped = LL::Warning;
            break;
        case LogLevel::ERR:
            mapped = LL::Error;
            break;
        case LogLevel::FATAL:
            mapped = LL::Fatal;
            break;
        }
        ::igi::logging::log(mapped, "formats", message);
    }

    const std::vector<LogEntry>& GetEntries() const
    {
        return entries_;
    }

    void Clear()
    {
        entries_.clear();
    }

  private:
    Logger() = default;
    std::vector<LogEntry> entries_;
};
