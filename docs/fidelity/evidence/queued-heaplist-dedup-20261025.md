# Queued change: remove the 0x004AF8F0 duplicate and its helper (verified 2026-10-25)

Status: QUEUED. Must not start while an autoverify run owns the build directory, because
`engine/src/retail/mem/heap_list.cpp` is in the build target and editing it invalidates the
run's build receipts.

## What is verified in the tree today

Retail `0x004AF8F0` is 28 bytes, `0x004AF8F0-0x004AF90B`, 1 basic block, 10 instructions,
0 edges, `ret` at `0x004AF90B`, with `nop` padding after it. The ten instructions are:

```
mov eax,[esp+4]      mov ecx,[esp+8]      mov edx,[eax+8]      mov [ecx+4],edx
lea edx,[eax+4]      mov [ecx],edx        mov edx,[eax+8]      mov [edx],ecx
mov [eax+8],ecx      ret
```

Two files implement that one address, which violates rule 13 (one implementation per
retail VA). Measured:

| file | symbol | matches retail? | call sites |
| --- | --- | --- | --- |
| `engine/src/retail/sys/native_4af8f0.cpp` | `igi::retail::sys::Sys_InsertListTail_4AF8F0` | **yes** - extent, block count, instruction count and the ten instructions in order, including the double `mov edx,[eax+8]` re-load at `0x004AF903` | 2 |
| `engine/src/retail/mem/heap_list.cpp` | `igi::retail::mem::HeapList_Insert` | no - same four stores rewritten as pointer arithmetic, provenance claims `0x004AF8F0-0x004AF910` (32 bytes, 4 past the real end) and cites the artifact `CLEAN/camera/004AF8F0-Camera_SetFieldOfView_33.c`, which is another routine's name | 14 |

Both are compiled and linked, in different namespaces, so no symbol collision occurs and a
green build cannot reveal this. The live copy is the **wrong-provenance** one: 14 files call
`HeapList_Insert`, only 2 call `Sys_InsertListTail_4AF8F0`.

## The second defect

`engine/include/igi/retail/mem/heap_list.hpp:21` defines

```cpp
inline void* Camera_SetFieldOfView_33(void* context, void** arg2) { return HeapList_Insert(context, arg2); }
```

That is a forwarding helper (rule 9: no helper methods at all; rule 2: no port-only
scaffolding) carrying a decompiler-artifact name (rule 8: no artifact names). It is used
from 6 files, including `engine/src/retail/dynobj/mtp_package_loader.cpp` at lines 228, 279
and 387.

## Planned change (one change, after the run releases the build)

1. `git rm engine/src/retail/mem/heap_list.cpp engine/include/igi/retail/mem/heap_list.hpp`.
2. Repoint every `HeapList_Insert(...)` and `Camera_SetFieldOfView_33(...)` call to
   `Sys_InsertListTail_4AF8F0(...)` with the correct namespace, and fix the
   `mem_alloc.hpp:149` comment that names the removed helper.
3. Remove the `mem/heap_list.cpp` entries from the build target
   (`CMakeLists.txt` and `engine/CMakeLists.txt`).
4. Build; confirm the target links with no undefined symbols and that
   `HeapList_Insert`/`Camera_SetFieldOfView_33` no longer appear anywhere.
5. Manifest, CHANGELOG, commit.

Behaviourally safe: both copies perform the same four stores, and the surviving copy is the
one whose provenance matches retail exactly. The alternative - deleting the `sys` copy -
would keep a file whose extent and artifact attribution are wrong, and would still leave the
helper, so it is the wrong direction.
