# Rename candidates where the tree's placeholder is contradicted

**Not renames.** Per the naming rule an unverified native keeps its address-suffixed
placeholder. These are recorded so the correction happens at verification time, not lost.

## Confirmed contradicted — `0x004C1780`

Retail bytes: `c3 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90` — a **bare `ret`** plus padding.
Independently verified: `RetailImage.recover(0x4C1780)` returns **exactly 1 instruction**, `ret`.

| source | name |
|---|---|
| this tree (`include/igi/retail/core/native_4c1780.hpp`) | `AnimSound_LoadParameters` |
| names.tsv | `Debug_Log` |
| IGI-RE ORIG | `Debug_Log` |

**A ret-only body cannot load parameters**, so the tree's name is wrong — and the tree's own header
already says so: *"the BN alias `AnimSound_LoadParameters` is not established for a ret-only body."*
Two independent external sources agree on `Debug_Log`, and a no-op stub pushed as a handler at slots
0 and 2 of many task types is what a compiled-out logging hook looks like.

**Limits:** a bare `ret` proves the routine is a no-op. It does **not** prove the name. `Debug_Log`
is the best available candidate from two agreeing sources, not a verified identity. The ORIG
signature `(void *task, int level, const char *fmt, ...)` is call-site-derived — the body contains
no parameter access at all.

**Rename requires:** the rename must update every call site in the same change, including
`engine/src/retail/core/native_51b400.cpp`.

## Signature disagreement — `0x00401900`

Retail reads **four** slots: `[esp+4]`, `[esp+8]`, `[esp+0xC]`, `[esp+0x10]`; it never reads
`[esp+0x14]`. Hand-verified from the prologue trace.

IGI-RE ORIG declares **five**: `(const char *name, int size, uint16_t parent, int pool_count, int unk)`.
This tree's `TaskType_Register` has four and matches the retail reads. IGI-RE's own body calls it
with five, matching the retail *call-site* convention — so their fifth parameter is declared and
never read.

**Rule:** prefer the retail read count, then the tree's declaration, then IGI-RE's ORIG. A parameter
retail does not read may be carried as call-site-only, and the disagreement is recorded.

## Name disagreements between names.tsv and IGI-RE ORIG (substantive, neither picked)

| VA | names.tsv | IGI-RE ORIG |
|---|---|---|
| `0x004AF990` | `List_InsertBefore` | `List_Insert` |
| `0x004F2040` | `Weapon_GetEvent7` | `Weapon_GetEvent7b` |
| `0x00523D40` | `Lightmap_BuildLod` | `BuildLod` |

Six further disagreements are trailing-underscore weak-symbol variants
(`Quat_SlerpPrepare_`, `DynCube_GetRoot_`, `Coll_Descend_`, `Mtp_FindChunk_`,
`VirModel_InitAllLods_`, +1) and are harmless.

## Names we invented that a real name supersedes

| VA | this tree (invented) | IGI-RE |
|---|---|---|
| `0x004F1A80` | `Script_TaskContainer_Append` | `ParamTable_Add` |
| `0x004F1950` | `Script_TaskContainer_Alloc` | `ParamTable_New` |

Both were invented before the symbol table arrived. The IGI-RE names are real names from a
decompilation that has a body for each.

## Tree defect settled — `0x004EDB60` takes ONE argument, the tree declares two

Measured: `0x4EDB60` spans `0x4EDB60-0x4EDBC2`, 39 instructions, tiling closes. It contains
**exactly one stack read in the entire extent**:

```
0x4edb62  depth=8  mov edi, dword ptr [esp + 0xC]     -> arg1
```

No other `[esp+K]` or `[ebp+K]` read anywhere in those 39 instructions. At depth 8, `[esp+0xC]`
is entry `[esp+4]` = arg1. So retail reads **one** argument, and names.tsv independently names it
`ModelLODSettings_Changed`.

| source | claim |
|---|---|
| retail (measured) | one argument |
| names.tsv | `ModelLODSettings_Changed` |
| IGI-RE | `ModelLODSettings_Changed`, one argument |
| **this tree** | `LODSettings_LoadParameters(void* context, LODSettingsLookup lookup)` — **two parameters** |

`engine/src/retail/render/lod_settings.cpp:37` and `engine/include/igi/retail/render/lod_settings.hpp:40`.
The tree's second parameter is one retail never reads, and the tree's name is contradicted by two
independent external sources.

The converter flagged this and **declined to substitute**, keeping IGI-RE's name and leaving the link
edge open rather than silently resolving it. That was the correct call.

**Not changed.** It is an existing unverified native; the naming rule keeps its placeholder until
verification. Recorded so the correction is not lost.
