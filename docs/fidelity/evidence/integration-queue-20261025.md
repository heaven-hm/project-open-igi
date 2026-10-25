# Integration queue, 2026-10-25

Natives written by the worker lanes and verified by the Lead, waiting for the autoverify run
to release the build directory. All are new untracked files, so none of them can invalidate
the run's build receipts; none is in the build target yet.

| VA | file | native | verified by Lead | links today? |
| --- | --- | --- | --- | --- |
| `0x004F1CB0` | `sys/native_4f1cb0.cpp` | `Sys_InitFloatPair_4F1CB0` | compile clean, 0 undefined, `.data` matches image | yes |
| `0x00488BB0` | `core/native_488bb0.cpp` | `Task_DefineLevelFlow` | compile clean, 35 calls, 19 undefined all C-linkage, 0 `__Z`, 0 `stack_chk`, r2 580 B/171 i/1 block | yes |
| `0x00488800` | `core/native_488800.cpp` | `Task_DefineEditVariable` | compile clean (warning fixed), 23 calls, `.data` = image dword `0x00000181`, one `66 a3` 16-bit store | yes |
| `0x00488620` | `core/native_488620.cpp` | `Task_DefineSubLevel` | compile clean, 13 calls, 10 undefined all C-linkage, r2 202 B/56 i/1 block | yes |
| `0x00487BE0` | `core/native_487be0.cpp` | `GuardGenerator` registrar | compile clean, 28 calls, 17 undefined all C-linkage, r2 421 B/115 i/1 block | yes |
| `0x00402FF0` | `core/native_402ff0.cpp` | `Config_RegisterGameOptions_402FF0` | compile clean, 81 calls, 11 undefined, r2 per-block counts match the 1639 B/15 block/449 instruction extent | **no** - needs the four gaps below |

Verification method for each: `i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra -c
-Iengine/include <file>`, then `nm -u` for C-linkage/`__Z`/`stack_chk` counts, then
`i686-w64-mingw32-objdump -d` for the call count, then radare2 `af` + `afbj` for the
structure, compared against the file's own provenance claim. One VA per file was checked
against the rest of the tree.

## The one dependency chain that matters

`0x00402FF0` is the guard frontier (position 5 of the 155-call sequence at `0x00402930`), but
its implementation references four natives that do not exist yet, so it cannot link:

| gap VA | claimed behavior | note |
| --- | --- | --- |
| `0x004B88F0` | `Script_QHashNodeRegisterMode2_4B88F0` | exactly one registration uses it, `"Config_GraphicOptionsSetGamma"`; body is `0x004B8890`'s with tail constants 1,2,0 instead of 1,0,0 |
| `0x00491EA0` | `Display_DriverDispatch_491EA0` | builds `{arg1, &local, arg2}` on its stack, calls the driver handler at `0x00A94E84` indexed by `[0x00542334]` |
| `0x00491CF0` | `Display_GetActiveMode_491CF0` | |
| `0x00491980` | `Display_ApplyFlags_491980` | |

Consequence: implementing position 5 does **not** by itself move the guard. The frontier
moves only when `0x00402FF0` compiles **and links**, which needs those four. Assigned to
`handler-abi-worker` for those four VAs only, to avoid colliding with `chain-worker`'s
`0x46xxxx-0x0048xxxx`.

## Resolved question

`native_402ff0.cpp` declares `extern "C" char Config_ContentControlPassword[0x20]`, while
`config_profile.hpp:18` declares it inside the config namespace. Checked against the linked
candidate rather than by reading: the image defines `_Config_ContentControlPassword`, i.e. the
C-linkage symbol, so the `extern "C"` declaration matches the real definition. No change
needed.

## Still queued separately

Removal of the `0x004AF8F0` duplicate and the `Camera_SetFieldOfView_33` helper:
`docs/fidelity/evidence/queued-heaplist-dedup-20261025.md`.
