// Diagnostic logging infrastructure - NOT a game native and NOT part of any retail flow.
//
// Owner rule 3(a): logging that never changes control flow, values or ordering is the only non-retail code allowed inside engine/.
// This header exists so every landing can leave a breadcrumb when the candidate dies, which is how the fail-closed guard and the startup
// prefix are debugged without guessing. It writes to $TMPDIR/igi1-diagnostic.log (or /tmp on non-Windows hosts) and flushes every line.
//
// It must never be called from code whose ordering matters relative to game state: call it at entry/exit boundaries and before an abort.
#ifndef IGI_DIAGNOSTIC_LOG_HPP
#define IGI_DIAGNOSTIC_LOG_HPP

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace igi::diagnostic {

inline const char* LogPath()
{
    static std::string path = [] {
        const char* temp = std::getenv("TMPDIR");
        std::string base = (temp != nullptr && temp[0] != '\0') ? temp : "/tmp";
        if (!base.empty() && base.back() != '/' && base.back() != '\\')
        {
            base += '/';
        }
        return base + "igi1-diagnostic.log";
    }();
    return path.c_str();
}

inline void LogLine(const char* tag, const char* format, ...)
{
    std::FILE* file = std::fopen(LogPath(), "a");
    if (file == nullptr)
    {
        return;
    }
    std::fprintf(file, "[%s] ", tag);
    va_list arguments;
    va_start(arguments, format);
    std::vfprintf(file, format, arguments);
    va_end(arguments);
    std::fputc('\n', file);
    std::fclose(file);
}

} // namespace igi::diagnostic

// Marker for the retail-VA a diagnostic line came from; keeps log lines traceable to the binary.
#define IGI_DIAG(tag, ...) ::igi::diagnostic::LogLine((tag), __VA_ARGS__)

#endif
