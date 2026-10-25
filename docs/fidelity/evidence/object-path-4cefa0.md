# The object path: `0x004CEFA0` (`.mtp` package opener) and `Resource_Flush 0x004B63D0`

Measured against retail `igi.exe` SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, PE32 i386 image base `0x00400000`, `.text` VA `0x00401000` raw `0x1000`. Instrument: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a `ret`, jump-table bodies seeded), instructions counted by distinct address, blocks/edges by splitting at every branch target and after every branch.

## 1. `0x004CEFA0` — the `.mtp` package opener

| property | measured |
| --- | --- |
| extent | `0x004CEFA0`–`0x004CF043`, **164 bytes (0xA4)** |
| `ret` boundary | `ret` at `0x004CF043`, ends `0x004CF044` |
| byte after the extent | **twelve `90` nops**, then `0x004CF050` begins `push %esi` |
| instructions | **51** |
| blocks / edges | **1 / 0** — straight line, no branches at all |
| argument | one, `char const*` (caller passes `lea edx,[edi+0x444]`) |
| result | the record pointer (`mov %esi,%eax`), stored by the caller into `[0x0057BABC]+0x24` |
| call sites image-wide | **2**: `0x00415C9E` and `0x00415CE1`, both in the phase-0 level-start handler |

File: `engine/src/retail/dynobj/native_4cefa0.cpp`. Read to the `ret` before it was described; the description below is from the complete body, not a head read.

**Body, in retail order.** `Mem_Alloc(0x4C,4)` for a 76-byte record; three fields cleared (`+0x04`, `+0x00`, `+0x48`); `Sys_InsertListTail_4AF8F0(&g_a4ec60, record)` appends it to the global package list; three embedded twelve-byte sentinels initialised in place at `+0x10`, `+0x28` and `+0x1C` (the third's middle dword `+0x20` is the word cleared earlier at `0x004CEFED`, which is why no second clear appears); `sprintf(buffer, "%s%s.mtp", g_a4ed6c, name)` into the `0x40`-byte frame local that `sub esp,0x40` allocates; `Resource_Load(buffer, nullptr)` with the handle stored at `record+0x08`; `DynObj_LoadModelTexturePackage(record, name, 1)`; then `0x004CF050()`. One single cleanup `add esp,0x34` at `0x004CF034` for all **thirteen** argument dwords pushed since the frame opened (`0x34` = 13 dwords, which matches the push census exactly — no callee cleans its own arguments).

**This is the missing link, confirmed by measurement:** `0x004CF080` (`DynObj_LoadModelTexturePackage`, already transcribed at `dynobj/mtp_package_loader.cpp:308`) has **exactly one call site image-wide, `0x004CF02F`, inside this body**. The `'INST'` chunk resolution in that native therefore has no transcribed caller until this file lands.

**Two details that differ from the tree's existing notes.** (1) `0x004CEFA0` has **two** call sites, not one: the note at `core/game_flow.cpp:80` records only `0x00415C9E`. (2) The prefix buffer `0x00A4ED6C` holds **zero bytes in the image** (BSS); the tree initialises its owner copy to `"LOCAL:"`. This transcription reads the global rather than inlining `"LOCAL:"`, so that `0x004CF080`'s animation path and this opener read the same buffer — the §10.14 rule-10 literal was considered and rejected for exactly that reason.

## 2. `Resource_Flush 0x004B63D0`

| property | measured |
| --- | --- |
| extent | `0x004B63D0`–`0x004B6413`, **68 bytes** |
| `ret` boundary | `ret` at `0x004B6413`, ends `0x004B6414` |
| byte after the extent | **twelve `90` nops** |
| instructions | **25** |
| blocks / edges | **9 / 15** |
| call sites image-wide | **26** — a widely used flush, not a private helper of the QVM teardown |
| callees | `Debug_Error 0x004AF7B0` (tree), `Resource_FreeNode 0x004B5C90` (tree) |
| string | `0x00547150` `"Resource_Flush() : Couldn't locate resource at %p."`, byte-exact |

File: `engine/src/retail/resource/native_4b63d0.cpp`.

`0x004B63D0` reads the **dword at** the global `0x00943E38` and treats it as the first node, so that global holds the head pointer and the list links through `+0x00`; each visited node is compared by its dword at `+0x34`. **Two retail quirks preserved verbatim:** (1) the null test at `0x004B63DC` precedes the key comparison, so when the head has no successor the function reports not-found **without ever comparing the head's own key** — a one-element list can never be found; (2) the walk advances through the node it just compared, so the last node of a longer list is compared before the null test ends the loop. On a match, `+0x24` selects: `3` returns it, `1` returns it, anything else releases the node through `Resource_FreeNode` and returns that result. On no match it formats the message through `Debug_Error` and spins forever at `0x004B63FB` — the fatal path never returns.

## 3. Fail-closed status — exit status and exact symbol lists

Both files were compiled and link-checked, and neither was made to link by stubbing a callee.

| file | compile | link check | undefined in tree |
| --- | --- | --- | --- |
| `dynobj/native_4cefa0.cpp` | `rc=0`, zero warnings | **`rc=1` NOT LINKABLE** | `DynObj_SweepPackageRecords_4CF050`, `g_a4ec60`, `g_a4ed6c` |
| `resource/native_4b63d0.cpp` | `rc=0`, zero warnings | **`rc=1` NOT LINKABLE** | `g_943e38` — **and nothing else** |

Everything else in both files resolves to tree natives, verified by `nm` on the objects: `Sys_InsertListTail_4AF8F0`, `igi::retail::mem::Mem_Alloc`, `igi::retail::dynobj::DynObj_LoadModelTexturePackage`, `igi::retail::resource::Resource_Load`, `Debug_Error`, `igi::retail::resource::Resource_FreeNode` (plus the toolchain's `___mingw_sprintf`).

`native_4b63d0.cpp` therefore closes the `Resource_Flush` half of the terrain closure and leaves **the sentinel half open**: `0x00943E38` has no owner anywhere under `engine/`, and that is the single symbol standing between the terrain closure and a link.

## 4. The two measurements requested: `0x004CF050` and `0x004CED50`

| target | extent | bytes | instructions | blocks | `ret` | nops after |
| --- | --- | --- | --- | --- | --- | --- |
| `0x004CF050` | `0x004CF050`–`0x004CF072` | 35 | **13** | 5 | `0x004CF072` | 13 |
| `0x004CED50` | `0x004CED50`–`0x004CEE26` | 215 | **84** | 5 | `0x004CEE26` | 9 |

`0x004CF050` is exactly the sweep the Lead described: `esi = 0x00A443A0`, and for each `0x48`-byte record it tests the dword at `+0x14` and calls `0x004CED50(record)` when it is non-null, advancing until `esi >= 0x00A4EC60` — **600 records** (`0xA8C0 / 0x48`). Two notes for whoever transcribes it: the guard is `lea eax,[esi+0x14]; test eax,eax; je` — a computed address that can never be null, so **the branch is dead and every record is always passed to `0x004CED50`** (a faithful transcription keeps the check); and it has a second call site at `0x004CFB1F` besides `0x004CF037` in this new file.

**Sizing warning — `0x004CF050` is small but does not stand alone.** Its one callee is `0x004CED50` (84 instructions), which takes the record, reads the string at `record+0x14` with `repne scasb`, copies it to a local, pushes the literal `"johan"` at `0x0053C918`, calls `0x004A5BB0` (66 instructions, extent `0x004A5BB0`–`0x004A5C2F`, no alignment padding after it, **no manifest record**), then loops `i = 1..5` computing `100 / (i*i)`, storing each word at `record+0x3E + 2*(i-1)`, calling `0x004CEE30` (54 instructions, extent `0x004CEE30`–`0x004CEE9A`, manifest name `DynObj_ParseConfigFile_13`, implementation path outside the tree) and storing each result dword at `record+0x00 + 4*(i-1)`, finally writing a word at `record+0x3C`. So the minimum closed unit is **217 instructions across four functions** (13 + 84 + 66 + 54), and two of those four have no tree implementation at all.

## 5. Follow-ups the Lead owns

1. **Two globals have internal linkage.** `dynobj/mtp_package_loader.cpp:15` opens `namespace { ... }` and defines `g_a4ec60` and `g_a4ed6c` inside it. Proven by `nm` on the host object: `g_a4ed6c` mangles to `__ZN3igi6retail6dynobj12_GLOBAL__N_18g_a4ed6cE` with type **`d`** (local), while `DynObj_LoadModelTexturePackage` is type `T` (external). The `_GLOBAL__N_1` marker is the unnamed namespace. Exposing those two names (moving them out of the unnamed namespace, or declaring them in the header) is what lets `native_4cefa0.cpp` link; no stub was used instead.
2. **A one-line interface correction.** `script/native_4bd380.cpp:21` declares `void NATIVE_DECL Resource_Flush(void* data);`, inferred from a call site where EAX is discarded (retail overwrites it at `0x004BD3B6`). The new definition returns `std::int32_t` because the bytes leave the compared field value (1 or 3) or `Resource_FreeNode`'s result in EAX at the `ret`, and returning it is what preserves that machine state. The bytes cannot distinguish `void` from `int` here; the caller's declaration should be updated for agreement. C linkage means the mismatch cannot break the link.

## 6. Open questions

- `0x00943E38`: is it a head-pointer variable or a sentinel struct at that address whose first field is read? The two readings are machine-identical for this body; the declaration here is `void*`.
- Whether `0x004CF050` + `0x004CED50` + `0x004CEE30` + `0x004A5BB0` go into this increment or the next; measurement only, nothing written.
- `"johan"` at `0x0053C918` is byte-exact but its role is unexplained — recorded rather than guessed.
