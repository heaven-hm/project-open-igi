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
