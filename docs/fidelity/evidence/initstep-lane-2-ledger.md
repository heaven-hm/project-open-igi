# InitStep lane 2 — measurement ledger (VAs 0x505C90 onward)

Author: `initstep-worker`. Scope: the 50 remaining `AIGraph_InitStep_*` targets from
`initstep-targets-sorted.txt`, worked smallest-first from **0x505C90**. The six units already
landed by `chain-worker` are not repeated here.

Method: `initstep-transcription-method.md`, ten-step checklist, plus `deferred-push-rule.md`.
Every count below comes from an **ndisasm 3.01 linear sweep of the exact extent** with the
byte accounting asserted to close; `scripts/native_fidelity_audit.py` `call_index()` decides
called-vs-address-taken. Capstone and ndisasm agreed on every unit so far.

Status ceiling for every unit in this lane: **BUILD_VERIFIED / STRUCTURALLY_VERIFIED, runtime
UNOBSERVED.** Nothing here has runtime evidence.

---

## 0x505C90 — `Container_Init` (candidate name) — 42 instructions

- **File:** `engine/src/retail/core/native_505c90.cpp`
- **Extent:** 0x00505C90-0x00505D26, 151 bytes (0x97). ndisasm **42** instructions, capstone 42,
  byte accounting **closes** (42 instructions over all 151 bytes). List says 42 — agrees.
- **Terminator:** `ret` at 0x00505D26; byte after is `0x90` padding.
- **Compile:** `i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra` → exit **0**, **0 warnings**.
- **Object shape:** 11 `E8` calls + 1 `ret`, 0 `jmp` — matches retail's 11 calls + 1 `ret` exactly.
- **Linkable:** `scripts/check_native_linkable.py` → **linkable (9 undefined, all defined in tree)**,
  measured against `build-win32-no-verify-gate/CMakeFiles/igi.dir/objects.a` (age 0.00 days).
- **Undefined-symbol names (9):** `DynObj_GetByte_4C56F0`, `DynObj_GetByte_4C5800`,
  `QTask_GetCurrentType`, `QTask_LinkQTaskEventHandler`, `QTask_LinkQTaskEventHandlerNoContextFn`,
  `QTask_PhaseIdGet548176_4CEA10`, `QTask_PhaseIdGet548177_4CEA20`, `TaskType_Register`, `g_a7a7bc`.
- **Manifest extractor:** `provenance_va` → `0x505c90`; `candidate_symbols` → `['AIGraph_InitStep_505C90']`.

### Findings worth keeping

1. **Stack accounting closes at 20 pushes / 20 released.** `add esp,0x20` (0x505CB9) = 8 dwords
   (2 ignored to 0x4018F0 + 3 for 0x401900 + 3 for 0x401530); each of four `add esp,0xC` = 3 dwords.
   2+3+3 + 4×3 = 20. No slack.
2. **Fourth argument of `TaskType_Register` is 0, and it is a deferred push.** 0x401900 reads four
   slots; the fourth is the *second* of the two pushes made for the argument-less 0x4018F0. This is
   the same shape as 0x0052CA70 and 0x00503D60 — now a fourth instance of the deferred-push rule.
3. **Four more deferred handlers.** 0x004E0890 (0x505CBC), 0x004879E0 (0x505CD6), 0x00487E20
   (0x505CF1) and 0x00487D90 (0x505D0C) are each pushed *before* an argument-less byte getter and
   consumed as the **handler of the following 0x00401400 call**. None is dead.
4. **Five address-taken-only code addresses, all deliberately undeclared:** 0x00488700, 0x004E0890,
   0x004879E0, 0x00487E20, 0x00487D90 — `call_index()` finds no `E8` to any of them. Note 0x00488700
   **is** already transcribed (`core/native_488700.cpp`, `Task_WalkAndDispatch_488700`), but retail
   pushes it as a plain code immediate and `core/native_464260.cpp` passes it as a cast integer too,
   so it is a cast integer here and **not** a symbol reference.
5. **Partial-register writes preserved.** All four byte getters are `mov al,[global]; ret` and write
   **only AL**, so each event word is `(previous_call_return & 0xFFFFFF00) | byte`. The previous call
   is 0x00401530 for the first event word and the preceding 0x00401400 for the other three. Only the
   low byte of an event word reaches the handler-table index (0x00401400 masks it), so this
   restoration changes no handler slot — recorded as fidelity, not as a behavioural claim.
6. **`mov cx,[...]` / `mov dx,[...]` are 16-bit reads that do not disturb EAX.** At 0x505CE0 and
   0x505CFB the class id is read into CX/DX, leaving the event word in EAX intact for the `push eax`
   that follows. Preserved as separate reads rather than hoisted into one local.
7. **String 0x0054E76C = `Container`** (byte-identical). `0x00A7A7BC` is BSS zero-fill and is
   **defined by** `sys/native_505d40.cpp`, so this file declares it `extern`.

### Open frontier from this unit

0x004E0890, 0x004879E0, 0x00487E20, 0x00487D90 are untranscribed handler bodies (address-taken only).

---

## ⚠ NEW TRAP — `call_index()` is NOT a sound called-vs-address-taken oracle

Found while measuring **0x51B400**. `scripts/native_fidelity_audit.py` `call_index()` decodes the
whole `.text` in **one linear capstone sweep**. That sweep **desyncs**, and addresses inside real
code stop being instruction boundaries in it.

Measured on the 0x51B400 body:

| site | retail instruction | boundary in the `call_index()` sweep? |
|---|---|---|
| 0x0051B444 | `call 0x004EC060` | **no** |
| 0x0051B45F | `call 0x004EC050` | **no** |
| 0x0051B47A | `call 0x004EC040` | **no** |
| 0x0051B452 | `call 0x00401400` | **no** (site missed; target found from other sites) |

Consequence: `call_index()` reports **0x004EC040 / 0x004EC050 / 0x004EC060 as address-taken-only
when they are in fact called**. That is a **false negative** — the dangerous direction, because a
real call edge misread as "address-taken only" gets a cast integer and the link edge is silently
hidden. The method note's trap #2 says "use `call_index()`, never a raw `E8` scan"; that is still
right about *false positives*, but `call_index()` alone is not sufficient.

### The sound procedure

1. **`raw_e8_hits(A) == 0` is CONCLUSIVE that A is address-taken only.** A real call is an `E8`
   byte, so if no `E8` byte anywhere computes target A, A is never called. (A raw scan over-reports
   *targets*, but over-reporting cannot hide a real call.)
2. **`raw_e8_hits(A) > 0`** means "possibly called"; confirm by decoding at real boundaries.

For step 2 a **resync-capable sweep** was used (`/tmp/is2.py`, analysis-only, nothing added to the
tree): candidate entries = every raw `E8`/`E9` target plus every VA named by a provenance line; the
linear sweep **discards any instruction that spans a candidate entry and resumes at that entry**.
This yields **3251 distinct call targets vs 1729 from `call_index()`** — 1522 targets the old index
misses.

**Validation:** the resyncing index reproduces all eleven previously-landed called/address-taken
conclusions exactly — 0x00508E00/0x00508E30/0x00508E40/0x0052CAD0/0x0052CAF0/0x00503DF0/
0x00503F20/0x00504250/0x004D9970/0x004EDD20 address-taken only (**0 raw E8 hits each**), and
0x004D3710 called (13 raw hits). So the six landed units are unaffected; only units measured with
the old index alone are at risk.

**Recommendation to the Lead:** `initstep-transcription-method.md` §2 should be amended — the
`raw_e8_hits == 0` test is the conclusive one, and `call_index()` is a fast positive filter, not a
decision procedure.

---

## 0x51B400 — `AnimSound_Open` (candidate name) — 45 instructions

- **File:** `engine/src/retail/core/native_51b400.cpp`
- **Extent:** 0x0051B400-0x0051B4A6, 167 bytes (0xA7). ndisasm **45** instructions, capstone 45,
  byte accounting **closes** (45 over all 167 bytes). List says 45 — agrees. `ret` at 0x0051B4A6.
- **Compile:** exit **0**, **0 warnings**.
- **Object shape:** 11 `E8` calls + 1 `ret`, 0 `jmp` — matches retail's 11 calls + 1 `ret`.
- **Linkable:** **NOT LINKABLE** — 3 undefined in tree: `Sub_4EC040`, `Sub_4EC050`, `Sub_4EC060`.
  Measured against `build-win32-no-verify-gate/CMakeFiles/igi.dir/objects.a`.
- **Undefined-symbol names (9):** `AnimSound_LoadParameters`, `QTask_GetCurrentType`,
  `QTask_LinkQTaskEventHandler`, `QTask_LinkQTaskEventHandlerNoContextFn`, `Script_QHashNodeRegister`,
  `Sub_4EC040`, `Sub_4EC050`, `Sub_4EC060`, `TaskType_Register`. Only the three `Sub_4EC0xx` are
  undefined *in the tree*; the other six are defined in the archive.
- **Manifest extractor:** `provenance_va` → `0x51b400`; `candidate_symbols` → `['AIGraph_InitStep_51B400']`.

### Findings worth keeping

1. **Stack accounting closes at 22 pushes / 22 released.** `add esp,0x2C` (0x51B43C) = 11;
   `add esp,0xC` at 0x51B457 and 0x51B472 = 3 each; the final `add esp,0x14` (0x51B4A3) = 5, and it
   **folds the third link's three pushes together with the two pushes of 0x004B8810** — there is no
   separate cleanup after the third 0x00401400 call. 11+3+3+5 = 22.
2. **0x004C1780 (`AnimSound_LoadParameters`) is both a handler and a real call edge.** It is pushed
   as a handler at 0x51B416 and 0x51B42F *and* called from 162 sites, so it is **declared and
   referenced as a symbol**, not a cast integer — same treatment as `QTask_NoOpHandler` 0x004D3710
   in `core/native_508d70.cpp`.
3. **The 0x0051B4B0 / 0x0051B4E0 / 0x0051B4F0 handlers have zero raw `E8` hits** → conclusively
   address-taken only → cast integers, deliberately undeclared.
4. **The three `0x004EC0xx` are called and NOT transcribed**, so they are declared and reported
   undefined. They are trivial 6-byte `mov al,[global]; ret` getters (0x004EC060 → 0x0054DB82,
   0x004EC050 → 0x0054DB81, 0x004EC040 → 0x0054DB80). **Transcribing those three 6-byte natives
   would make this unit linkable** — flagged to the Lead, not done because they are outside my 50.

### Open frontier from this unit

0x0051B4B0, 0x0051B4E0, 0x0051B4F0 (address-taken only, untranscribed);
0x004EC040, 0x004EC050, 0x004EC060 (called, untranscribed, 6 bytes each).

---

## The three `0x4EC0xx` getters — LANDED

`engine/src/retail/dynobj/native_4ec040.cpp`, `native_4ec050.cpp`, `native_4ec060.cpp`.
6 bytes each, 2 instructions each, byte accounting closes, compile exit 0 / 0 warnings, all three
linkable with 0 undefined. Bodies `mov al,byte [0x0054DB80/81/82]; ret`. `Sub_4EC040/50/60` were
the only possible names because `core/native_51b400.cpp` already declared them so.
**Unit 2 (`0x51B400`) re-measured as linkable** once these three are in the archive:
`linkable (9 undefined, all defined in tree)`.
Folder `dynobj/` justified by evidence, not taste: each global has exactly five reference sites
image-wide and all fifteen lie inside the AnimTrigger band 0x4EBE00-0x004EC140. A 6-byte body
cannot confirm a semantic identity, so the address-suffixed names are kept.

---

## VERIFICATION LANE — `igire_verify.py`

Analysis helper at `/tmp/igire_verify.py` (analysis-only; `scripts/` is the Lead's to own — adopt it
if wanted). Four checks: (1) signature agreement, (2) string agreement, (3) call-graph agreement,
(4) names.tsv vs IGI-RE ORIG cross-check.

### Extent recovery is self-validated, because `recover()` is not trustworthy

The Lead measured `[esp+0x20154]` inside `0x0052AD10`, which has no `sub esp` at all - impossible
for a single function - so `native_fidelity_audit.py recover()` was returning a **mixture** of
instructions. `igire_verify.py` therefore recovers its own extent (worklist CFG traversal, both
edges of conditional jumps, bounded to the routine window) and then **asserts the instructions TILE
the extent with no gap and no overlap** and that the extent ends on a terminator. If tiling fails it
reports `EXTENT RELIABLE: False` and **skips** signature checking rather than printing a number
derived from a mixture.

### Arity rule (the Lead's bulk attempt gave 32 false disagreements)

`depth = 0` at entry; `push` +4; `sub esp,N` +N; `add esp,N` -N; `pop` -4. `[esp+K]` is argument
`((K - depth - 4)/4 + 1)` only when `K - depth - 4 >= 0` and divisible by 4; with an ebp frame
`[ebp+K]` is argument `((K-8)/4+1)` for `K >= 8`. **Operand reads are evaluated before the
instruction's own esp effect**, or `push [esp+K]` is off by one slot. Capstone note: `X86_REG_ESP`
is **23** and `X86_REG_EBP` is **24** - using 4/5 silently yields arity 0 for every function, which
is how the first run produced ten bogus agreements.

### Gate result — all ten landed files CONFIRMED

| file | retail extent | insns | tiling | sig | strings | calls | IGI-RE name |
|---|---|---|---|---|---|---|---|
| `core/native_505c90.cpp` | 151 B | 42 | OK | AGREE 0 | AGREE 1 | AGREE 8 | Container_Init |
| `core/native_51b400.cpp` | 167 B | 45 | OK | no ORIG | AGREE 2 | AGREE 8 | AnimSound_Open |
| `dynobj/native_4ec040.cpp` | 6 B | 2 | OK | AGREE 0 | AGREE 0 | AGREE 0 | AnimTrigger_GetFireEvent |
| `dynobj/native_4ec050.cpp` | 6 B | 2 | OK | AGREE 0 | AGREE 0 | AGREE 0 | AnimTrigger_GetSizeEvent |
| `dynobj/native_4ec060.cpp` | 6 B | 2 | OK | AGREE 0 | AGREE 0 | AGREE 0 | AnimTrigger_GetParseEvent |
| `core/native_4f8340.cpp` | 928 B | 276 | OK | AGREE 0 | AGREE 1 | AGREE 14 | TerrainLightMap_Init |
| `core/native_505f60.cpp` | 730 B | 211 | OK | AGREE 0 | AGREE 1 | AGREE 15 | TextureModifier_Init |
| `core/native_52c0c0.cpp` | 453 B | 125 | OK | AGREE 0 | AGREE 1 | AGREE 14 | DiscardTerrain_Init |
| `core/native_4b47e0.cpp` | 50 B | 20 | OK | AGREE 2 | AGREE 0 | AGREE 1 | Random_Seed |

The verifier confirms every file we believe correct, so it is fit to judge converted output.
It also **reproduced the Lead's three hand-verified arities exactly**: `0x4E5360` 0, `0x4B47E0` 2,
`0x4F1A80` 6 - agreement on independently-measured cases, not bulk agreement.

### DEFECT 1 — `0x401900` reads FOUR argument slots, IGI-RE ORIG declares FIVE

Hand-verified from the prologue trace. At depth 4 (after `push ebp`) `[esp+8]` -> arg1 and
`[esp+0xC]` -> arg2; after `push esi; push edi` (depth 12) `[esp+0x18]` -> arg3 and `[esp+0x1C]` ->
arg4. Mapped back to entry offsets the body reads **exactly `[esp+4]`, `[esp+8]`, `[esp+0xC]`,
`[esp+0x10]`** and **never reads `[esp+0x14]`**. IGI-RE ORIG:
`(const char *name, int size, uint16_t parent, int pool_count, int unk)` - five params.
The tree's `TaskType_Register` has four and **matches the retail reads**. IGI-RE's own body for
`0x51B400` calls `TaskType_New("AnimSound", 4, Task_GetRootType(), 0, 0)` with five, which matches
the retail *call site* (five pushes at 0x51B400-0x51B411). So IGI-RE's signature describes the
call-site convention and its 5th parameter is never read. **Adopting it verbatim would add a
parameter retail does not read.** Report, do not fix.

### DEFECT 2 — our tree's name for `0x4C1780` is contradicted by both external sources

Retail bytes at `0x004C1780` are `c3 90 90 90 90 90 90 90` - a **bare `ret`**, nothing else.
names.tsv says `Debug_Log` and IGI-RE ORIG says `ORIG(0x4C1780, void, Debug_Log, ...)` - they
**agree**. Our tree calls it `AnimSound_LoadParameters` (`include/igi/retail/core/native_4c1780.hpp`).
A bare `ret` cannot load parameters; a no-op stub pushed as a handler at slots 0 and 2 of many task
types is exactly what a `Debug_Log` hook looks like. Rename candidate - **not renamed**, and my
`native_51b400.cpp` references it under the tree's current name, so a rename must update that
reference in the same change.

### DEFECT 3 — 3165 of 5384 IGI-RE functions have a body and a name but NO ORIG signature

`ORIG` covers 2276 VAs; `HOOK` (name + body) covers 5384; the difference is **3165 functions the
converter can get a name and body for but no signature from ORIG**. `0x51B400` is one: its body is
at `src/nodereg/animsound.c`, its name is in names.tsv, and it has no ORIG entry. The converter must
take those signatures from the C body (or from retail reads), not from ORIG.

### CHECK 4 bulk — 8 name disagreements in 2224 shared VAs

Six are trailing-underscore weak-symbol variants and harmless (`Quat_SlerpPrepare_`,
`DynCube_GetRoot_`, `Coll_Descend_`, `Mtp_FindChunk_`, `VirModel_InitAllLods_`, and one more).
Three are substantive and need a decision:
- `0x4AF990` names.tsv `List_InsertBefore` vs ORIG `List_Insert`
- `0x4F2040` names.tsv `Weapon_GetEvent7` vs ORIG `Weapon_GetEvent7b`
- `0x523D40` names.tsv `Lightmap_BuildLod` vs ORIG `BuildLod`

### Positive validation of the converter's source material

IGI-RE's body for `0x51B400` agrees with retail on **everything measurable**: 11 calls in the same
order, 8 distinct targets, handler slots 0 and 2 then the three getters, handler addresses
`0x51B4B0`/`0x51B4E0`/`0x51B4F0` (which names.tsv independently calls `AnimSound_Parse` /
`AnimSound_GetSize` / `AnimSound_Fire`, matching IGI-RE's own names), both string literals
byte-identical (`"AnimSound"`, `"TASKTYPE_ANIMSOUND"`), the getter order Parse/Size/Fire, and the
two trailing zero arguments. Its `gAnimSoundType` is `0xA83764` - the same BSS word this lane
defines. That is one function; it is evidence the conversion is sound, not proof for the set.

---

## VERIFICATION LANE — first converted output and two self-caught false positives

### `engine/src/retail/model/native_4ed6b0.cpp` (`LODSettings_Init`, 0x4ED6B0) — PASSES all four checks

Extent 0x4ED6B0-0x4EDA65, 950 B, 264 instructions, tiling OK, ends on `ret`. Signature AGREE (0
slots, IGI-RE ORIG 0, source 0). Strings AGREE (3 distinct, byte-identical). Call set AGREE (10
distinct targets). **Ordered call sequence AGREE - all 49 calls in identical evaluation order.**
Name consistent between names.tsv and ORIG.

### Two false positives caught in my own verifier before reporting them

Both were wrong accusations against the converter. Recording them because they are the same class
of error as the `call_index()` and `recover()` defects - a confident, specific, wrong answer.

1. **Declarations are not calls.** The first run flagged **9 phantom extra edges** on
   `native_4ed6b0.cpp` (0x4EDA70, 0x4EDAA0, 0x4EDB20, 0x4EDB30, 0x4EDB60, 0x4EDBD0, 0x4EDBF0,
   0x4EDC00, 0x4EDC50). They are the converter's block of `void NATIVE_DECL Sibling (Args);`
   **forward declarations** for the siblings in the same IGI-RE source file. Fix: brace-match the
   target function's body and scan only inside it.
2. **Textual order is not evaluation order.** The first ordered check then reported "order differs"
   on every file, because in `TaskType_Register("X", 4, Task_GetRootType(), 0)` the outer name
   appears first textually but is called **last**. Fix: compute real evaluation order - per
   statement, sort call sites by nesting depth descending, then by position (innermost first).
   With that, all seven files agree: 49, 11, 11, 51, 41, 28, 1 calls.

### `0x4EDB60` — the converter's open warning is settled, and the TREE is wrong

Retail `0x4EDB60` spans 0x4EDB60-0x4EDBC2 (39 instructions, tiling OK) and contains **exactly one
stack read in the entire extent**: `0x4edb62 depth=8 mov edi,dword [esp+0xc]` -> arg1. At depth 8,
`[esp+0xC]` is entry `[esp+4]`. So retail takes **one** argument, and names.tsv calls it
`ModelLODSettings_Changed` - agreeing with IGI-RE's 1-parameter signature.

Our tree declares **two**: `render/lod_settings.cpp:37`
`LODSettingsRecord* LODSettings_LoadParameters(void* context, LODSettingsLookup lookup)` and
`include/igi/retail/render/lod_settings.hpp:40`. One parameter is never read. Not touched - it is
an existing tree file owned by the Lead.

### Limits of the verifier, stated plainly

The four checks are **structural**: right VA, name, signature arity, byte-identical strings,
complete call edge set in the right order. They do **not** verify body **values** - argument values
at each call site, immediates, arithmetic, field offsets. A converted body can pass all four and
still compute the wrong thing. "Structurally faithful" is not "behaviourally equivalent", and
adopted bodies remain second-hand per the adoption ruling. The next increment is per-call-site
argument-constant comparison, which is what would catch a wrong literal.

---

## VALUE-LEVEL CHECKS ADDED — checks 5 and 6

### CHECK 5 — global / data-address agreement

Retail data addresses referenced by the extent vs the addresses encoded in the source's global
names (`g_a83764`, `g_54db80`). **A retail address is also treated as covered when it is a 4-aligned
field offset < 0x40 inside a declared global**, and those cases are reported explicitly.

That refinement exists because the first run accused the converter of 4 missing globals
(`0xA5EFAC`, `0xA5EFB4`, `0xA5EFBC`, `0xA5EFC4`). They are **fields** of `ParamRange g_a5efa8;`
- retail writes the struct's second word at +4 and the source never names the field address.
**False positive of mine, not a converter defect.** With the refinement: `native_4ed6b0.cpp`
13 retail data addresses vs 9 source globals -> AGREE (4 covered as fields).

### CHECK 6 — argument constants per call site

This is the check that catches a **wrong literal** — a wrong task size, parent type or handler
slot compiles, links, and passes every structural check.

**The first implementation was unsound and I did not report its output.** It took "all pushes
since the previous call" as the argument list and produced false mismatches on files verified by
hand, e.g. `0x505CDB -> 0x4C5800 arg1: retail 0x4879E0 vs source 0x4E0890`. Cause: that window
**mis-attributes deferred pushes** — the central trap of this lane. It was rebuilt on a **stack
model**:

| instruction | model effect |
|---|---|
| `push v` | push v |
| `call f` | the last `arity(f)` entries ARE f's arguments, reversed; pop them, leaving anything below as a deferred argument for a later call |
| `add esp,N` | discard N/4 entries |
| `sub esp,N` | push N/4 unknown slots (locals) |
| `pop` | discard one |

`arity(f)` comes from **f's own prologue**, measured by the validated rule, cached per target. If a
callee's arity is unknown the call is reported **unattributable** rather than guessed.

A second false positive followed: I parsed calls from the **raw** file, so provenance prose became
call sites - `151 bytes (0x97)` parsed as a call named `bytes` with argument `0x97`, and
`(reports)`, `(THREE)`, `(pushes)` likewise. Fixed by parsing the comment-stripped text.

### Final gate — all eight files pass checks 5 and 6

| file | check 5 | check 6 |
|---|---|---|
| `core/native_505c90.cpp` | AGREE | AGREE (11 sites) |
| `core/native_51b400.cpp` | AGREE | AGREE (11 sites) |
| `dynobj/native_4ec040.cpp` | AGREE | N/A (no calls) |
| `core/native_4f8340.cpp` | AGREE | AGREE (51 sites) |
| `core/native_505f60.cpp` | AGREE | AGREE (41 sites) |
| `core/native_52c0c0.cpp` | AGREE | AGREE (28 sites) |
| `core/native_4b47e0.cpp` | AGREE | AGREE (1 site) |
| **`model/native_4ed6b0.cpp` (converted)** | **AGREE** | **AGREE (49 sites)** |

### Self-caught false positives in this instrument - five so far

1. declarations read as calls (9 phantom edges on the converted file);
2. textual order read as evaluation order (every file reported "order differs");
3. struct field offsets read as missing globals (4 on the converted file);
4. naive push window mis-attributing deferred pushes (false mismatches on hand-verified files);
5. comments parsed as call sites (false mismatches on five files).

Every one was caught by gating the check against files whose correctness was already established.
**None of the five was reported as a finding against the converter.** The gate is the method.

### Still not proven

Even with checks 1-6, a converted body is verified **structurally and at literal-argument level**.
Still unverified: arithmetic and comparisons inside the body, field offsets used by the body's own
memory accesses, control-flow shape, and the order in which side effects are written. Nothing here
is runtime evidence, and adopted bodies remain second-hand per the adoption ruling.
