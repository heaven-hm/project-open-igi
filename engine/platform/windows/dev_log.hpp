// igi.log - development logging for the retail-native reconstruction.
//
// INFRASTRUCTURE, NOT A RETAIL NATIVE. This file has no retail VA and no manifest record. It is
// authorized by section 10.13 rule 3(a): logging that never changes control flow, values or
// ordering. Every macro here is do/while(0) guarded by an enabled check, evaluates no argument
// when logging is off, returns no value, and has no side effect but the append. No call site may
// make a log statement the only statement in a branch.
//
// ---------------------------------------------------------------------------------------------
// LINE FORMAT
// ---------------------------------------------------------------------------------------------
//
//   2026-10-25 17:45:12.345 [INFO ] StoreInstanceHandle        0x0048F0D0  entered
//   ^--------------------^ ^-----^ ^-----------------------^  ^--------^  ^------^
//   local wall clock        level   native name                retail VA   message
//
// Fixed-width columns so a run reads as a trace and can be grepped by level or by VA. The
// timestamp is local wall-clock with milliseconds, not elapsed-since-open, because a run has to
// be correlatable against other tools' output and against a debugger.
//
// ---------------------------------------------------------------------------------------------
// LEVELS
// ---------------------------------------------------------------------------------------------
//
//   ERROR  something failed, or a path retail treats as fatal was reached
//   WARN   retail behaviour that is surprising, preserved on purpose, or a recorded deviation
//   INFO   a native entry/exit, a stage boundary, or a value the caller supplied
//   DEBUG  detail inside a method: each load, store, branch taken, allocation, format result
//
// INFO is the default so a normal run stays readable; DEBUG is opt-in through IGI_LOG_LEVEL so
// the detailed form costs nothing unless it is asked for. ERROR and WARN are never suppressed.
//
// ---------------------------------------------------------------------------------------------
// RECORDING INPUTS AND OUTPUTS
// ---------------------------------------------------------------------------------------------
//
// There is no way to read a native's arguments or return value generically without touching the
// stack or a register the caller owns, and doing that from a logger would be a real change to
// machine state - exactly what rule 3(a) forbids. So arguments and return values are logged BY
// THE TRANSCRIPTION, at the point where the transcription already knows the value, through
// LOG_ARG / LOG_RET / LOG_VALUE. That is the only honest source for them: the logger reports what
// the native itself observed and never guesses a value it did not see.
//
//   LOG_ARG("arg2", arg2)        a parameter, at the top of the body
//   LOG_RET(result)              the value being returned, immediately before the return
//   LOG_VALUE("record+0x08", h)  a load, a store, an allocation, an intermediate result
//   LOG_STRING("name", name)     a string, printed with a visible truncation marker
//
// Values print as 0x%08X for integers and pointers. Strings are bounded and visibly marked,
// because a format string that is not NUL-terminated is a real thing in this binary.
//
// ---------------------------------------------------------------------------------------------
// USE
// ---------------------------------------------------------------------------------------------
//
//   NATIVE_LOG_ENTER(name, va)      first statement of a native body
//   NATIVE_LOG_LEAVE(name, va)      immediately before a return that carries no value
//   NATIVE_LOG_RETURN(name, va, v)  immediately before a return that carries one
//
// The `va` argument is the native's OWN retail VA, resolved from
// docs/fidelity/native-verification.json by the instrumentation pass. Where the manifest has no
// record the string is empty and the logger says so rather than printing a fabricated address -
// a wrong VA is worse than no VA, because it looks like evidence.
#ifndef IGI_PLATFORM_WINDOWS_DEV_LOG_HPP
#define IGI_PLATFORM_WINDOWS_DEV_LOG_HPP

#include <cstdint>

namespace igi::devlog {

enum class Level : int {
    Error = 0,
    Warn = 1,
    Info = 2,
    Debug = 3,
};

// Opens igi.log for appending and writes a run separator. Called once from the WinMain
// transcription before any native runs. Safe to call again; the second call is a no-op. If the
// file cannot be opened the logger stays disabled and every later call is a cheap no-op, so a
// read-only directory can never turn logging into a crash.
void Init(const char *path) noexcept;

// Applies IGI_LOG_LEVEL (error|warn|info|debug). Called by Init; exposed so a caller can change
// the level later without reopening the file. An unset or unrecognised value leaves INFO.
void SetLevelFromEnvironment() noexcept;
void SetLevel(Level level) noexcept;
Level GetLevel() noexcept;

// True once Init has opened the file. The macros test this so a disabled logger costs one
// predictable branch and no call.
bool Enabled() noexcept;
bool EnabledAt(Level level) noexcept;

// The single place a formatted line is produced. Every public macro routes here. Thread-safe.
// Never throws, never allocates beyond a fixed buffer, never touches retail state.
void Write(Level level, const char *name, const char *va, const char *fmt, ...) noexcept;

// Native entry and exit. Entry is INFO so a plain run shows the call trace; exit is INFO too,
// and deliberately carries no return value - see NATIVE_LOG_RETURN for the value-bearing form.
void NativeEnter(const char *name, const char *va) noexcept;
void NativeLeave(const char *name, const char *va) noexcept;

// Records one labelled value as part of the current method's detail. `label` is a short
// description of what the value is ("arg2", "record+0x08", "sprintf result").
void NativeValue(const char *name, const char *va, const char *label, std::uint32_t value) noexcept;
void NativeValueString(const char *name, const char *va, const char *label,
                       const char *text) noexcept;

void Shutdown() noexcept;

}  // namespace igi::devlog

#define IGI_DEVLOG_ENABLED() (::igi::devlog::Enabled())

#define LOG_ERROR(name, va, ...)                                                                   \
    do {                                                                                           \
        if (::igi::devlog::EnabledAt(::igi::devlog::Level::Error)) {                               \
            ::igi::devlog::Write(::igi::devlog::Level::Error, (name), (va), __VA_ARGS__);          \
        }                                                                                          \
    } while (0)

#define LOG_WARN(name, va, ...)                                                                    \
    do {                                                                                           \
        if (::igi::devlog::EnabledAt(::igi::devlog::Level::Warn)) {                                \
            ::igi::devlog::Write(::igi::devlog::Level::Warn, (name), (va), __VA_ARGS__);           \
        }                                                                                          \
    } while (0)

#define LOG_INFO(name, va, ...)                                                                    \
    do {                                                                                           \
        if (::igi::devlog::EnabledAt(::igi::devlog::Level::Info)) {                                \
            ::igi::devlog::Write(::igi::devlog::Level::Info, (name), (va), __VA_ARGS__);           \
        }                                                                                          \
    } while (0)

#define LOG_DEBUG(name, va, ...)                                                                   \
    do {                                                                                           \
        if (::igi::devlog::EnabledAt(::igi::devlog::Level::Debug)) {                               \
            ::igi::devlog::Write(::igi::devlog::Level::Debug, (name), (va), __VA_ARGS__);          \
        }                                                                                          \
    } while (0)

#define NATIVE_LOG_ENTER(name, va)                                                                 \
    do {                                                                                           \
        if (::igi::devlog::EnabledAt(::igi::devlog::Level::Info)) {                                \
            ::igi::devlog::NativeEnter((name), (va));                                              \
        }                                                                                          \
    } while (0)

#define NATIVE_LOG_LEAVE(name, va)                                                                 \
    do {                                                                                           \
        if (::igi::devlog::EnabledAt(::igi::devlog::Level::Info)) {                                \
            ::igi::devlog::NativeLeave((name), (va));                                              \
        }                                                                                          \
    } while (0)

#define NATIVE_LOG_RETURN(name, va, value)                                                         \
    do {                                                                                           \
        if (IGI_DEVLOG_ENABLED()) {                                                                \
            ::igi::devlog::NativeValue((name), (va), "return",                                     \
                                       static_cast<std::uint32_t>(value));                         \
            ::igi::devlog::NativeLeave((name), (va));                                              \
        }                                                                                          \
    } while (0)

// Argument and intermediate-value recording, called from inside a transcription at the point
// where the value is already known. These are the only source of inputs and outputs in this log.
#define LOG_ARG(name, va, label, value)                                                            \
    do {                                                                                           \
        if (IGI_DEVLOG_ENABLED()) {                                                                \
            ::igi::devlog::NativeValue((name), (va), (label),                                      \
                                       static_cast<std::uint32_t>(value));                         \
        }                                                                                          \
    } while (0)

#define LOG_VALUE(name, va, label, value) LOG_ARG(name, va, label, value)

#define LOG_RET(name, va, value) LOG_ARG(name, va, "return", value)

#define LOG_STRING(name, va, label, text)                                                          \
    do {                                                                                           \
        if (IGI_DEVLOG_ENABLED()) {                                                                \
            ::igi::devlog::NativeValueString((name), (va), (label), (text));                       \
        }                                                                                          \
    } while (0)

#endif  // IGI_PLATFORM_WINDOWS_DEV_LOG_HPP
