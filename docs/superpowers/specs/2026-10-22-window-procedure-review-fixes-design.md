# WindowProcedure Review Fixes Design

## Goal
Make the current `0x0048F970` reconstruction’s callback ABI explicit, cover the retail chain node layout (`next` at `+0x00`, `previous` at `+0x04`, handler at `+0x08`) and dispatch behavior in the focused host harness, and document unresolved behavior without promoting verification status.

## Scope
- Introduce an explicit x86 cdecl callback type for each `WindowHandlerEntry::handler`, preserving the 12-byte x86 node, name the `+0x04` field `previous` (retail writes it from `HeapList_Insert` at `0x004AF8F0`), and keep handler offset `+0x08`.
- Extend `scripts/test_window_procedure.py` with x86 size/offset checks and realistic callback-chain coverage for argument forwarding, order, nonzero fall-through, and zero-result interception.
- Document that the Win32 `WM_ACTIVATEAPP` (`0x001C`; `WM_ACTIVATE` is `0x0006`) branch calls the x87 chop-rounding setter at `0x00498460` and returns zero. The setter is transcribed as `FP_InitExtendedPrecision`; the `WM_DESTROY` shutdown call at `0x004010B0` remains unimplemented.
- Record the head-null and handler-null checks as port-only safety divergences: retail loads the head global `0x005C8DF0`, dereferences it, and calls `[eax+8]` without null checks. The `WM_DESTROY` tests run with a null chain, a state in which retail would fault, so they exercise only the port guard.
- Keep the native provisional (`TRANSCRIBED`); do not change manifests/evidence/status, use runtime tools, or deploy.

## Approach
Use a named callback typedef with `__cdecl` on MSVC x86 and `__attribute__((cdecl))` on GCC/Clang x86; use the platform default where x86 decoration is inapplicable. Keep the existing struct field order/types, renaming the `+0x04` field from `reserved` to `previous`; the candidate still needs only `next` and `handler`, but the layout stays 12 bytes. In the existing temporary host harness, add compile-time layout assertions under a 32-bit target and callback probes that record invocation order/arguments and return chosen values. The test remains explicitly a shim-based behavior test, not Windows or retail execution evidence.

## Verification
Run the focused test against the current source and a temporary pre-fix source negative control; check x86 `_WindowProcedure@16`/`ret 0x10`, callback symbol/decorations and node offsets with MinGW; run `git diff --check`. Runtime parity and `VERIFIED` acceptance remain out of scope.
