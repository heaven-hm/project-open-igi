# Startup chain `0x00402930` — transcription gate report for the next direct callees

- Author: `chain-worker` (delegated by `lead`), 2026-10-25 (Asia/Calcutta).
- Artifact: retail `igi.exe`, image base `0x00400000`,
  SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`
  (`shasum -a 256 /Users/haseeb-mir/Downloads/IGI/igi.exe` re-run for this report; matches the manifest).
- Tools/evidence: radare2 6.2.0 function records (`afl` export, 3254 functions); linear disassembly of
  `.text` in `build/retail-dis.txt` (wrapped instruction continuation lines merged); direct reads of the
  PE raw bytes for the argument-role spot checks below; `docs/fidelity/native-verification.json` as the
  single owner of what is implemented. **No decompiler was used** (Binary Ninja is not available to this
  worker); per AGENTS §10.2 the decompiler leg of the evidence chain is therefore absent for every
  function named here, and nothing below claims more than what the bytes + manifest show.
- Result: **0 of the 6 assigned candidates is landable; no code file was written.** All six push at
  least one code address (a handler/callback) that has no implementation in the manifest, so the
  task's gate rule (a) blocks all six. Their direct `call` targets are all implemented (rule (b) passes).

## 1. The gate that was applied

For each candidate, in this order:

1. **(a) Code-address immediates.** Does the body push a code address (`push $0x4xxxxx`, where the value
   lies in `.text` = `0x401000`–`0x53250F`) that the body hands to another routine as a callback? If yes,
   that handler is a dependency of the candidate, and the candidate is blocked until the handler exists
   as a transcribed native.
2. **(b) Direct call targets.** Are all of the body's direct `call` targets implemented per the manifest
   (or inside the CRT ranges `0x401000`–`0x402000`, `0x4A5000`–`0x4AB000`, `0x4AF000`–`0x4B0000`)?

Both rules must pass. Rule (b) passes for all six; rule (a) fails for all six.

The six candidates are single-block, straight-line `cdecl` registrars with no stack arguments. They are
called from the startup sequence with **zero arguments** (no `push` precedes the calls, and no `add esp`
follows them; verified in the listings below), and they perform deferred cdecl cleanup of all argument
slots they push. Every one of them follows the same shape:

```
TaskType_Register(class-name, object-size, parent-type, task-list-count, <unread 5th dword>)
store returned class id in a class-id word
QTask_LinkQTaskEventHandler_401400(class id, event id, handler)        // optional
QTask_LinkQTaskEventHandlerNoContextFn_401530(class id, event id, handler)   // 2..3 times
<class-specific side effects>
```

`TaskType_Register` at `0x00401900` is `TRANSCRIBED`; `0x00401400`/`0x00401530` are
`STRUCTURALLY_VERIFIED` in `engine/src/retail/core/task_types.cpp`. The four `0x004018F0`
(`QTask_GetCurrentType`, `STRUCTURALLY_VERIFIED`) and `0x004E7530` (`Core_GetEventWord_4E7530`,
`STRUCTURALLY_VERIFIED`) links are the same calls the tree's `QTask_InitPhaseHandlers`/`Flow_Init`-style
natives already make.

Cross-check against the runtime registration capture recorded in
`docs/fidelity/evidence/RETAIL-REGISTRATION-BASELINE.md`: every pushed object size and every `parent`
argument agrees with the captured records (`IntroScreen` 56 = `0x38`, `GameScreen` 40 = `0x28`,
`KnifeStab` 32 = `0x20`, `ComputerObjectives` 56 = `0x38`, `ComputerLog` 36 = `0x24`, `Cursor` 64 = `0x40`;
`parent` 0 for the five that pass `QTask_GetCurrentType()` while the current type is still 0, and
`parent = word[0x0054E200]` for `KnifeStab`, captured as 0x58). That corroborates the argument mapping
used below; it does not make any handler landable.

## 2. Candidates at a glance

Extents are the measured instruction stream from the candidate's entry through its `ret` (the trailing
alignment `nop` bytes are excluded); the counts agree with the radare2 records (`nbbs` = 1, `edges` = 0
for all six). Startup call sites are the `call` instruction address; the next instruction address is the
one listed in `RETAIL-REGISTRATION-BASELINE.md`.

| Candidate | Extent through `ret` | Instr | Stack frame | Startup call site | Missing code immediates (blockers) |
|---|---|---|---|---|---|
| `0x0046DF10` | `0x0046DF10`–`0x0046DF60` (81 B) | 22 | 0x14 | `0x00402A11` | `0x0046DF70` |
| `0x00415070` | `0x00415070`–`0x004150DD` (110 B) | 30 | 0x40 | `0x0040296C` | `0x004150E0`, `0x00415380`, `0x00415450` |
| `0x004688B0` | `0x004688B0`–`0x0046891E` (111 B) | 31 | 0x38 | `0x00402A4D` | `0x00468920`, `0x00468BC0` |
| `0x00468E20` | `0x00468E20`–`0x00468E8E` (111 B) | 31 | 0x38 | `0x00402A48` | `0x00468E90`, `0x00469110` |
| `0x004851B0` | `0x004851B0`–`0x0048521E` (111 B) | 31 | 0x38 | `0x00402980` | `0x00485220`, `0x00424280` |
| `0x00424B90` | `0x00424B90`–`0x00424C08` (121 B) | 33 | 0x38 | `0x00402BC9` | `0x00424C10`, `0x00424C40`, `0x00424CA0` (and `0x00424CE0` has no engine implementation, §6.4) |

Verdict for all six: **BLOCKED by rule (a)**. The pushed code immediates are not stray data — each one is
the handler argument of a `QTask_LinkQTaskEventHandler*` or `WinProc_RegisterHandler` call in the same
body, i.e. exactly the callback the registrar exists to install.

## 3. Per-candidate detail

### 3.1 `0x0046DF10` — registers `"KnifeStab"` (class 168)

Startup call: `0x00402A11`. Class-name string `0x0053FE9C` = `"KnifeStab"`; class-id word `0x0053FE84`
(image bytes `81 01 00 00`, i.e. word `0x0181`, the linker's "unregistered/root" sentinel; overwritten with
the returned id); hash-name string `0x0053FE88` = `"TASKTYPE_KNIFESTAB"`.

```
0046df10  push $0x0
0046df12  push $0x0
0046df14  call 0x4f45f0              ; Sys_GetGlobal16_4F45F0 -> word[0x54E200] (runtime parent id)
0046df19  push %eax                  ; parent
0046df1a  push $0x20                 ; object size 32
0046df1c  push $0x53fe9c             ; "KnifeStab"
0046df21  call 0x401900              ; TaskType_Register(name, size, parent, task_list, unread 0)
0046df26  add $0x14,%esp             ; 5 pushed dwords
0046df29  mov %ax,0x53fe84           ; store class id
0046df2f  push $0x46df70             ; HANDLER -> MISSING
0046df34  call 0x413b20              ; Core_GetFlagByte_413B20 -> byte[0x5391AE] (event id)
0046df39  push %eax                  ; event id (linker arg 2)
0046df3a  mov 0x53fe84,%ax           ; AX = class id
0046df40  push %eax                  ; class id (linker arg 1)
0046df41  call 0x401400              ; QTask_LinkQTaskEventHandler
0046df46  mov 0x53fe84,%ecx
0046df4c  and $0xffff,%ecx
0046df52  push %ecx                  ; class id
0046df53  push $0x53fe88             ; "TASKTYPE_KNIFESTAB"
0046df58  call 0x4b8810              ; Script_QHashNodeRegister(name, value)
0046df5d  add $0x14,%esp
0046df60  ret
```

Direct calls: `0x004F45F0`, `0x00401900`, `0x00413B20`, `0x00401400`, `0x004B8810` — all implemented
(`STRUCTURALLY_VERIFIED`, except `0x00401900` = `TRANSCRIBED`). Rule (b): pass.
Code immediate: `0x0046DF70` — **not in the manifest**, and not an `afl` function start either.
Blocker: handler `0x0046DF70` (see §4.1).

Runtime note: the handler argument 2 is `byte[0x005391AE]`, not the image byte `0xFF`. Startup itself
allocates the event ids: `0x00402A11` is preceded by `0x004029AD call 0x413DF0`, and `0x00413DF0` runs
`push $0x0; call 0x4017C0` seven times, storing the returned ids at `0x005391AC`…`0x005391B2`; the
`0x00413B20` getter returns the byte at `0x005391AE`, the third of those ids. `0x00413DF0` is itself a
direct startup callee (`0x004029AD`) and is **not in the manifest**, so at runtime the tree's
`g_413b20` would still be `0xFF` until `0x00413DF0` is transcribed. This does not change the static
transcription of `0x0046DF10`, but it does mean the phase/event argument must be captured at runtime
before the handler link can be claimed as matching.

### 3.2 `0x00415070` — registers `"IntroScreen"` (class 130)

Startup call: `0x0040296C`. Class-name string `0x005394CC` = `"IntroScreen"`; class-id word `0x0057B198`
(zero-filled at load); alias key `0x005394A4` = `"INTROSCREEN:"`; alias target `0x005394B4` =
`"LOCAL:screens/intro/"`.

```
00415070  push $0x0
00415072  push $0x0
00415074  call 0x4018f0              ; QTask_GetCurrentType
00415079  push %eax                  ; parent
0041507a  push $0x38                 ; object size 56
0041507c  push $0x5394cc             ; "IntroScreen"
00415081  call 0x401900
00415086  push $0x4150e0             ; HANDLER -> MISSING
0041508b  push $0x0                  ; event id 0
0041508d  push %eax                  ; class id
0041508e  mov %ax,0x57b198
00415094  call 0x401400
00415099  mov 0x57b198,%ax
0041509f  push $0x415380             ; HANDLER -> MISSING
004150a4  push $0x1                  ; event id 1
004150a6  push %eax
004150a7  call 0x401530
004150ac  mov 0x57b198,%cx
004150b3  push $0x415450             ; HANDLER -> MISSING
004150b8  push $0x2                  ; event id 2
004150ba  push %ecx
004150bb  call 0x401530
004150c0  push $0x5394b4             ; "LOCAL:screens/intro/"
004150c5  push $0x5394a4             ; "INTROSCREEN:"
004150ca  call 0x4b12f0              ; QFile_RegisterAlias(key, target)
004150cf  add $0x40,%esp
004150d2  push $0x5394b4
004150d7  call 0x4e68d0              ; manifest name "SoundLoad" (one pointer argument)
004150dc  pop %ecx                   ; cdecl cleanup of the one argument
004150dd  ret
```

Direct calls: `0x004018F0`, `0x00401900`, `0x00401400`, `0x00401530` (×3), `0x004B12F0`, `0x004E68D0` —
all present in the manifest. Rule (b): pass on paper. But `0x004E68D0` is manifest-`VERIFIED` only
against an external JSON catalogue (`/Users/haseeb-mir/Documents/Code/project-igi-natives/…/IGI-Natives.json`),
there is no engine implementation of it (§6.4), so this candidate cannot link as written even after the
handlers exist.
Code immediates: `0x004150E0`, `0x00415380`, `0x00415450` — none in the manifest.
Blocker: those three handlers (`0x004150E0` in addition pushes a fourth missing code address
`0x00415170`; see §4.2).

### 3.3 `0x004688B0` — registers `"ComputerLog"` (class 180)

Startup call: `0x00402A4D`. Class-name string `0x0053F6E4`; class-id word `0x005BDC98` (zero-filled);
the fourth link's event id is `Core_GetEventWord_4E7530()` (`0x005488C8`, image `0xFF`, owned by
`native_4e7530.cpp` as `g_5488c8`).

```
004688b0  push $0x0 / push $0x0
004688b4  call 0x4018f0              ; current type = parent
004688b9  push %eax / push $0x24 / push $0x53f6e4  ; size 36, "ComputerLog"
004688c1  call 0x401900
004688c6  push $0x468920             ; HANDLER -> MISSING
004688cb  push $0x0 / push %eax      ; class id, event 0
004688ce  mov %ax,0x5bdc98
004688d4  call 0x401400
004688d9  mov 0x5bdc98,%ax
004688df  push $0x488700             ; implemented (Task_WalkAndDispatch_488700)
004688e4  push $0x1 / push %eax
004688e7  call 0x401530
004688ec  mov 0x5bdc98,%cx
004688f3  push $0x468bb0             ; implemented (Task_ClearPendingWord_468BB0)
004688f8  push $0x2 / push %ecx
004688fb  call 0x401530
00468900  add $0x38,%esp
00468903  push $0x468bc0             ; HANDLER -> MISSING
00468908  call 0x4e7530              ; Core_GetEventWord_4E7530
0046890d  mov 0x5bdc98,%dx
00468914  push %eax / push %edx
00468916  call 0x401530              ; (class id, event word, handler)
0046891b  add $0xc,%esp
0046891e  ret
```

Direct calls: `0x004018F0`, `0x00401900`, `0x00401400`, `0x00401530` (×3), `0x004E7530` — all implemented.
Code immediates: `0x00468920` (**missing**), `0x00488700` (implemented), `0x00468BB0` (implemented),
`0x00468BC0` (**missing**).
Blocker: `0x00468920`, `0x00468BC0`.

### 3.4 `0x00468E20` — registers `"ComputerObjectives"` (class 179)

Startup call: `0x00402A48`. Identical shape to §3.3 with size `0x38`, name `"ComputerObjectives"`
(`0x0053F700`), class-id word `0x005BDD70`, handler set
`0x00468E90` (**missing**), `0x00488700` (implemented), `0x004D3710` (implemented,
`QTask_NoOpHandler`), `0x00469110` (**missing**). The second link is registered for event id 1, the
third for event id 2, the fourth for `Core_GetEventWord_4E7530()`. Blocker: `0x00468E90`, `0x00469110`.

### 3.5 `0x004851B0` — registers `"GameScreen"` (class 134)

Startup call: `0x00402980`. Same shape: size `0x28`, name `"GameScreen"` (`0x00540A80`), class-id word
`0x005C8A28`; handlers `0x00485220` (**missing**), `0x00488700` (implemented), `0x004D3710`
(implemented), `0x00424280` (**missing**, event id = `Core_GetEventWord_4E7530()`).
Blocker: `0x00485220`, `0x00424280`.

### 3.6 `0x00424B90` — registers `"Cursor"` (class 258)

Startup call: `0x00402BC9`. Size `0x40`, name `"Cursor"` (`0x0053B9F8`), class-id word `0x0057BC60`.
Handler set: `0x00424C10` (**missing**), `0x00424CE0` (manifest-`VERIFIED`, but the implementation is
`scripts/GameNatives/00424ce0-Cursor_RunHandler.cpp`, not an engine native — §6.4), `0x004D3710`
(implemented), `0x00424C40` (**missing**), and a WinProc registration of `0x00424CA0` (**missing**):

```
00424bfb  push $0x424ca0             ; WinProc handler -> MISSING
00424c00  call 0x48f490              ; WinProc_RegisterHandler(handler)
00424c05  add $0x10,%esp             ; cleans 3 leftover 0x401530 args + this handler
```

Blocker: `0x00424C10`, `0x00424C40`, `0x00424CA0`, plus an engine implementation of `0x00424CE0`.

## 4. The missing handlers (dependency closure)

None of the 14 handler addresses pushed by the six registrars is an `afl` function start (radare2's
recursive analysis never carved them, because they are only referenced as immediates). Their extents
below are therefore recovered by walking the instruction stream from the pushed address to the first
`ret`, requiring the alignment `nop` pad to end at the next known entry; every one is confirmed by a
clean `ret` + `nop` pad (except `0x00468E90` and `0x00469110`, whose successor starts immediately after
`ret`). Rule (b) was re-applied to each handler body.

| Handler | Pushed by | Extent through `ret` | Instr | Calls not implemented | Code immediates not implemented | Landable now? |
|---|---|---|---|---|---|---|
| `0x0046DF70` | `0x0046DF10` | `0x46DF70`–`0x46DFAA` (59 B) | 23 | `0x0046DFB0` (583 B, 9 blocks — not in manifest) | none | no |
| `0x004150E0` | `0x00415070` | `0x4150E0`–`0x415169` (138 B) | 42 | `0x00491610`, `0x004905A0` | `0x00415170` | no |
| `0x00415380` | `0x00415070` | `0x415380`–`0x41544A` (203 B) | 71 | `0x00490D10`, `0x004903B0`, `0x004905A0` | `0x00415170` | no |
| `0x00415450` | `0x00415070` | `0x415450`–`0x415474` (37 B) | 16 | `0x004903B0` | none | no |
| `0x00468920` | `0x004688B0` | `0x468920`–`0x468AFE` (479 B) | 152 | `0x00505600`, `0x0046B060`, `0x0046C0B0`, `0x00418E40`, `0x0046B970`, `0x0046B660` | `0x00468B00`, `0x00468B30` | no |
| `0x00468BC0` | `0x004688B0` | `0x468BC0`–`0x468D94` (469 B) | 145 | `0x004B4910`, `0x00418E40`, `0x00469450`, `0x004B5730` | none | no |
| `0x00468E90` | `0x00468E20` | `0x468E90`–`0x46902F` (416 B) | 137 | `0x00505600`, `0x0046B060`, `0x0046C0B0`, `0x00418E40`, `0x0046B970`, `0x0046B660`, `0x0046B500` | `0x00469030`, `0x00469060` | no |
| `0x00469110` | `0x00468E20` | `0x469110`–`0x46941F` (784 B) | 251 | `0x004B4910`, `0x0046C0B0`, `0x00467590`, `0x0046B7D0`, `0x00418E40`, `0x00469450`, `0x004B64E0` | none | no |
| `0x00485220` | `0x004851B0` | `0x485220`–`0x4852EB` (204 B) | 60 | `0x00416D40`, `0x00416920`, `0x004E8110`, `0x004E9430`, `0x004E8370`, `0x004E9500`, `0x004E9460`, `0x00487160`, `0x004E7AC0` | none | no |
| `0x00424280` | `0x004851B0` | `0x424280`–`0x4242F4` (117 B) | 40 | none (`0x004E7530` only) | none | **yes** |
| `0x00424C10` | `0x00424B90` | `0x424C10`–`0x424C3E` (47 B) | 12 | none | none | **yes** |
| `0x00424C40` | `0x00424B90` | `0x424C40`–`0x424C93` (84 B) | 30 | `0x004B4E00` (317 B, 74 instr, not in manifest) | none | no |
| `0x00424CA0` | `0x00424B90` | `0x424CA0`–`0x424CD1` (50 B) | 13 | none (`0x0048F130` only) | none | **yes** |
| `0x00424CE0` | `0x00424B90` | `0x424CE0`–`0x424E03` (292 B) | 100 | none missing per manifest (`0x0048F130`, `0x00491CF0`) | none | only via a scripts-only candidate (§6.4) |

### 4.1 The smallest chain: `0x0046DF10` → `0x0046DF70` → `0x0046DFB0`

The KnifeStab handler `0x0046DF70` (23 instructions) is a thin wrapper: it reads entry argument 2
(`mov 0x14(%esp),%edi` after `push ecx/esi/edi`, i.e. `[entry+8]`), stores the float bit pattern
`0xBE860A92` in a local, then retries `0x0046DFB0` up to three times (adding the float at
`0x00533658` to the local per attempt) and returns; `0x0046DFB0` is its only call target.
`0x0046DFB0` is an `afl` function (583 bytes, 154 instructions, 9 blocks) that is **not in the manifest**;
its body allocates a `0x108`-byte frame and calls at least `0x004B3790` (missing), `0x00416830`
(implemented, `Sys_GetSettingByte_416830`), `0x00416920` (missing) and `0x004CC2A0` (missing) before its
end. That dependency set was not fully enumerated here (it is not one of the
assigned candidates), so the KnifeStab registrar is at least two handler layers away from landable.

### 4.2 Handlers that are closer

- `0x00424C10` (12 instructions, no calls, no code immediates): reads `[entry+4]` and `[entry+8]`, stores
  argument 2 at `[arg1+0x20]`, then writes the constants `0x140` (`+0x24`), `0xF0` (`+0x28`),
  `0xFFFFFFFE` (`+0x2C`) and zeroes `+0x30`, `+0x34`, `+0x38`, `+0x3C`. It is a clean 2-argument
  `void f(void* object, void* value)` handler with no external dependencies.
- `0x00424CA0` (13 instructions): WinProc-shaped; compares `[entry+8]` with `0x200`
  (`WM_MOUSEMOVE`), calls `0x0048F130` (`Display_GetScreenMode`, `RUNTIME_TESTED`, owned by
  `engine/src/retail/core/app_startup_globals.cpp`), and on success writes the split `lParam`
  (`[entry+0x10]`) as two sign-extended 16-bit values to `0x0057BC58`/`0x0057BC5C`; returns 1.
- `0x00424280` (40 instructions): reads `[entry+4]` (the per-frame dispatch object), walks the
  `[object+8]` list, pushes/removes the linked handler ids in `g_afa6e0`/`g_afa7e0` (both owned by
  `engine/src/retail/core/native_4015f0.cpp`; `0x00488700` refers to them `extern`), calls
  `0x004E7530` (`Core_GetEventWord_4E7530`) and then calls the table handler at
  `QTask_HandlerTable[event*0x180 + [object+0x1C]]` with one argument. No missing dependencies.

Those three are the cheapest additions that make progress on this wave, but **on their own they do not
unblock any of the six registrars**: `0x00424B90` still needs `0x00424C40` (→ `0x004B4E00`) and an
engine native for `0x00424CE0`; `0x004851B0` still needs `0x00485220` (nine missing callees).

## 5. ABI / signature uncertainty (recorded as unknown, not guessed)

1. **Handler arity is not one argument.** `engine/include/igi/retail/core/task_types.hpp` declares
   `using FrameHandler = void (*)(void*);`. The pushed handlers' bodies read more than one stack
   argument: `0x0046DF70` reads `[entry+8]`; `0x004150E0` reads `[entry+4]` (object) **and**
   `[entry+8]` (a struct pointer whose `+0`, `+4`, `+8` bytes/words are copied into `+0x20`, `+0x28`,
   `+0x34` of the object); `0x00424C10` reads `[entry+4]` and `[entry+8]`. `0x00424CA0` is a
   four-argument WinProc handler (`[entry+4]` window, `[entry+8]` message, `[entry+0xC]` wParam,
   `[entry+0x10]` lParam), matching the `WindowHandlerEntry::handler` type in
   `engine/include/igi/retail/core/native_0048f970.hpp`. So retail dispatches different phase tables
   with different handler arities; the single `FrameHandler` type cannot express the corpus. This is
   not changed here — it is a Lead decision, and it matters only when the handler natives are written
   (the registrar bodies merely push the address).
2. **`TaskType_Register` callers push five dwords, the callee reads four.** All six push
   `[0, 0]` before the getter and then `[name, size, parent]`, i.e. a trailing `0x0` dword four slots
   above the return address that `0x00401900` never reads (it reads `[E+4]`…`[E+0x10]` and the callers
   clean `0x14` bytes). The tree's four-parameter
   `TaskType_Register(name, object_size, parent_type, task_list_count)` matches the four read arguments;
   the fifth push is codegen/ABI noise and must be preserved only as an unread stack slot. Recorded for
   completeness, low risk.
3. **Pre-existing inconsistency in `QTask_LinkQTaskEventHandler` (`0x00401400`).** This is not one of
   the assigned candidates; it is reported because every one of the six registrars calls it, and the
   tree's implementation disagrees with the retail instruction stream. Retail, byte-verified
   (`8b 6c 24 10` at `0x00401408`):
   - `0x00401408` loads the **second** stack argument into `ebp`; `0x00401413`/`0x00401417` derive the
     phase from `ebp & 0xff` (second argument).
   - `0x00401429`/`0x00401430` reload the **first** argument and mask it to `0xffff`; `0x0040143B`
     computes `index = (arg1 & 0xffff) + (arg2 & 0xff) * 0x180`.
   - `0x0040145F` compares the type record's parent word with the **first** argument's low word;
     `0x00401478`–`0x0040147A` recurse with `(slot, arg2, handler)`.
   The tree body
   (`engine/src/retail/core/task_types.cpp`, `QTask_LinkQTaskEventHandler(event_id, context, handler)`)
   computes `phase = event_id & 0xff` and `index = (event_id & 0xffff) + phase * 0x180`, compares the
   record word against `context`, and recurses with `(slot, event_id, handler)` — i.e. it uses the
   first argument where retail uses the second and vice versa, in all three places. The header comment
   in `task_types.hpp` ("the retail computes the index as class_id + phase * 384 at 0x00401400") is
   consistent with the retail bytes, not with the body. Consequence for this wave: with the call shapes
   the registrars use (arg1 = class id, arg2 = event id 0/1/2), retail indexes
   `class + event*0x180` (e.g. KnifeStab class 168 → `168 + event*0x180`), while the current body
   indexes `class + (class & 0xff)*0x180` for every event, so the three phase links of one class
   collide. The Lead's own Frida capture (`docs/fidelity/evidence/library-vs-game-code-policy.md`,
   call `0x004C59A4` with arguments `(2, 36, 0x004C5A10)`) reads naturally only under the retail
   mapping (class 2, event 36). `0x00401530` (the no-context variant) matches retail for the index and
   the recursion; only its record-parent comparison uses the second argument where retail uses the
   first (`0x00401589` reads the first argument at `[esp+0x14]` after four pushes). **Not modified by
   this worker**: `0x00401400`/`0x00401530` are manifest-`STRUCTURALLY_VERIFIED` tree natives owned by
   the Lead's integration, the change would affect every existing call site, and it is outside the
   assigned scope. It must be reconciled before the registrar wave is claimed as matching.
4. **Runtime values of the class-id/phase slots are not established statically.** The class-id words
   (`0x0053FE84`, `0x0057B198`, `0x005BDC98`, `0x005BDD70`, `0x005C8A28`, `0x0057BC60`) are written by
   these registrars at run time; the image has `0x0181` at `0x0053FE84` and zero-fill/reserved zeros
   elsewhere. `0x005391AE` (event id for KnifeStab) is allocated by the unimplemented `0x00413DF0`
   (§3.1); `0x005488C8` (event word for the Computer*/GameScreen/Cursor fourth links) is owned by
   `engine/src/retail/core/native_4e7530.cpp` as `g_5488c8 = 0xFFU`. No runtime capture of these six
   registration sequences exists in this tree; the phase arguments are therefore unknown values that
   must be observed, not guessed.
5. **No decompiler evidence.** Per §10.2 a decompiler pass (Binary Ninja first) is required for a
   transcription. None was available to this worker; the report above is disassembly-only. This is one
   more reason not to transcribe the six in this wave.

## 6. Ownership and linkability notes

### 6.1 Data referenced by the six candidates

| Address | Content (retail bytes) | Tree owner today |
|---|---|---|
| `0x0053FE9C` | `"KnifeStab"` | none |
| `0x0053FE88` | `"TASKTYPE_KNIFESTAB"` (20 bytes, ends at `0x0053FE9C`) | none |
| `0x0053FE84` | class-id word (image dword `0x00000181`) | none |
| `0x005394A4` | `"INTROSCREEN:"` | none |
| `0x005394B4` | `"LOCAL:screens/intro/"` | none |
| `0x005394CC` | `"IntroScreen"` | none |
| `0x0057B198` | class-id word (zero-fill) | none |
| `0x0053F6E4` | `"ComputerLog"` | none |
| `0x005BDC98` | class-id word (zero-fill) | none |
| `0x0053F700` | `"ComputerObjectives"` | none |
| `0x005BDD70` | class-id word (zero-fill) | none |
| `0x00540A80` | `"GameScreen"` | none |
| `0x005C8A28` | class-id word (zero-fill) | none |
| `0x0053B9F8` | `"Cursor"` | none |
| `0x0057BC60` | class-id word (zero-fill) | none |
| `0x0057BC58`, `0x0057BC5C` | two 32-bit mouse coordinates written by `0x00424CA0` | none |
| `0x0054E200` | KnifeStab parent id word | `engine/src/retail/sys/native_4f45f0.cpp` (`g_54e200`) |
| `0x0054F928` | current task type word | `engine/src/retail/core/task_types.cpp` (`QTask_CurrentType`) |
| `0x005391AE` | event-id byte read by `0x00413B20` | `engine/src/retail/core/native_413b20.cpp` (`g_413b20`) |
| `0x005488C8` | event word read by `0x004E7530` | `engine/src/retail/core/native_4e7530.cpp` (`g_5488c8`) |
| `0x00AFA6E0`, `0x00AFA7E0`, `0x00A96AE0` | handler/dispatch arrays touched by `0x00424280` | `engine/src/retail/core/native_4015f0.cpp`, `task_types.cpp` |

Everything in the first group is unowned, so a future transcription of a registrar would own its own
class-id word and use byte-identical string literals (§10.14 rule 10); the parent/event getters are
called through the existing natives, whose globals are `extern` at the call site. Nothing is added here.

### 6.2 Startup-chain bookkeeping

The `0x00402930` sequence decodes to 155 calls followed by a tail jump to `0x004CEF30` at `0x00402C37`.
At least one of those direct callees is unimplemented: `0x00413DF0` (called at `0x004029AD`; allocates
seven event ids and initialises the `"TASKTYPE_*"` hash entries). Its own callees `0x004F1DB0` and
`0x004B8890`-family entries are likewise partly absent from the manifest. This is consistent with the
Lead's statement that `0x00402930` is not transcribed and that the candidate build fails closed on it;
it is mentioned here because it is a direct dependency of the KnifeStab phase argument (§3.1).

### 6.3 Calls whose "implemented" status is manifest-only

`0x004E68D0` (`"SoundLoad"`) is manifest-`VERIFIED` with an external JSON path and has no engine
implementation anywhere under `engine/`; `0x00415070` cannot link this call even after its handlers
exist. `0x004B63D0`, `0x00491CF0` and `0x00406220` are listed in the manifest against
`scripts/GameNatives/…` paths, which are not part of the engine build; `0x00406220` does have an engine
implementation under a different name (`Config_GetActivePlayerProfileRecord`,
`engine/src/retail/config/config_profile.cpp`), while `0x004B63D0` and `0x00491CF0` do not appear to
(the tree only mentions `Display_GetActiveMode` in a comment in `engine/src/retail/render/view_block.cpp`).
Any future registrar transcription must resolve these symbol-level questions with the Lead rather than
reference scripts-only candidates.

### 6.4 Handlers that must move from reference candidates into engine natives

`0x00424CE0` (`Cursor_RunHandler`) is the only pushed handler with a manifest implementation, and that
implementation is `scripts/GameNatives/00424ce0-Cursor_RunHandler.cpp` — a reference candidate, not an
engine native (confirmed by the manifest path and by the absence of any engine file with that VA). It
also calls `0x00491CF0`, which has no engine implementation (§6.3). Before `0x00424B90` can link, an
engine native for `0x00424CE0` must exist in `engine/src/retail/<domain>/` with a real
`NATIVE_DECL`, which in turn needs `0x00491CF0` resolved.

## 7. Recommended next wave (facts, not assignments)

1. Transcribe the three handler natives that have zero missing dependencies:
   `0x00424C10`, `0x00424CA0` (needs the two new globals `0x0057BC58`/`0x0057BC5C`), `0x00424280`.
   This creates symbols the `Cursor` and `GameScreen` registrars can push, and exercises the
   `FrameHandler` arity question on a small, provable body.
2. Then `0x00424C40` becomes possible only after `0x004B4E00` (317 bytes, 74 instructions, single
   block, an `afl` function that is not in the manifest).
3. For the KnifeStab chain, transcribe `0x0046DFB0` (583 bytes, 9 blocks) before `0x0046DF70`; the
   handler `0x0046DF70` is only reachable after its sole callee exists.
4. The IntroScreen, ComputerLog, ComputerObjectives and GameScreen registrars each remain behind
   5–9 unimplemented callees plus 2–9 unimplemented second-level callbacks (§4); none is a small
   increment.
5. Resolve §5.3 (`0x00401400`/`0x00401530` argument roles) with the Lead **before** any registrar is
   committed, because every registrar's handler links go through those two natives and the phase
   indices depend on the roles.

## 8. What this report does not claim

- It does not claim any of the six is transcribed, verified or behaviorally observed. Nothing was
  transcribed.
- Instruction extents and call lists are from a linear disassembly of the retail `.text` plus raw-byte
  spot checks; the handler extents are not `afl`-confirmed entries (they are not function starts in
  radare2's analysis), so they are recovered bodies, not independently bounded functions.
- No runtime evidence was collected for any address in this report; statements about runtime values
  are explicitly marked unknown.
- No decompiler pass exists for these bodies (§5.5).
