# WindowProcedure Review Fixes Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make the `0x0048F970` callback ABI explicit and test the retail x86 node layout and chain walk while retaining provisional status.

**Architecture:** Keep `WindowHandlerEntry`’s existing fields and offsets, replacing only the handler pointer type with a named platform-aware cdecl callback typedef. Extend the temporary host test harness to exercise chain semantics and compile-time x86 layout assertions; document the two remaining retail call gaps.

**Tech Stack:** C++17, Python 3 standard library test runner, host C++ compiler, MinGW i686 cross-compiler.

## Global Constraints

- Retail `igi.exe` is the behavior source of truth.
- Never promote status without required paired retail/candidate runtime evidence.
- Do not alter manifest/evidence/status or deploy binaries in this change.
- Host Win32 shims are test doubles and do not establish retail runtime parity.
- Preserve all pre-existing dirty files and index state.

---

### Task 1: Enforce and test the callback ABI and chain walk

**Files:**
- Modify: `engine/include/igi/retail/core/native_0048f970.hpp`
- Modify: `scripts/test_window_procedure.py`
- Modify: `engine/include/igi/retail/core/native_0048f970.hpp` comments to state the `WM_ACTIVATE` call at `0x00498460` remains unimplemented.

**Interfaces:**
- Produces: `igi::retail::core::WindowHandlerCallback`, a function-pointer alias explicitly cdecl on 32-bit Windows toolchains.
- Preserves: `WindowHandlerEntry` size 12 bytes on x86, `handler` offset 8, and `WindowProcedure` stdcall ABI.

- [ ] **Step 1: Add failing chain and layout checks**
  - Add a 32-bit-only `static_assert(sizeof(WindowHandlerEntry) == 12)` and `static_assert(offsetof(WindowHandlerEntry, handler) == 8)` in the temporary test harness.
  - Add two explicit cdecl callbacks that record arguments and invocation order. Use a three-node chain with node 0 as the retail head, node 1 as successor, and a terminal node. Verify handlers run in retail order, see the same window/message/wParam/lParam, and nonzero results fall through. Change the first callback to return zero and verify the second is not called and WindowProcedure returns zero.
  - Run the test before changing the header; expected compile failure or chain/layout test failure.

- [ ] **Step 2: Add explicit cdecl callback type and gap comment**
  - Define a small `IGI_RETAIL_CDECL` macro: `__cdecl` for MSVC x86, `__attribute__((cdecl))` for GCC/Clang i386, empty elsewhere.
  - Define `using WindowHandlerCallback = std::intptr_t (IGI_RETAIL_CDECL *)(void*, std::uint32_t, std::uintptr_t, std::intptr_t);` and use it for `WindowHandlerEntry::handler`.
  - Undefine the helper macro after the public declarations, retaining the alias’s encoded type.
  - Add the unresolved `WM_ACTIVATE` call `0x00498460` to the header’s documented gaps. Do not implement it.

- [ ] **Step 3: Run focused regression tests**
  - Run `python3 scripts/test_window_procedure.py`.
  - Expected: existing Win32-shim behavior checks, chain order/argument/short-circuit checks, x86 layout assertions, `_WindowProcedure@16`, `ret $0x10`, and explicit cdecl callback call evidence all pass.

- [ ] **Step 4: Run pre-fix negative control**
  - Extract the pre-change header/source into a `TemporaryDirectory` using `git show HEAD:<path>`; adapt the test runner only to point at those temporary files if needed.
  - Expected: new layout or ABI assertions fail against the old implicit callback type under a forced non-default x86 convention, or the check reports exactly which ABI property is not encoded. The control must not modify tracked files.

- [ ] **Step 5: Review diff and status**
  - Run `git diff --check`.
  - Confirm no manifest/evidence files changed, no executable deployed, and `0x0048F970` remains `TRANSCRIBED`.
  - Keep any inability to prove cdecl with a stable compiler-visible check explicit rather than treating compilation alone as proof.
