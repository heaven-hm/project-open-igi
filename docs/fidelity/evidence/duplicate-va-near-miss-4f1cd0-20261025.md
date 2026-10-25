# Duplicate-VA near miss: 0x004F1CD0 declared as a new gap (2026-10-25)

Status: caught before integration, no duplicate written. Recorded because the same trap will
recur while several agents work the same call graph from different directions.

## What happened

Two workers independently needed retail `0x004F1CD0` and `0x004F1DB0` as callees:

- `chain-worker` (working `0x0047CF20`) declared both and its compile showed
  `U __ZN3igi6retail3sys29Sys_InitGlobalListHead_4F1CD0EPv` - a mangled reference that resolved
  against an existing definition.
- `handler-abi-worker` (working `0x00413DF0`) declared both as **new gaps**, named
  `Script_NameSpaceAttachHandler_4F1CD0` and `Script_NameSpaceAddPattern_4F1DB0`, and reported
  "two new gaps declared here, so this file cannot link until they land".

Only one of those is a gap. Verified in the tree:

```
engine/src/retail/sys/native_4f1cd0.cpp:23  void NATIVE_DECL Sys_InitGlobalListHead_4F1CD0(void* head)   // VA 0x004F1CD0
CMakeLists.txt:301                          engine/src/retail/sys/native_4f1cd0.cpp                     // in the build
```

`0x004F1DB0` is genuinely unimplemented: nothing under `engine/src` defines it.

So `handler-abi-worker`'s `0x004F1CD0` was about to become a **second implementation of an
already-implemented retail VA** - rule 13. It was stopped before it wrote one, and the
declaration is being deleted in favour of the existing definition.

## Root cause: two namespaced natives

`sys/native_4f1cd0.cpp` and `sys/native_4f1d60.cpp` wrap their definitions in
`namespace igi::retail::sys` rather than `extern "C"`, so they export
`__ZN3igi6retail3sys...` while every other native in the tree exports a C-linkage name. A caller
therefore cannot see the VA as implemented by looking for the C name, and one caller was
"rewarded" for the mangled form while the other concluded the VA was missing. Both files are in
the build; the linkage is being normalised to `extern "C"`.

## The check that would have caught it earlier

Before declaring any callee as a gap, search for the VA and for the C name across `engine/` -
not for the placeholder name alone. A missing *name* is not a missing *implementation*, and a
mangled symbol in an `nm` output means some definition exists with a non-C linkage.

## Name unification applied

`0x004F1DB0` now has one name: `Sys_NewListRecord_4F1DB0`, implemented in
`engine/src/retail/sys/native_4f1db0.cpp`, referenced by both callers. `0x004F1CD0` keeps its
existing name `Sys_InitGlobalListHead_4F1CD0` and its existing file. No VA has two names and no
VA has two implementations.
