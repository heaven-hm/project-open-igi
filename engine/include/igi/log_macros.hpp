#pragma once
#include "igi/logger.hpp"

#include <sstream>
#include <string>
#define IGI_LOG_TRACE(subsystem, message)                                                                    \
    do                                                                                                       \
    {                                                                                                        \
        std::ostringstream _s;                                                                               \
        _s << message;                                                                                       \
        ::igi::logging::log(::igi::logging::LogLevel::Trace, subsystem, _s.str());                           \
    } while (false)
#define IGI_LOG_DEBUG(subsystem, message)                                                                    \
    do                                                                                                       \
    {                                                                                                        \
        std::ostringstream _s;                                                                               \
        _s << message;                                                                                       \
        ::igi::logging::log(::igi::logging::LogLevel::Debug, subsystem, _s.str());                           \
    } while (false)
#define IGI_LOG_INFO(subsystem, message)                                                                     \
    do                                                                                                       \
    {                                                                                                        \
        std::ostringstream _s;                                                                               \
        _s << message;                                                                                       \
        ::igi::logging::log(::igi::logging::LogLevel::Info, subsystem, _s.str());                            \
    } while (false)
#define IGI_LOG_WARNING(subsystem, message)                                                                  \
    do                                                                                                       \
    {                                                                                                        \
        std::ostringstream _s;                                                                               \
        _s << message;                                                                                       \
        ::igi::logging::log(::igi::logging::LogLevel::Warning, subsystem, _s.str());                         \
    } while (false)
#define IGI_LOG_ERROR(subsystem, message)                                                                    \
    do                                                                                                       \
    {                                                                                                        \
        std::ostringstream _s;                                                                               \
        _s << message;                                                                                       \
        ::igi::logging::log(::igi::logging::LogLevel::Error, subsystem, _s.str());                           \
    } while (false)
#define IGI_LOG_FATAL(subsystem, message)                                                                    \
    do                                                                                                       \
    {                                                                                                        \
        std::ostringstream _s;                                                                               \
        _s << message;                                                                                       \
        ::igi::logging::log(::igi::logging::LogLevel::Fatal, subsystem, _s.str());                           \
    } while (false)
