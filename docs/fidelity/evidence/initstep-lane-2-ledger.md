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
