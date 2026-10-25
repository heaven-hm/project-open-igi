// Infrastructure: development logging implementation. NOT a retail native; no retail VA.
// Permitted by section 10.13 rule 3(a). See dev_log.hpp for the contract.
//
// Constraints this file is written to, all of which matter for a matching decompilation:
//   - It must never be reachable from a native's computation. Nothing here reads or writes a
//     retail global or calls a retail routine.
//   - It must never throw, never abort, and never allocate on a path that can fail. A logger
//     that crashes the game is worse than no logger.
//   - It must be thread-safe by a plain critical section: retail's own task system runs
//     handlers from more than one thread, and interleaved half-lines would make the log
//     unreadable exactly when it is needed most.
//   - Every string operation is bounded. Truncation is logged as truncation.
#include "dev_log.hpp"

#include <windows.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace igi::devlog {
namespace {

// One FILE* guarded by one critical section. A single handle keeps ordering honest: two
// threads that both log get a total order rather than two interleaved streams.
FILE *g_file = nullptr;
CRITICAL_SECTION g_lock;
bool g_lock_ready = false;
DWORD g_start_tick = 0;

// 4 KiB is enough for every line this project emits and small enough to sit on the stack of
// a retail native without moving its frame anywhere near a page boundary.
constexpr int kLineCapacity = 4096;

// Formats into a caller-provided buffer, always NUL-terminating, and reports truncation
// rather than silently dropping the tail.
void FormatV(char *out, int capacity, const char *fmt, va_list args) noexcept {
    if (capacity <= 0) {
        return;
    }
    const int written = _vsnprintf(out, static_cast<size_t>(capacity) - 1, fmt, args);
    if (written < 0) {
        out[capacity - 1] = '\0';
        // _vsnprintf returns -1 on overflow; mark the truncation at the end of what fits.
        const int tail = capacity - 2;
        if (tail > 0) {
            out[tail] = '~';
        }
    } else {
        out[capacity - 1] = '\0';
    }
}

// Writes one already-formatted body with the elapsed-milliseconds prefix and CRLF, under the
// lock. Separated from the public entry points so every line has one shape.
void EmitLocked(const char *body) noexcept {
    if (g_file == nullptr) {
        return;
    }
    const DWORD elapsed = GetTickCount() - g_start_tick;
    std::fprintf(g_file, "[%lu] %s\r\n", static_cast<unsigned long>(elapsed), body);
    // Flushed every line on purpose: the interesting failures in this project are fatal
    // self-jumps and load failures that terminate the process, and a buffered tail would be
    // lost exactly then.
    std::fflush(g_file);
}

// Formats a variadic body and emits it. This is the one place a printf-style argument list
// becomes a va_list: the first version passed varargs straight to FormatV, which takes a
// va_list, and the compiler rejected it. Keeping va_start here means every public entry point
// below passes plain arguments.
void Emitf(const char *fmt, ...) noexcept {
    if (g_file == nullptr || fmt == nullptr) {
        return;
    }
    char body[kLineCapacity];
    va_list args;
    va_start(args, fmt);
    FormatV(body, kLineCapacity, fmt, args);
    va_end(args);
    EnterCriticalSection(&g_lock);
    EmitLocked(body);
    LeaveCriticalSection(&g_lock);
}

}  // namespace

void Init(const char *path) noexcept {
    if (g_file != nullptr) {
        return;
    }
    if (!g_lock_ready) {
        InitializeCriticalSection(&g_lock);
        g_lock_ready = true;
    }
    const char *target = (path != nullptr) ? path : "igi.log";
    // "a" rather than "w": several runs in one session append, and the first line of each run
    // is a separator so runs can be told apart without losing earlier evidence.
    g_file = std::fopen(target, "a");
    if (g_file == nullptr) {
        return;
    }
    g_start_tick = GetTickCount();
    EnterCriticalSection(&g_lock);
    EmitLocked("================ igi.log opened ================");
    LeaveCriticalSection(&g_lock);
}

bool Enabled() noexcept { return g_file != nullptr; }

void Write(const char *text) noexcept {
    if (g_file == nullptr || text == nullptr) {
        return;
    }
    EnterCriticalSection(&g_lock);
    EmitLocked(text);
    LeaveCriticalSection(&g_lock);
}

void Writef(const char *fmt, ...) noexcept {
    if (g_file == nullptr || fmt == nullptr) {
        return;
    }
    char body[kLineCapacity];
    va_list args;
    va_start(args, fmt);
    FormatV(body, kLineCapacity, fmt, args);
    va_end(args);
    EnterCriticalSection(&g_lock);
    EmitLocked(body);
    LeaveCriticalSection(&g_lock);
}
void NativeEnter(const char *name, const char *va) noexcept {
    Emitf("NATIVE ENTER %s @ %s", (name != nullptr) ? name : "(unnamed)", (va != nullptr) ? va : "0x00000000");
}
void NativeLeave(const char *name) noexcept {
    Emitf("NATIVE LEAVE %s", (name != nullptr) ? name : "(unnamed)");
}
void Shutdown() noexcept {
    if (g_file == nullptr) {
        return;
    }
    EnterCriticalSection(&g_lock);
    EmitLocked("================ igi.log closed ================");
    std::fclose(g_file);
    g_file = nullptr;
    LeaveCriticalSection(&g_lock);
    if (g_lock_ready) {
        DeleteCriticalSection(&g_lock);
        g_lock_ready = false;
    }
}

}  // namespace igi::devlog
