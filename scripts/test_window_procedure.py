#!/usr/bin/env python3
"""Host regression checks for the retail WindowProcedure implementation.

The host test links the real engine source against explicit Win32 API spies.
It tests dispatch intent and state changes, not Windows or retail execution.
An optional MinGW x86 object check verifies stdcall decoration, stack cleanup,
the call into the transcribed 0x00498460 setter, and that no 0x004010B0
placeholder symbol survives.
"""

from __future__ import annotations

import argparse
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SOURCE = ROOT / "engine/src/retail/core/native_0048f970.cpp"

WINDOWS_SHIM = r"""
#pragma once
#include <cstdint>
using BOOL = int;
using UINT = unsigned int;
using HWND = void*;
using WPARAM = std::uintptr_t;
using LPARAM = std::intptr_t;
using LRESULT = std::intptr_t;
constexpr BOOL TRUE = 1;
extern "C" int ShowCursor(BOOL);
extern "C" void PostQuitMessage(int);
extern "C" BOOL PostMessageA(HWND, UINT, WPARAM, LPARAM);
extern "C" LRESULT DefWindowProcA(HWND, UINT, WPARAM, LPARAM);
"""

HARNESS = r"""
#include "igi/retail/core/native_0048f970.hpp"
#include "igi/retail/core/fpu_state.hpp"
#include <windows.h>
#include <cfenv>
#include <cstdint>
#include <cstddef>
#include <cstdio>

#if UINTPTR_MAX == 0xffffffffU
static_assert(sizeof(igi::retail::core::WindowHandlerEntry) == 12,
              "retail x86 handler node must be 12 bytes");
static_assert(offsetof(igi::retail::core::WindowHandlerEntry, handler) == 8,
              "retail x86 handler must remain at offset +8");
#endif

struct Win32SpyState
{
    int show_cursor_calls;
    BOOL last_show_cursor;
    int post_quit_calls;
    int last_quit_code;
    int post_message_calls;
    HWND posted_window;
    UINT posted_message;
    WPARAM posted_wparam;
    LPARAM posted_lparam;
    int def_window_proc_calls;
    HWND default_window;
    UINT default_message;
    WPARAM default_wparam;
    LPARAM default_lparam;
    int call_order[2];
    int call_order_count;
};

Win32SpyState win32_spy{};
constexpr LRESULT DEF_WINDOW_PROC_SENTINEL = 0x13579;

extern "C" {
void* g_window_handle = nullptr;
extern std::uint8_t Loop_StatusByte;
extern std::uint32_t DebugKeyFlag;

int ShowCursor(BOOL show)
{
    ++win32_spy.show_cursor_calls;
    win32_spy.last_show_cursor = show;
    win32_spy.call_order[win32_spy.call_order_count++] = 1;
    return 1;
}

void PostQuitMessage(int code)
{
    ++win32_spy.post_quit_calls;
    win32_spy.last_quit_code = code;
    win32_spy.call_order[win32_spy.call_order_count++] = 2;
}

BOOL PostMessageA(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    ++win32_spy.post_message_calls;
    win32_spy.posted_window = window;
    win32_spy.posted_message = message;
    win32_spy.posted_wparam = wparam;
    win32_spy.posted_lparam = lparam;
    return TRUE;
}

LRESULT DefWindowProcA(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    ++win32_spy.def_window_proc_calls;
    win32_spy.default_window = window;
    win32_spy.default_message = message;
    win32_spy.default_wparam = wparam;
    win32_spy.default_lparam = lparam;
    return DEF_WINDOW_PROC_SENTINEL;
}
}

#if defined(_MSC_VER) && defined(_M_IX86)
#define TEST_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define TEST_CDECL __attribute__((cdecl))
#else
#define TEST_CDECL
#endif

namespace {
int failures = 0;
int checks = 0;
int callback_order[4]{};
int callback_count = 0;
std::intptr_t callback2_result = 1;
void* callback_window = nullptr;
std::uint32_t callback_message = 0;
std::uintptr_t callback_wparam = 0;
std::intptr_t callback_lparam = 0;

std::intptr_t TEST_CDECL callback_one(void* window, std::uint32_t message, std::uintptr_t wparam,
                           std::intptr_t lparam)
{
    callback_order[callback_count++] = 1;
    callback_window = window;
    callback_message = message;
    callback_wparam = wparam;
    callback_lparam = lparam;
    return 1;
}

std::intptr_t TEST_CDECL callback_two(void* window, std::uint32_t message, std::uintptr_t wparam,
                           std::intptr_t lparam)
{
    callback_order[callback_count++] = 2;
    callback_window = window;
    callback_message = message;
    callback_wparam = wparam;
    callback_lparam = lparam;
    return callback2_result;
}

void reset_callbacks()
{
    callback_order[0] = callback_order[1] = callback_order[2] = callback_order[3] = 0;
    callback_count = 0;
    callback2_result = 1;
    callback_window = nullptr;
    callback_message = 0;
    callback_wparam = 0;
    callback_lparam = 0;
}

void reset_spy()
{
    win32_spy = {};
}

void check(const char* name, bool ok)
{
    ++checks;
    std::printf("%s %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) ++failures;
}
}

int main()
{
    constexpr UINT WM_DESTROY_RETAIL = 0x0002U;
    constexpr UINT WM_ACTIVATEAPP_RETAIL = 0x001CU;
    constexpr UINT WM_SYSKEYDOWN_RETAIL = 0x0104U;
    constexpr UINT WM_SYSKEYUP_RETAIL = 0x0105U;
    constexpr UINT WM_USER_RETAIL = 0x0400U;
    constexpr std::uintptr_t VK_MENU_RETAIL = 0x12U;
    constexpr std::uintptr_t VK_F4_RETAIL = 0x73U;

    Loop_StatusByte = 0;
    DebugKeyFlag = 0;
    reset_spy();
    check("SYSKEYDOWN VK_MENU sets flag, returns zero, and bypasses default",
          WindowProcedure(nullptr, WM_SYSKEYDOWN_RETAIL, VK_MENU_RETAIL, 0) == 0 &&
          DebugKeyFlag == 1U && win32_spy.def_window_proc_calls == 0);

    reset_spy();
    check("repeated SYSKEYDOWN VK_MENU preserves flag and bypasses default",
          WindowProcedure(nullptr, WM_SYSKEYDOWN_RETAIL, VK_MENU_RETAIL, 0) == 0 &&
          DebugKeyFlag == 1U && win32_spy.def_window_proc_calls == 0);

    reset_spy();
    check("SYSKEYUP VK_MENU clears flag and bypasses default",
          WindowProcedure(nullptr, WM_SYSKEYUP_RETAIL, VK_MENU_RETAIL, 0) == 0 &&
          DebugKeyFlag == 0U && win32_spy.def_window_proc_calls == 0);

    DebugKeyFlag = 0xA5U;
    reset_spy();
    check("unmatched SYSKEYDOWN returns zero without changing flag or defaulting",
          WindowProcedure(nullptr, WM_SYSKEYDOWN_RETAIL, 0x41U, 0) == 0 &&
          DebugKeyFlag == 0xA5U && win32_spy.def_window_proc_calls == 0);

    DebugKeyFlag = 0x5AU;
    reset_spy();
    check("unmatched SYSKEYUP returns zero without changing flag or defaulting",
          WindowProcedure(nullptr, WM_SYSKEYUP_RETAIL, 0x41U, 0) == 0 &&
          DebugKeyFlag == 0x5AU && win32_spy.def_window_proc_calls == 0);

    g_window_handle = reinterpret_cast<void*>(0x12345678U);
    DebugKeyFlag = 1U;
    reset_spy();
    check("SYSKEYDOWN VK_F4 posts WM_DESTROY to retail handle and returns zero",
          WindowProcedure(reinterpret_cast<void*>(0x87654321U), WM_SYSKEYDOWN_RETAIL,
                          VK_F4_RETAIL, 0x55) == 0 &&
          DebugKeyFlag == 1U && win32_spy.post_message_calls == 1 &&
          win32_spy.posted_window == g_window_handle &&
          win32_spy.posted_message == WM_DESTROY_RETAIL &&
          win32_spy.posted_wparam == 0 && win32_spy.posted_lparam == 0 &&
          win32_spy.def_window_proc_calls == 0);

    DebugKeyFlag = 0U;
    reset_spy();
    check("SYSKEYDOWN VK_F4 without debug flag posts nothing and returns zero",
          WindowProcedure(nullptr, WM_SYSKEYDOWN_RETAIL, VK_F4_RETAIL, 0) == 0 &&
          DebugKeyFlag == 0U && win32_spy.post_message_calls == 0 &&
          win32_spy.def_window_proc_calls == 0);

    // The retail chain node is 12 bytes: next at +0x00, previous at +0x04,
    // handler at +0x08. These harness nodes only drive next/handler; the
    // port's walk does not read previous. The retail chain is terminated by a
    // successor cell; that terminal cell is not itself invoked. The first node
    // is called before its successor.
    igi::retail::core::WindowHandlerEntry chain_head{};
    igi::retail::core::WindowHandlerEntry chain_next{};
    igi::retail::core::WindowHandlerEntry chain_tail{};
    chain_head.next = &chain_next;
    chain_head.handler = callback_one;
    chain_next.next = &chain_tail;
    chain_next.handler = callback_two;
    chain_tail.next = nullptr;
    chain_tail.handler = nullptr;
    WindowHandlerChain = &chain_head;
    Loop_StatusByte = 1U;
    const void* const chain_window = reinterpret_cast<void*>(0x23456789U);
    constexpr UINT chain_message = WM_USER_RETAIL + 1U;
    constexpr WPARAM chain_wparam = 0xBEEFU;
    constexpr LPARAM chain_lparam = 0x7654;
    reset_callbacks();
    reset_spy();
    const bool chain_fallthrough =
        WindowProcedure(const_cast<void*>(chain_window), chain_message, chain_wparam,
                        chain_lparam) == DEF_WINDOW_PROC_SENTINEL &&
        callback_count == 2 && callback_order[0] == 1 && callback_order[1] == 2 &&
        callback_window == chain_window && callback_message == chain_message &&
        callback_wparam == chain_wparam && callback_lparam == chain_lparam &&
        win32_spy.def_window_proc_calls == 1;
    check("handler chain preserves callback order, arguments, and nonzero fall-through",
          chain_fallthrough);

    reset_callbacks();
    callback2_result = 0;
    reset_spy();
    check("zero callback result intercepts and stops the handler chain",
          WindowProcedure(const_cast<void*>(chain_window), chain_message, chain_wparam,
                          chain_lparam) == 0 &&
          callback_count == 2 && callback_order[0] == 1 && callback_order[1] == 2 &&
          win32_spy.def_window_proc_calls == 0);
    // From here on the chain head is null. The WM_DESTROY checks below set
    // Loop_StatusByte again with a null chain; retail loads 0x005C8DF0 and
    // dereferences it at 0x0048F98E with no null check, so it would fault in
    // this state. Those checks therefore exercise only the port's head-null
    // safety guard, not retail behavior.
    WindowHandlerChain = nullptr;
    Loop_StatusByte = 0U;

    // Message 0x001C is Win32 WM_ACTIVATEAPP (winuser.h; WM_ACTIVATE is 0x0006);
    // the _RETAIL suffix avoids the windows.h macro of the same name. Retail
    // handles WM_ACTIVATEAPP by calling the x87 chop-rounding setter at
    // 0x00498460 and returning zero. On x86 the retail setter is checked
    // through the x87 control word; other hosts use the source's C
    // floating-point environment fallback.
#if defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
    unsigned short original_control_word;
    __asm__ __volatile__("fnstcw %0" : "=m"(original_control_word) : : "memory");
    unsigned short test_control_word = 0x037F;
    __asm__ __volatile__("fldcw %0" : : "m"(test_control_word) : "memory");
    reset_spy();
    const std::intptr_t chop_result = WindowProcedure(nullptr, WM_ACTIVATEAPP_RETAIL, 0, 0);
    unsigned short observed_control_word;
    __asm__ __volatile__("fnstcw %0" : "=m"(observed_control_word) : : "memory");
    const bool chop_rounding =
        chop_result == 0 && igi::retail::core::g_6e5bc4 == 0x0F7F &&
        observed_control_word == 0x0F7F && win32_spy.def_window_proc_calls == 0;
    check("WM_ACTIVATEAPP (0x001C) invokes the chop-rounding setter, returns zero, and bypasses default",
          chop_rounding);
    __asm__ __volatile__("fldcw %0" : : "m"(original_control_word) : "memory");
#else
    std::fesetround(FE_TONEAREST);
    reset_spy();
    const bool chop_rounding =
        WindowProcedure(nullptr, WM_ACTIVATEAPP_RETAIL, 0, 0) == 0 &&
        std::fegetround() == FE_TOWARDZERO &&
        win32_spy.def_window_proc_calls == 0;
    check("WM_ACTIVATEAPP (0x001C) invokes the chop-rounding setter, returns zero, and bypasses default",
          chop_rounding);
    std::fesetround(FE_TONEAREST);
#endif

    // Retail WM_DESTROY order is: clear loop byte, ShowCursor(TRUE), call
    // 0x004010B0, PostQuitMessage(0). The 0x004010B0 step is unreconstructed
    // and deliberately omitted from the candidate, so these checks assert the
    // documented ordering around that gap, not a placeholder call. The chain
    // head is null here; retail would fault on it, so only the port's
    // head-null guard is exercised.
    Loop_StatusByte = 1U;
    reset_spy();
    check("WM_DESTROY clears loop status and calls cursor/quit APIs",
          WindowProcedure(nullptr, WM_DESTROY_RETAIL, 0, 0) == 0 &&
          Loop_StatusByte == 0U && win32_spy.show_cursor_calls == 1 &&
          win32_spy.last_show_cursor == TRUE && win32_spy.post_quit_calls == 1 &&
          win32_spy.last_quit_code == 0 && win32_spy.def_window_proc_calls == 0);

    Loop_StatusByte = 1U;
    reset_spy();
    check("WM_DESTROY runs cursor then quit in retail order (0x004010B0 omitted)",
          WindowProcedure(nullptr, WM_DESTROY_RETAIL, 0, 0) == 0 &&
          Loop_StatusByte == 0U && win32_spy.show_cursor_calls == 1 &&
          win32_spy.post_quit_calls == 1 && win32_spy.last_show_cursor == TRUE &&
          win32_spy.last_quit_code == 0 && win32_spy.call_order_count == 2 &&
          win32_spy.call_order[0] == 1 && win32_spy.call_order[1] == 2 &&
          win32_spy.def_window_proc_calls == 0);

    HWND const default_window = reinterpret_cast<HWND>(static_cast<std::uintptr_t>(0x87654321U));
    constexpr WPARAM default_wparam = 0xABU;
    constexpr LPARAM default_lparam = 0x1234;
    DebugKeyFlag = 0x33U;
    reset_spy();
    check("unhandled message calls DefWindowProcA and preserves its result/arguments",
          WindowProcedure(default_window, WM_USER_RETAIL, default_wparam,
                          default_lparam) == DEF_WINDOW_PROC_SENTINEL &&
          win32_spy.def_window_proc_calls == 1 &&
          win32_spy.default_window == default_window &&
          win32_spy.default_message == WM_USER_RETAIL &&
          win32_spy.default_wparam == default_wparam &&
          win32_spy.default_lparam == default_lparam && DebugKeyFlag == 0x33U);

    std::printf("CHECKS=%d FAILURES=%d\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
"""


def run(command: list[str], *, cwd: Path = ROOT) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, cwd=cwd, text=True, capture_output=True, check=False)


def host_regression(source: Path, work: Path, compiler: str) -> bool:
    shim_dir = work / "win32-shim"
    shim_dir.mkdir()
    (shim_dir / "windows.h").write_text(WINDOWS_SHIM, encoding="utf-8")
    harness = work / "window_procedure_host_test.cpp"
    executable = work / "window_procedure_host_test"
    harness.write_text(HARNESS, encoding="utf-8")
    compile_result = run([
        compiler,
        "-std=gnu++20",
        "-D_WIN32",
        "-I",
        str(shim_dir),
        "-I",
        str(ROOT / "engine/include"),
        str(harness),
        str(source),
        str(ROOT / "engine/src/retail/core/fpu_state.cpp"),
        "-o",
        str(executable),
    ])
    if compile_result.stdout:
        print(compile_result.stdout, end="")
    if compile_result.stderr:
        print(compile_result.stderr, end="", file=sys.stderr)
    if compile_result.returncode != 0:
        print(f"HOST_COMPILE_EXIT={compile_result.returncode}")
        return False

    test_result = run([str(executable)])
    if test_result.stdout:
        print(test_result.stdout, end="")
    if test_result.stderr:
        print(test_result.stderr, end="", file=sys.stderr)
    print(f"HOST_TEST_EXIT={test_result.returncode}")
    return test_result.returncode == 0


def mingw_abi(source: Path, work: Path) -> bool | None:
    compiler = shutil.which("i686-w64-mingw32-g++")
    nm = shutil.which("i686-w64-mingw32-nm")
    objdump = shutil.which("i686-w64-mingw32-objdump")
    if not compiler or not nm or not objdump:
        print("MINGW_ABI=SKIP (i686-w64-mingw32-g++, nm, or objdump unavailable)")
        return None

    abi_probe_source = work / "window_handler_abi_probe.cpp"
    abi_probe_source.write_text(r'''#include "igi/retail/core/native_0048f970.hpp"
#include <cstddef>
#include <type_traits>
using ExpectedCallback = std::intptr_t (__attribute__((cdecl)) *)(
    void*, std::uint32_t, std::uintptr_t, std::intptr_t);
static_assert(sizeof(igi::retail::core::WindowHandlerEntry) == 12);
static_assert(offsetof(igi::retail::core::WindowHandlerEntry, handler) == 8);
static_assert(std::is_same<igi::retail::core::WindowHandlerCallback, ExpectedCallback>::value);
extern "C" std::intptr_t __attribute__((cdecl)) explicit_cdecl_callback(
    void*, std::uint32_t, std::uintptr_t, std::intptr_t) { return 1; }
void invoke_callback(igi::retail::core::WindowHandlerEntry* entry) {
    (void)entry->handler(nullptr, 0, 0, 0);
}
''', encoding="utf-8")
    abi_probe = work / "window_handler_abi_probe.o"
    probe_result = run([
        compiler,
        "-std=gnu++20",
        "-mrtd",
        "-I",
        str(ROOT / "engine/include"),
        "-c",
        str(abi_probe_source),
        "-o",
        str(abi_probe),
    ])
    if probe_result.stderr:
        print(probe_result.stderr, end="", file=sys.stderr)
    if probe_result.returncode != 0:
        print(f"MINGW_CALLBACK_ABI_COMPILE_EXIT={probe_result.returncode}")
        return False
    probe_symbols = run([nm, "-g", "--defined-only", str(abi_probe)])
    if probe_symbols.returncode != 0:
        print(probe_symbols.stderr, end="", file=sys.stderr)
        return False
    cdecl_encoded = re.search(r"\b_explicit_cdecl_callback\b", probe_symbols.stdout) is not None
    print(f"MINGW_CALLBACK_CDECL_AND_LAYOUT={'PASS' if cdecl_encoded else 'FAIL'}")
    if not cdecl_encoded:
        return False

    objects: dict[str, Path] = {}
    for name, unit in (
        ("window_procedure_x86.o", source),
        ("fpu_state_x86.o", ROOT / "engine/src/retail/core/fpu_state.cpp"),
    ):
        obj = work / name
        compile_result = run([
            compiler,
            "-std=gnu++20",
            "-O3",
            "-DNDEBUG",
            "-I",
            str(ROOT / "engine/include"),
            "-c",
            str(unit),
            "-o",
            str(obj),
        ])
        if compile_result.stdout:
            print(compile_result.stdout, end="")
        if compile_result.stderr:
            print(compile_result.stderr, end="", file=sys.stderr)
        if compile_result.returncode != 0:
            print(f"MINGW_COMPILE_EXIT={compile_result.returncode} ({name})")
            return False
        objects[name] = obj

    obj = objects["window_procedure_x86.o"]
    fpu_obj = objects["fpu_state_x86.o"]
    symbols = run([nm, "-g", "--defined-only", str(obj)])
    fpu_symbols = run([nm, "-g", "--defined-only", str(fpu_obj)])
    disassembly = run([objdump, "-dr", str(obj)])
    fpu_disassembly = run([objdump, "-dr", str(fpu_obj)])
    if symbols.stdout:
        print("MINGW_SYMBOL_OUTPUT:")
        print(symbols.stdout, end="")
    if symbols.stderr:
        print(symbols.stderr, end="", file=sys.stderr)
    if disassembly.returncode != 0:
        print(disassembly.stderr, end="", file=sys.stderr)
        print(f"MINGW_OBJDUMP_EXIT={disassembly.returncode}")
        return False
    if fpu_disassembly.returncode != 0:
        print(fpu_disassembly.stderr, end="", file=sys.stderr)
        print(f"MINGW_FPU_OBJDUMP_EXIT={fpu_disassembly.returncode}")
        return False

    decorated = re.search(r"\b_WindowProcedure@16\b", symbols.stdout) is not None
    body_match = re.search(
        r"<_WindowProcedure@16>:(.*?)(?=\n[0-9a-fA-F]+ <|\Z)",
        disassembly.stdout,
        flags=re.S,
    )
    body = body_match.group(1) if body_match else ""
    callee_pop_16 = re.search(r"\bret[l]?\s+\$0x10\b", body, flags=re.I) is not None
    chop_setter_call = (
        re.search(r"\bDISP32\s+\S*FP_InitExtendedPrecision", body) is not None
    )
    # The retail chop-rounding setter is transcribed in fpu_state.cpp; check the
    # exact instruction sequence there instead of a second inline copy.
    fpu_body_match = re.search(
        r"<__ZN3igi6retail4core24FP_InitExtendedPrecisionEv>:(.*?)(?=\n[0-9a-fA-F]+ <|\Z)",
        fpu_disassembly.stdout,
        flags=re.S,
    )
    fpu_body = fpu_body_match.group(1) if fpu_body_match else ""
    chop_sequence = re.search(
        r"\bf(st|c)cw\b.*?\borw\s+\$?0xc00\b.*?\bfldcw\b",
        fpu_body,
        flags=re.S | re.I,
    ) is not None
    fpu_global_defined = (
        re.search(r"\b__ZN3igi6retail4core8g_6e5bc4E\b", fpu_symbols.stdout) is not None
    )
    placeholder_absent = (
        "RetailWindowCloseCleanup" not in symbols.stdout
        and "RetailWindowCloseCleanup" not in fpu_symbols.stdout
        and "RetailWindowCloseCleanup" not in disassembly.stdout
        and "RetailWindowCloseCleanup" not in fpu_disassembly.stdout
    )
    print(f"MINGW_STDCALL_SYMBOL={'PASS' if decorated else 'FAIL'}")
    print(f"MINGW_CALLEE_RET_16={'PASS' if callee_pop_16 else 'FAIL'}")
    print(f"MINGW_CHOP_SETTER_CALL={'PASS' if chop_setter_call else 'FAIL'}")
    print(f"MINGW_CHOP_ROUNDING_SEQUENCE={'PASS' if chop_sequence else 'FAIL'}")
    print(f"MINGW_FPU_GLOBAL={'PASS' if fpu_global_defined else 'FAIL'}")
    print(f"MINGW_NO_CLOSE_PLACEHOLDER={'PASS' if placeholder_absent else 'FAIL'}")
    return (decorated and callee_pop_16 and chop_setter_call and chop_sequence
            and fpu_global_defined and placeholder_absent)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=DEFAULT_SOURCE,
                        help="implementation source (defaults to the current real engine source)")
    parser.add_argument("--skip-mingw", action="store_true",
                        help="skip the optional x86 MinGW ABI object check")
    args = parser.parse_args()
    source = args.source.resolve()
    if not source.is_file():
        parser.error(f"implementation source does not exist: {source}")
    compiler = shutil.which("c++") or shutil.which("clang++")
    if compiler is None:
        print("BLOCKED: no host C++ compiler found", file=sys.stderr)
        return 2

    print(f"IMPLEMENTATION_SOURCE={source}")
    print("LIMITATION=Win32 APIs are explicit test doubles; this does not exercise Windows or prove retail runtime parity.")
    outcomes: list[bool] = []
    with tempfile.TemporaryDirectory(prefix="igi-window-procedure-") as directory:
        work = Path(directory)
        outcomes.append(host_regression(source, work, compiler))
        if not args.skip_mingw:
            abi_result = mingw_abi(source, work)
            if abi_result is not None:
                outcomes.append(abi_result)
    passed = all(outcomes)
    print(f"RESULT={'PASS' if passed else 'FAIL'}")
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
