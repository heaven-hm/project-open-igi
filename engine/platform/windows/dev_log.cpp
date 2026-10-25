// igi.log - development logging implementation. NOT a retail native; no retail VA.
// Permitted by section 10.13 rule 3(a). See dev_log.hpp for the contract and the line format.
//
// Constraints this file is written to, all of which matter for a matching decompilation:
//   - It must never be reachable from a native's computation. Nothing here reads or writes a
//     retail global or calls a retail routine.
//   - It must never throw, never abort, and never allocate on a path that can fail. A logger
//     that crashes the game is worse than no logger.
//   - It must be thread-safe by a plain critical section: retail's own task system runs handlers
//     from more than one thread, and interleaved half-lines would make the log unreadable
//     exactly when it is needed most.
//   - Every string operation is bounded. Truncation is logged as truncation.
//   - It never reads a register or a stack slot to obtain a value it was not handed. Arguments
//     and return values arrive from the transcription, which is the only place they are known.
#include "dev_log.hpp"

#include <windows.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace igi::devlog {
namespace {

// One FILE* guarded by one critical section. A single handle keeps ordering honest: two threads
// that both log get a total order rather than two interleaved streams.
FILE *g_file = nullptr;
CRITICAL_SECTION g_lock;
bool g_lock_ready = false;
Level g_level = Level::Info;

// 4 KiB is enough for every line this project emits and small enough to sit on the stack of a
// retail native without moving its frame anywhere near a page boundary.
constexpr int kLineCapacity = 4096;
constexpr int kNameWidth = 34;
constexpr int kLabelWidth = 22;

// Padded to five columns so the level field lines up in every line of the log.
const char *LevelTag(Level level) noexcept {
    switch (level) {
        case Level::Error: return "ERROR";
        case Level::Warn:  return "WARN ";
        case Level::Info:  return "INFO ";
        case Level::Debug: return "DEBUG";
        default:           return "?????";
    }
}

// Unpadded, for prose (the open/close banners) where the column alignment is noise.
const char *LevelName(Level level) noexcept {
    switch (level) {
        case Level::Error: return "ERROR";
        case Level::Warn:  return "WARN";
        case Level::Info:  return "INFO";
        case Level::Debug: return "DEBUG";
        default:           return "UNKNOWN";
    }
}

// "2026-10-25 17:45:12.345" - local wall clock, so a run can be correlated against other tools
// and against a debugger. GetLocalTime is used rather than GetSystemTime because a developer
// reading the log is reading it in local time.
void FormatTimestamp(char *out, int capacity) noexcept {
    SYSTEMTIME st;
    GetLocalTime(&st);
    _snprintf(out, static_cast<size_t>(capacity) - 1U, "%04u-%02u-%02u %02u:%02u:%02u.%03u",
              static_cast<unsigned>(st.wYear), static_cast<unsigned>(st.wMonth),
              static_cast<unsigned>(st.wDay), static_cast<unsigned>(st.wHour),
              static_cast<unsigned>(st.wMinute), static_cast<unsigned>(st.wSecond),
              static_cast<unsigned>(st.wMilliseconds));
    out[capacity - 1] = '\0';
}

// Copies into a fixed-width field, padding with spaces and truncating with a visible '~' rather
// than running past the end. The columns only line up if this is exact.
void PadInto(char *out, int width, const char *text) noexcept {
    const char *const safe = (text != nullptr) ? text : "";
    int n = 0;
    while (safe[n] != '\0' && n < width) {
        out[n] = safe[n];
        ++n;
    }
    if (n == width && safe[n] != '\0') {
        out[width - 1] = '~';  // mark the truncation at the last visible column
    }
    for (int i = n; i < width; ++i) {
        out[i] = ' ';
    }
    out[width] = '\0';
}

// Formats into a caller-provided buffer, always NUL-terminating, and reports truncation rather
// than silently dropping the tail.
void FormatV(char *out, int capacity, const char *fmt, va_list args) noexcept {
    if (capacity <= 0) {
        return;
    }
    const int written = _vsnprintf(out, static_cast<size_t>(capacity) - 1, fmt, args);
    if (written < 0) {
        out[capacity - 1] = '\0';
        const int tail = capacity - 2;
        if (tail > 0) {
            out[tail] = '~';
        }
    } else {
        out[capacity - 1] = '\0';
    }
}

// Writes one fully-composed body under the lock, with CRLF and an immediate flush. The flush is
// per line on purpose: this binary has paths that spin forever and paths that jump to themselves,
// and a buffered tail would be lost exactly when the log matters most.
void EmitLocked(const char *body) noexcept {
    if (g_file == nullptr) {
        return;
    }
    std::fprintf(g_file, "%s\r\n", body);
    std::fflush(g_file);
}

// Builds "<timestamp> [LEVEL] <name padded>  <va padded>  <message>" and emits it. This is the
// single place a line is assembled, so every line in the log has one shape.
void EmitLine(Level level, const char *name, const char *va, const char *message) noexcept {
    if (!g_lock_ready) {
        return;
    }
    char stamp[40];
    char name_field[kNameWidth + 1];
    char va_field[14];
    FormatTimestamp(stamp, static_cast<int>(sizeof(stamp)));
    PadInto(name_field, kNameWidth, name);
    // An empty VA is stated as such rather than shown as 0x00000000, which reads as an address
    // and is not one. A wrong address is worse than no address because it looks like evidence.
    if (va != nullptr && va[0] != '\0') {
        PadInto(va_field, 12, va);
    } else {
        PadInto(va_field, 12, "(no record)");
    }
    char line[kLineCapacity];
    _snprintf(line, sizeof(line) - 1U, "%s [%s] %s  %s  %s", stamp, LevelTag(level), name_field,
              va_field, (message != nullptr) ? message : "");
    line[sizeof(line) - 1U] = '\0';
    EnterCriticalSection(&g_lock);
    EmitLocked(line);
    LeaveCriticalSection(&g_lock);
}

}  // namespace

void SetLevel(Level level) noexcept { g_level = level; }

Level GetLevel() noexcept { return g_level; }

void SetLevelFromEnvironment() noexcept {
    const char *const raw = std::getenv("IGI_LOG_LEVEL");
    if (raw == nullptr) {
        return;
    }
    if (_stricmp(raw, "error") == 0) {
        g_level = Level::Error;
    } else if (_stricmp(raw, "warn") == 0) {
        g_level = Level::Warn;
    } else if (_stricmp(raw, "debug") == 0) {
        g_level = Level::Debug;
    } else if (_stricmp(raw, "info") == 0) {
        g_level = Level::Info;
    }
    // Anything unrecognised keeps the current level rather than silently disabling the log.
}

void Init(const char *path) noexcept {
    if (g_file != nullptr) {
        return;
    }
    if (!g_lock_ready) {
        InitializeCriticalSection(&g_lock);
        g_lock_ready = true;
    }
    SetLevelFromEnvironment();
    const char *const safe = (path != nullptr) ? path : "igi.log";
    g_file = std::fopen(safe, "a");
    if (g_file == nullptr) {
        return;
    }
    char banner[256];
    _snprintf(banner, sizeof(banner) - 1U,
              "================ igi.log opened (level=%s) ================", LevelName(g_level));
    banner[sizeof(banner) - 1U] = '\0';
    EmitLine(Level::Info, "(devlog)", "", banner);
}

bool Enabled() noexcept { return g_file != nullptr; }

bool EnabledAt(Level level) noexcept {
    return g_file != nullptr && static_cast<int>(level) <= static_cast<int>(g_level);
}

void Write(Level level, const char *name, const char *va, const char *fmt, ...) noexcept {
    if (!EnabledAt(level)) {
        return;
    }
    char message[kLineCapacity];
    va_list args;
    va_start(args, fmt);
    FormatV(message, kLineCapacity, (fmt != nullptr) ? fmt : "", args);
    va_end(args);
    EmitLine(level, name, va, message);
}

// Entry and exit are INFO. The level is tested HERE as well as in the macro, because a function
// that writes unconditionally is a function that silently ignores IGI_LOG_LEVEL for every caller
// that reaches it by any route other than the macro.
void NativeEnter(const char *name, const char *va) noexcept {
    if (!EnabledAt(Level::Info)) {
        return;
    }
    EmitLine(Level::Info, name, va, "entered");
}

void NativeLeave(const char *name, const char *va) noexcept {
    if (!EnabledAt(Level::Info)) {
        return;
    }
    EmitLine(Level::Info, name, va, "left");
}

void NativeValue(const char *name, const char *va, const char *label, std::uint32_t value) noexcept {
    if (!EnabledAt(Level::Debug)) {
        return;
    }
    char message[kLineCapacity];
    _snprintf(message, sizeof(message) - 1U, "  %s = 0x%08X", (label != nullptr) ? label : "value",
              static_cast<unsigned>(value));
    message[sizeof(message) - 1U] = '\0';
    EmitLine(Level::Debug, name, va, message);
}

void NativeValueString(const char *name, const char *va, const char *label,
                       const char *text) noexcept {
    if (!EnabledAt(Level::Debug)) {
        return;
    }
    // Bounded copy with a visible marker. A retail string that is not NUL-terminated is a real
    // possibility in this binary, so the copy stops at a fixed length regardless.
    constexpr int kStringCap = 192;
    char bounded[kStringCap + 4];
    int n = 0;
    if (text != nullptr) {
        while (n < kStringCap && text[n] != '\0') {
            const unsigned char c = static_cast<unsigned char>(text[n]);
            bounded[n] = (c >= 0x20U && c < 0x7FU) ? static_cast<char>(c) : '.';
            ++n;
        }
        if (n == kStringCap && text[n] != '\0') {
            bounded[n++] = '~';  // truncated
        }
    }
    bounded[n] = '\0';
    char message[kLineCapacity];
    _snprintf(message, sizeof(message) - 1U, "  %s = \"%s\" (%d chars shown)",
              (label != nullptr) ? label : "string", bounded, n);
    message[sizeof(message) - 1U] = '\0';
    EmitLine(Level::Debug, name, va, message);
}

void Shutdown() noexcept {
    if (g_file == nullptr || !g_lock_ready) {
        return;
    }
    EmitLine(Level::Info, "(devlog)", "", "================ igi.log closed ================");
    EnterCriticalSection(&g_lock);
    std::fclose(g_file);
    g_file = nullptr;
    LeaveCriticalSection(&g_lock);
}

}  // namespace igi::devlog
