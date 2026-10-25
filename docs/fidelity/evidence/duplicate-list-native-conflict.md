# Open conflicts in the intrusive-list natives (found 2026-10-25, not yet fixed)

Scope: this is a record of a real rule-13 (one implementation per retail VA) and rule-9 (no invented helpers) violation that survives in the
tree today. It is written down rather than half-fixed because the fix touches six translation units and one of the callers is still a legacy
port file whose addresses do not line up.

## The duplicate

`0x004AF8F0` has two implementations, both in the `igi` exe target:

- `engine/src/retail/sys/native_4af8f0.cpp` - `Sys_InsertListTail_4AF8F0`, per-instruction provenance (28 bytes, 1 block, 10 instructions),
  namespace `igi::retail::sys`, `NATIVE_DECL` cdecl.
- `engine/src/retail/mem/heap_list.cpp` - `HeapList_Insert`, "complete port", provenance citing external candidates
  `CLEAN/camera/004AF8F0-Camera_SetFieldOfView_33.c` and a 32-byte extent (`0x004AF8F0-0x004AF910`) instead of the measured 28 bytes.

They agree on the splice sequence (node[1] = *(head+8); node[0] = head+4; **(head+8) = node; *(head+8) = node; return head), so the duplication
does not change behaviour today - it means the same retail routine exists twice, and the second copy is labelled with a camera-candidate name
that has nothing to do with what the body does.

## The invented aliases

`engine/include/igi/retail/mem/heap_list.hpp` adds two inline wrapper functions that have no retail address:

- `Camera_SetFieldOfView_33(context, arg2)` -> `HeapList_Insert` - called from `dynobj/mtp_package_loader.cpp` at lines 228, 279, 387.
- `Camera_ApplyHandiCam_33(node)` -> `HeapList_Remove` - called from `resource/sys_resource.cpp` line 252.

Both are rule-9 violations (invented helpers) whose names come from external candidate filenames, and the second one is worse than cosmetic:
`sys_resource.cpp`'s provenance describes `Camera_ApplyHandiCam_33 (0x004AF910)` while the transcribed remove native is `0x004AF960` and the
legacy header pairs the alias with `HeapList_Remove` (0x004AF960, 33 bytes). One of those three addresses is wrong and the record does not say
which; that has to be settled against retail before anything is renamed.

`0x004AF960` (`HeapList_Remove`) currently has **no** per-instruction transcription - only the legacy port copy - so deleting the legacy file
outright would remove the only implementation of the remove half.

## What a correct fix requires

1. Transcribe `0x004AF960` as its own file under `engine/src/retail/mem/` (or `sys/`, matching wherever `0x004AF8F0` lives) with a measured
   per-instruction provenance block, and settle `0x004AF910` vs `0x004AF960` from the image.
2. Delete `HeapList_Insert` from `heap_list.cpp` and retarget the five insert call sites to the transcribed native:
   `core/native_48f490.cpp:41`, `script/native_4bac30.cpp:46` and `:55`, `qfile/native_4b19c0.cpp:34`, `mem/mem_alloc.cpp:242`.
3. Delete `heap_list.hpp`'s declarations and both inline aliases, replace them with the retail-native declarations, and update the six files
   that include the header.
4. Re-point `mtp_package_loader.cpp`'s three call sites at the retail routine they actually call. Its own provenance names `0x004B0E60`
   (a different insert) at three places, so the alias may be masking a missing native rather than merely misnaming one.
5. Only after that, delete `mem/heap_list.cpp` if nothing else in the exe target uses it.

## Why it is not done in this pass

Item 4 changes the dynobj package loader's call targets, and item 1 needs a fresh transcription of the remove half. Doing that while the
startup-chain workers are adding natives would put two writers in the same dependency graph; it is queued as its own increment.
