// Infrastructure: development logging. This file is NOT a retail native and has no retail
// VA. It exists under the permitted addition in section 10.13 rule 3(a) - "logging that
// never changes control flow, values, or ordering, clearly marked as infrastructure" - and
// section 10.14 rule 9, which places logging outside native flows.
//
// It writes igi.log beside the executable. Nothing here may read or write a retail global,
// call a retail routine, or alter any value a native computes. Every entry is a statement
// whose only effect is an append to the log.
#ifndef IGI_PLATFORM_WINDOWS_DEV_LOG_HPP
#define IGI_PLATFORM_WINDOWS_DEV_LOG_HPP

namespace igi::devlog {

// Opens igi.log for appending. Called once from the WinMain transcription before any native
// runs. Safe to call again; the second call is a no-op. If the file cannot be opened the
// logger stays disabled and every later call is a cheap no-op, so a read-only directory can
// never turn logging into a crash.
void Init(const char *path) noexcept;

// True once Init has opened the file. Used by the macro below so that the common case of a
// disabled logger costs one predictable branch and no call.
bool Enabled() noexcept;

// Appends one line: "[<milliseconds since init>] <text>\r\n". Thread-safe. Never throws,
// never allocates beyond a fixed buffer, never touches retail state.
void Write(const char *text) noexcept;

// Appends one line built from a printf-style format. Truncates rather than overflows; a
// format that does not fit is logged in truncated form with a trailing marker, because a
// dropped log line is worse than a short one.
void Writef(const char *fmt, ...) noexcept;

// Appends the native-entry line: "NATIVE ENTER <name> @ <va>". Kept separate from Writef so
// the entry records have one shape and can be grepped and counted.
void NativeEnter(const char *name, const char *va) noexcept;

// Appends the native-exit line. Never carries a return value: capturing EAX here would mean
// reading a register the compiler may already have reused, and a wrong return value in the
// log is worse than no return value.
void NativeLeave(const char *name) noexcept;

// Closes the file. Called at shutdown. Safe to call when never initialised.
void Shutdown() noexcept;

}  // namespace igi::devlog

// NATIVE_LOG_ENTER - logs that a retail native was entered.
//
// Contract, and the reason this is a statement and not an expression: it evaluates no
// argument, returns no value, and produces no side effect other than the log append. It is
// written as `do { ... } while (0)` so it is safe in an unbraced if, and it is guarded by
// Enabled() so a disabled logger cannot change the cost of a hot path enough to matter.
//
// It must never be placed where retail has no statement, and it must never be the only
// statement in a branch, because adding it must not change which branch retail takes.
//
// The `va` argument is the function's OWN retail VA where the function name carries it as an
// address suffix, which is the case for every address-suffixed placeholder. Where the name
// carries no address - a behavior-derived name in a file with several functions - the value
// is passed with a LEADING `~`, meaning "this is the file's provenance VA, not this function's
// own". The logger prints it as "~0x..." so a reader cannot mistake it for the function's
// address. The first version of this instrumentation stamped the file's first provenance VA
// on every function in the file, which was right for single-function files and confidently
// wrong for the rest; a runtime log showed six different natives all reporting 0x0048F120.
#define NATIVE_LOG_ENTER(name, va)                       \
    do {                                                 \
        if (::igi::devlog::Enabled()) {                  \
            ::igi::devlog::NativeEnter((name), (va));    \
        }                                                \
    } while (0)

// NATIVE_LOG_LEAVE - logs that a retail native is returning.
//
// Placed immediately before a retail `ret`, never before an early return that retail does
// not have. On a native with several retail return paths, place one before each.
#define NATIVE_LOG_LEAVE(name)                           \
    do {                                                 \
        if (::igi::devlog::Enabled()) {                  \
            ::igi::devlog::NativeLeave((name));          \
        }                                                \
    } while (0)

// DEVLOG - a free-form infrastructure note. Use for port-level facts that are not a native
// boundary (a file that failed to open, a level path that resolved). Never use it to record
// a value a native computed as if retail had logged it.
#define DEVLOG(...)                                      \
    do {                                                 \
        if (::igi::devlog::Enabled()) {                  \
            ::igi::devlog::Writef(__VA_ARGS__);          \
        }                                                \
    } while (0)

#endif  // IGI_PLATFORM_WINDOWS_DEV_LOG_HPP
