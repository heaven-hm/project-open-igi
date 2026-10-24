# WinMain mutex prefix — observed correction, complete native still provisional

Retail: `igi.exe`, SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
Source: `engine/src/retail/core/main.cpp`, SHA-256 `df5a4ba29099be4bd45b0a6db4d6d3d7c3a7e526d61abae429ec82e82e5e56ed`.

Binary Ninja and Radare2 independently recover 26 retail blocks, CFG similarity 1.0;
angr recovers the exact entry without unresolved jumps. `capture.json` preserves
all tool output, including the decompiler's incorrect two-argument/name inference.
The Windows four-stack-argument boundary is supported by `retn 0x10`, independently
of that inference. Normalized graphs retain their raw-capture digest.

## Correction

Retail 0x48F54A/554 checks the mutex handle for NULL. Only the non-null path calls
GetLastError at 0x48F560, compares 0xB7, calls it again at 0x48F56D and compares 6.
The previous candidate omitted the null check and collapsed both calls into one.
The source now expresses those three short-circuited branches in that order.

`before-retail.json` and `before-candidate.json` preserve the initial diagnostic:
retail made two error queries; candidate `a1f2ab616cd952c766ea34d016e45ec39fdf703e9f790412e3cf63e2ac7510a4`
made one. That initial probe's bounded string decoder was subsequently corrected;
its mutex-name text is not evidence of string bytes. Its ordered API events remain
the recorded discrepancy. A later attempt to recapture that baseline found the
shared build had been replaced by another agent, so it is not a before-fix result.

## Current-source runtime scope

`comparison.json`, `retail.json[l]` and `candidate.json[l]` retain the final probe's
paired capture, retail first, same `window level1`, same game cwd, shared lease,
three-second bounded observations. Both owned processes were confirmed gone.
The probe only observes; it never forces return values or renames a mutex.

Candidate: `build-mutex-prefix-sol/igi1.exe`, SHA-256
`1f2290efe0ad6067534da524e32aff9e1763ec0ec0d65edb8f7ff28c547b0c13`.
`diagnostic-build.json` binds all 192 linked source files, dependencies, compiler,
objects and source snapshot. Its receipt was validated before and after capture.
The first receipt became stale when a concurrent agent added a source; that run
was rejected before launch. The recorded successful pair uses the refreshed build.

Both sides observed the order WinMain -> CreateMutexA -> GetLastError ->
GetLastError -> CoInitialize. Twelve checks passed for order, null security and
COM arguments, initial ownership, expected mutex names, non-null handles, two zero
error returns, absence of probe failures and owned-process cleanup. The authorized
testing build uses `IGI1MUTEX`; retail uses `IGIMUTEX`. Raw handles are not compared
across processes.

## Outstanding requirements

This is a successful-mutex-prefix regression, not complete WinMain acceptance.
Null handle, error 0xB7 and error 6 were not exercised. Complete startup, the
DirectX gate, frame-loop error path and shutdown remain open. Candidate startup
still encounters the unresolved class-registrar dependency. Independent critic
review and an authorized full-native typed contract remain pending. Strict
changed-native acceptance remains blocked; no VERIFIED status was assigned.

The diagnostic executable remains in its separate build directory; this task did
not deploy it over the other agents' candidate or change CMake's source list.
