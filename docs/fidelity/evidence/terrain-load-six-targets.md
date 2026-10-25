# The six missing terrain-closure targets: transcription result and link status

Scope of this note: the six targets the Lead dispatched as "the six missing terrain targets" — the
members of the 11-function `Terrain_Load` closure that had no manifest record at dispatch time (the
closure's one-level target set is re-measured at 44 in `level-load-closure.md`, whose earlier "26
combined targets / 20 with a file / 6 without" figures are withdrawn there) — plus
the two targets that turned out to sit *behind* them, plus the one that is still open behind those.
Instrument rules are the ones in `docs/fidelity/evidence/level-load-closure.md` §1 and
`docs/fidelity/evidence/guard-mvp-partition.md` §0: extents are bounded by the `ret` and the alignment
nops (and the bound used is named for every target), instructions are counted by distinct address, and
a file is looked up **by name**, never by a pattern.

Binary for every figure below: retail `igi.exe` SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, image base `0x00400000`.

## 0. Two corrections to the dispatch, both measured before any file was written

**(a) The six were five.** `0x004BAB30` already has a file: `engine/src/retail/script/native_4bab30.cpp`
(`Script_QHashSymbolRemove_4BAB30`, TRANSCRIBED; LINKABLE). It has **no manifest entry**, which is why
the address looked missing when looked up through the manifest. Nothing was transcribed for it.

**(b) The first-`ret` bound is wrong for three of these targets.** For `0x004BAB30`, `0x004BD080`,
`0x004BD220`, `0x004BC960` and `0x004BD380` the reachable set contains branch targets **past** the first
`ret` (`je 0x004BAB6D`, `je 0x004BD1D5`/`ja 0x004BD1B2`, `jge 0x004BD274`, the 7- and 15-entry jump
tables, `jge`/`jl` tails). A linear decode stopped at the first `ret` therefore understates the body by
a wide margin. All seven figures below are from a **reachability decode** (fallthrough plus every branch
target, no fallthrough past a `ret`) from the verified entry, and the bound used is named per target.

| target | linear-to-first-ret (my earlier figure) | reachable instructions | last instruction | bound used |
| --- | --- | --- | --- | --- |
| `0x004BAB30` | 22 | **26** | `0x004BAB7A` (`jmp 0x004BAB7A`) | terminal self-jump `eb fe`, then `0x004BAB7C` is padding |
| `0x004BD080` | 56 | **107** | `0x004BD1D9` | `ret`, then `8b ff` hot-patch nop at `0x004BD1DA` |
| `0x004BD220` | 34 | **126** | `0x004BD373` | `ret`, then 12 nops |
| `0x004BC960` | 97 | **97** | `0x004BCA92` | `ret`, then `0x90` padding |
| `0x004BD380` | 27 | **27** | `0x004BD3C1` | `ret`, then 14 nops |

`0x004CE3D0` (3 instructions, `ret` at `0x004CE3DE`, 1 nop) and `0x004D11C0` (24 instructions, `ret` at
`0x004D121A`, 5 nops) are unchanged from the dispatch and were confirmed exactly.

**The jump table hides instructions the same way a second undefined symbol does.** The first corrected
figures in this section were still under-counts for the two functions whose dispatch is a jump table:
a reachability walk that stops at the indirect `jmp` never reaches the case bodies, so it measured
`0x004BC960` at 41 instructions and `0x004BD080` at 38. Seeding the table's own targets into the walk
(the entries are all below the table base, because the table follows the code) gives **97** and **107**,
and each walk's last instruction is then exactly the `ret` at the recorded extent end — so the corrected
counts fill the measured extents (307 B and 346 B) and the earlier ones did not. My provenance blocks
said 41 and 38 until this correction; the transcribed source always covered every case body, but the
*counts I reported* hid them. Per-function figures in §1 are the corrected ones.

**(c) The QVM status dispatch is a jump table, read from the image.** `0x004BD080` dispatches on
`state+0x20 - 1` through the 15-entry table at `0x004BD1DC`; `0x004BC960` dispatches on
`type_stack[i] - 1` through the 7-entry table at `0x004BCA94`. Both tables were read out of the image and
both are recorded entry-by-entry in the two new files' provenance blocks.

**(d) Blocks and edges, measured against the Lead's own captures.** Splitting the reachable listing at
every branch target and after every branch gives, for the four targets the Lead captured with both
tools: `0x004BC960` 12 blocks/20 edges, `0x004BD080` 20/32, `0x004BD220` 23/35, `0x004BD380` 5/7. The
**block counts agree with both radare2 and Binary Ninja on all four** (12, 20, 23, 5), and the edge
counts agree with Binary Ninja on three of four (`0x004BD080` 32 vs 31). Earlier block/edge figures in
these provenance blocks (9/12, 17/20, 20/24, 3/3) were estimates and have been replaced by these
measurements.

## 1. What was written (seven files, dependency order by exit status)

| # | file | native | retail VA | extent | reachable ins | link status |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | `engine/src/retail/terrain/native_4ce3d0.cpp` | `DynObj_UpdateDynTree_11` | `0x004CE3D0`-`0x004CE3DE` | 15 B | 3 | **linkable (0 undefined)** |
| 2 | `engine/src/retail/terrain/native_4d11c0.cpp` | `Terrain_InitContourHashAndPools_4D11C0` | `0x004D11C0`-`0x004D121A` | 91 B | 24 | **linkable (5 undefined, all in tree)** |
| 3 | `engine/src/retail/script/native_4baca0.cpp` | `Script_QHashNodeRelease_4BACA0` | `0x004BACA0`-`0x004BACC5` | 38 B | 13 | **linkable (4 undefined, all in tree)** |
| 4 | `engine/src/retail/script/native_4bc960.cpp` | `Qvm_FormatOperandText_4BC960` | `0x004BC960`-`0x004BCA92` | 307 B | 97 | **linkable (2 undefined, all in tree)** |
| 5 | `engine/src/retail/script/native_4bd080.cpp` | `Qvm_RunProgram` | `0x004BD080`-`0x004BD1D9` | 346 B | 107 | **linkable (3 undefined, all in tree)** |
| 6 | `engine/src/retail/script/native_4bd220.cpp` | `Qvm_LoadCompiledProgram_4BD220` | `0x004BD220`-`0x004BD373` | 340 B | 126 | **linkable (4 undefined, all in tree)** |
| 7 | `engine/src/retail/script/native_4bd380.cpp` | `Sys_QvmStandaloneRelease_4BD380` | `0x004BD380`-`0x004BD3C1` | 66 B | 27 | **NOT LINKABLE — `Resource_Flush` (see §2)** |

Rows 1 and 2 are the Lead's integration, not this writer's: `0x004CE3D0` and `0x004D11C0` were committed
at `a11d47db` ("terrain: integrate 0x004CE3D0 and 0x004D11C0") byte-for-byte as written
(`git diff HEAD -- engine/src/retail/terrain/native_4ce3d0.cpp engine/src/retail/terrain/native_4d11c0.cpp`
is empty), and the five script files are in `CMakeLists.txt` with manifest records whose names match this
note. The link-status column is measured **after** that integration, on the integrated tree.

Build evidence, measured, both by exit status:

```
i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra -Iengine/include -c <file> -o /tmp/q218/obj/<base>.o
  -> rc=0 with zero warning/error lines for all seven
python3 scripts/check_native_linkable.py <file>        # after the Lead's integration
  -> rc=0  native_4ce3d0 (0 undefined) / native_4d11c0 (5) / native_4baca0 (4) / native_4bc960 (2)
           native_4bd080 (3) / native_4bd220 (4)   -- "all defined in tree"
  -> rc=1  native_4bd380  "NOT LINKABLE — undefined in tree:" (Resource_Flush, and nothing else)
```

The single rc=1 line is the real result: **exactly one symbol** of the whole closure has no tree
implementation. An earlier run, taken before the Lead's manifest records and archive rebuild, also
reported the symbols that files #3 and #4 define (`Qvm_FormatOperandText_4BC960`,
`Script_QHashNodeRelease_4BACA0`) as undefined; that was the checker's stale archive, not a defect, and
it is why the independent `ld -r` check below was run as well. `check_native_linkable.py` compares each
file's undefined set against the **defined symbols of an already-built object archive** (its own header:
"undefined set against the DEFINED symbols of the already-built object archive") and refuses to answer
if that archive is stale. Independent check of the same question, by linking the seven objects as a unit:

```
i686-w64-mingw32-ld -r obj/native_4{ce3d0,d11c0,baca0,bc960,bd080,bd220,bd380}.o -o obj/seven.o   -> rc=0
i686-w64-mingw32-nm -u obj/seven.o
  _Mem_AllocIndexedBlock  _Mem_DeAllocFn  _Script_QHashListInsert  _Sys_UnlinkListNode_4AF960
  igi::retail::mem::Mem_Alloc  igi::retail::script::Qvm_ExecuteInstructions
  igi::retail::resource::Resource_Load  igi::retail::resource::Resource_Unload
  igi::retail::terrain::g_b04340 / g_b12b60 / g_b12c40 / g_b16cc0
  g_a43e8c  g_a43eb0                                   <- every one of these is a tree native
  _Resource_Flush                                      <- the ONLY one with no implementation in the tree
  ___chkstk_ms  ___mingw_sprintf  _memset              <- toolchain/runtime, not tree gaps
```

## 2. The sufficiency answer: **the six are not sufficient — the closure is still open behind them**

**The closure is still open behind the six, and behind the two extra targets they hid. Measured, the
closure is open by exactly one tree function and one unowned global:**

- **`Resource_Flush` `0x004B63D0` has no declaration and no definition anywhere under `engine/`.**
  `grep -rn Resource_Flush engine/` returns nothing; the manifest points its implementation at
  `scripts/GameNatives/resource/004B63D0-Resource_Flush.cpp`, **outside the tree**. It is called by file
  #7 (`0x004BD380`) at `0x004BD3B1`. Measured properties for whoever transcribes it: 25 instructions,
  extent `0x004B63D0`-`0x004B6413` (`ret`, then padding), string `0x00547150`
  `"Resource_Flush() : Couldn't locate resource at %p."`, a fatal self-jump `jmp 0x004B63FB` on the
  not-found path, callees `0x004AF7B0 Debug_Error` (tree) and `0x004B5C90 Resource_FreeNode` (tree,
  `resource/sys_resource.cpp`), and it reads the global **`0x00943E38`, which currently has no owner
  anywhere in the tree**. Its signature in file #7 is a **call-site inference** (one 32-bit argument,
  cdecl, return unused here) and is flagged as such.
- **`0x004A54A0` is not a source-level dependency, and does not need a file.** `0x004BC960` calls it at
  `0x004BC96B` (`mov eax,0x1008` first), which is the MSVC 6 `__chkstk` probe for the 0x1008-byte frame.
  File #4 declares the buffer and lets the compiler emit its own probe, so it creates **no symbol
  reference to `0x004A54A0`** — proved by the `ld -r` undefined set above, which contains
  `___chkstk_ms` (toolchain) and not that address. The same reasoning applies to `_memset` and
  `___mingw_sprintf`.

Everything else the seven need is a tree native (`Mem_AllocIndexedBlock` `0x004B0B00`, `Mem_DeAllocFn`
`0x004B0D10`, `Mem_Alloc` `0x004B0C60`, `Script_QHashListInsert` `0x004AF910`,
`Sys_UnlinkListNode_4AF960` `0x004AF960`, `Qvm_ExecuteInstructions` `0x004BCFD0`, `Resource_Load`
`0x004B5F00`, `Resource_Unload` `0x004B6380`, and the six globals `g_a43eb0`/`g_a43e8c`/`g_b04340`/
`g_b12b60`/`g_b12c40`/`g_b16cc0`).

## 3. Two extra targets the six hid

- **`0x004BC960`**, called by `0x004BD080` (file #5), had no file. Written as file #4.
- **`0x004BACA0`**, called by `0x004BD380` (file #7), had no file. Written as file #3.

With files #1-#7 in the tree and rebuilt, `Terrain_Load`'s closure is complete **except** `0x004B63D0`
(`Resource_Flush`) and its global `0x00943E38`. Nothing else in the closure is missing.

## 4. Name and domain judgements the Lead must confirm or overrule

1. **`Qvm_LoadCompiledProgram` was already taken.** The tree's host-backed port
   `script/qvm_compiled_program.cpp:112` defines `QvmCompiledProgram Qvm_LoadCompiledProgram(std::span<const std::byte>)`
   for the same VA `0x004BD220`, and three call sites use it (`config/retail_config.cpp:356`,
   `script/qvm_loader.cpp:33`, `terrain/terrain_loaders.cpp:1661`). A second definition under that name
   would be a duplicate symbol and a §10.15 rule-13 violation, so file #6 carries the VA suffix
   `Qvm_LoadCompiledProgram_4BD220` and the collision is recorded in its provenance. The tree's
   existing description of `0x004BD220` agrees with my measured body.
2. **`Qvm_RunProgram` was already declared and never defined.** `script/qvm_execute.cpp:86` declares
   `std::int32_t NATIVE_DECL Qvm_RunProgram(std::uint32_t)` and calls it at line 137, with no definition
   anywhere. File #5 uses that exact name and signature, so it resolves that symbol. It does **not**
   make `qvm_execute.cpp` linkable: measured, that file still reports seven other undefined symbols of
   its own host layer (`Qvm_ParseBlock`, `Qvm_ReadNextBlock`, `Qvm_ReleaseBlock`,
   `Qvm_ReleaseBlockTree`, `Qvm_ReleaseSourceReader`, `Qvm_SourceBufferInit`, `Qvm_SourceReaderInit`).
3. **`DynObj_UpdateDynTree_11` is adopted, not invented.** The tree already names `0x004CE3D0` that way
   in `terrain/terrain_loaders.cpp:1275` and `terrain/terrain_loaders.hpp:215`. The measured body is a
   single field clear (`[arg+0xDC8] = 0`), which does not match "UpdateDynTree"; the name is kept so one
   VA does not acquire two names, and the mismatch is recorded in file #1's provenance.
4. **`Sys_CheckHeap_28` is a wrong alias and was not used.** `script/qvm_native_loader.cpp:228` attaches
   that catalog alias to `0x004BD380`; the body releases a QVM context and checks no heap. File #7 uses
   the address-suffixed placeholder `Sys_QvmStandaloneRelease_4BD380` with the signature
   `std::int32_t(void*)` to match the tree's existing boundary type `SysQvmStandaloneReleaseFn`
   (`qvm_native_loader.hpp:20`).
5. **Domain folders: five `script/`, two `terrain/` — a deliberate deviation from "`terrain/` for the
   new files".** §10.15 rule 15 requires the folder of the subsystem. `0x004CE3D0` and `0x004D11C0` are
   terrain (the former is `Terrain_Load`'s exit value, the latter initializes the `0x00B04340` contour
   hash cache and the three quadtree pools). The other five are the script/QVM subsystem: `0x004BACA0`
   and `0x004BD380` move script-symbol-table nodes and globals, `0x004BC960` and `0x004BD080` are the QVM
   interpreter's diagnostic formatter and program runner, and `0x004BD220` builds a QVM context from a
   loaded resource. Their siblings all live in `engine/src/retail/script/`. Move them if you disagree;
   the VAs are the identity, not the folder.
6. **The `%f` case in file #4 is a float-to-double vararg**, measured: `fld dword [ecx+esi*4]` then
   `fstp qword [esp]` before the `sprintf`, so the format is fed a double, not a float.
7. **The buffer in file #4 is never consumed.** The formatted text is written to the stack frame and
   neither this body nor its only caller (`0x004BD080`, which re-reads `state+0x20`) ever reads it back.
   That dead store is retail's behaviour and is preserved verbatim rather than tidied away.

## 5. Status

TRANSCRIBED and BUILD_VERIFIED for all seven (i686-w64-mingw32-g++, `-std=c++20 -O2 -Wall -Wextra`,
zero warning lines, rc=0 each). STRUCTURALLY_VERIFIED against each reachable instruction listing and
both jump tables read from the image. Runtime UNOBSERVED for all seven. Link status per file is in §1;
the seven as a unit leave exactly `Resource_Flush` and the toolchain helpers undefined.

Not touched, per the dispatch: `CMakeLists.txt`, `docs/fidelity/native-verification.json`, and every
existing `native_*.cpp`. No commit, no push.
