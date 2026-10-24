# `g_548638` (0x00548638) - who writes the global that blocks three bodies

The leaf batch flagged `g_548638` as the single unresolved symbol preventing
`0x004E5370` (id 2), `0x004DA1D0` (id 3) and dependent bodies from linking.
Retail xref recovery (`r2 aaa; axt 0x548638`) finds exactly these references:

| site | access | instruction |
| --- | --- | --- |
| `0x00412523` | read | `mov cx, word [0x548638]` |
| `0x0048EEC8` | read | `mov ax, word [0x548638]` |
| `0x004D977C` | **write** | `mov word [0x548638], ax` |
| `0x004D9790` | read | `mov ax, word [0x548638]` |
| `0x004D979E` | **write** | `mov word [0x548638], 0x181` |
| `0x004DA1D0` | read | `mov ax, word [0x548638]` |
| `0x004E5370` | read | `mov ax, word [0x548638]` |

So it is a 16-bit "current parent/type id" used by class registrations
(`0x4DA1D0`, `0x4E5370` read it and pass it as `TaskType_Register`'s parent
argument), and it is written in two places in one function region:

- `0x004D9730` starts with five calls to `0x401860` with `(4,0), (5,0), (6,0),
  (7,0)` then `0x4017C0(0)` and later writes the global at `0x004D977C`;
- the same region writes the **constant `0x181`** at `0x004D979E`, which is the
  same value seen as `QTask`'s third registration argument in the captured
  sequence (id 0, arg3 `0x181`).

Reading: the constant `0x181` initialisation is the "current parent" default, and
`QTask` registers with that parent - consistent with `0x181` being the root/task
type discriminator rather than a pointer. This is a hypothesis supported by the
two observations; it is **not** verified, and it does not yet justify writing a
definition of the global.

## What is still needed before the three bodies can link

1. Transcribe the function that owns `0x004D977C`/`0x004D979E` (extent to be
   confirmed: `0x4D9730` start is known from the xref function, the `0x5315B0`
   attribution from r2 looks like a naming artefact and must be resolved by a
   boundary check).
2. Define `g_548638` in a retail globals unit **only** as part of that writer's
   transcription, with the retail VA in the comment - not as a standalone
   declaration invented to satisfy the linker.
3. Re-check the three bodies after the writer lands; until then they stay blocked.

## Boundary resolved, and the global's meaning identified

r2 function info: `fcn.004d9730` is **83 bytes** (`0x004D9730-0x004D9782`, 1 block),
and the next function `fcn.004d9790` is **65 bytes** (`0x004D9790-0x004D97D1`,
1 block). So `0x4D977C` belongs to `0x4D9730` and `0x4D979E` belongs to
`0x4D9790`; the earlier `0x5315B0` attribution was an r2 `af` artefact and is
withdrawn.

Full body of `0x004D9730` (fn 83 B, no branches):

```
push (4,0); call 0x401860      ; phase handler 4
push (5,0); call 0x401860      ; phase handler 5
push (6,0); call 0x401860      ; phase handler 6
push (7,0); call 0x401860      ; phase handler 7
push 0;     call 0x4017C0      ; some no-arg call with 0
add esp, 0x24
mov byte [0x54863A], al        ; stores the 0x4017C0 result byte
push (0,0); call 0x4018F0      ; QTask_GetCurrentType()
push eax
push 0x50
push "QObj"                    ; 0x0054863C
call 0x401900                  ; TaskType_Register
add esp, 0x14
mov word [0x548638], ax        ; <-- g_548638 := returned id
```

**`g_548638` is the `QObj` class id**, not a parent/type discriminator: it is the
value returned when `"QObj"` (size `0x50`) is registered, stored immediately after
the call. That corrects the hypothesis in the previous section. It also explains
why `0x4DA1D0`/`0x4E5370` read it: they pass the QObj class id as the **parent**
argument to their own registrations, i.e. QObj is the base class of those objects.

The `0x181` write in the neighbouring function `0x004D9790` (65 B) is therefore a
separate object (a different class id) and must not be conflated with QObj.

## What that means for blocking

`0x004E5370` and `0x004DA1D0` can link as soon as `0x4D9730` is transcribed and
defines `g_548638` with its retail VA - and that transcription is a single
straight-line 83-byte body whose callees are `0x401860`, `0x4017C0`, `0x4018F0`
and `0x401900`. Dependency check needed before writing: whether `0x401860` and
`0x4017C0` exist in `engine/` (the latter apparently does, `0x4018F0`/`0x401900`
do), and what `"QObj"`'s exact bytes are (`0x0054863C`, NUL-terminated).

## Dependency check for landing 0x4D9730

- `0x4017C0` **exists**: `QTask_AllocateEvent` in `engine/src/retail/core/task_types.cpp:511`
  (also referenced by `native_401810.cpp`).
- `0x4018F0`, `0x401900` exist (`QTask_GetCurrentType`, `TaskType_Register`).
- **`0x401860` does not exist** in `engine/` - only mentioned in comments
  (`task_types.cpp:585` says the retail's QTask handler links run through `0x401860`
  and `0x401890`, which the port does not use). So `0x4D9730` cannot link until
  `0x401860` is transcribed first.

`0x401860` is 41 bytes (`0x00401860-0x00401888`), single block, no callees:

```
mov eax, [arg_8h]          ; dword parameter
mov dx,  [arg_4h]          ; word parameter
and eax, 0xFF
mov cl, 1
lea eax, [eax + eax*2]
shl eax, 1                 ; eax = (arg_8 & 0xFF) * 6
mov byte [eax + 0x54F932], cl   ; used flag  := 1
mov word [eax + 0x54F930], dx   ; word value := arg_4
mov byte [eax + 0x54F933], cl   ; second flag := 1
ret
```

Reading: it claims the next free event-table slot for `(word value, table index)`
- i.e. the "mark event id used and store its argument" writer that
`QTask_AllocateEvent` scans for. Both the stride (6) and the three field offsets
(`+0`, `+2`, `+3`) match the event table described at `task_types.hpp:58-63`.

Landing order therefore becomes: `0x401860` (41 B, no callees) -> `0x4D9730` (83 B)
-> unblock `0x4E5370` (id 2) and `0x4DA1D0` (id 3). Blocker to check before writing
`0x401860`: the event table at `0x0054F930` needs a single engine definition with
its retail VA (the header currently documents it but no symbol is defined there).

`"QObj"` bytes confirmed at `0x0054863C` (`51 4F 62 6A 00`), size `0x50`.

## Round 34: frozen pair for current tree; possible TaskType_Register arity mismatch

Frozen and deployed: `build/artifacts/igi1-bc2861e1.exe` + receipt, sha256
`bc2861e1747ce8b0d4cca2b86c08a823515035a35921cd86b147f97e63d7ed19`, deployed copy
verified identical. Source and deployed binary agree again after the 0x401860
commit.

### New discrepancy, must be resolved before transcribing 0x4D9730

The 0x4D9730 site pushes **five** dwords before `TaskType_Register`:

```
0x4D9763 push 0
0x4D9765 push 0
0x4D9767 call 0x4018F0      ; declared no-argument QTask_GetCurrentType()
0x4D976C push eax
0x4D976D push 0x50
0x4D976F push "QObj"
0x4D9774 call 0x401900
0x4D9779 add esp, 0x14      ; 20 bytes = 5 arguments
```

`add esp,0x14` is retail-owned evidence that this call consumed 5 arguments,
whereas `engine/include/igi/retail/core/task_types.hpp:129` declares
`TaskType_Register(const char* name, std::uint32_t object_size, ...)` with four
parameters (16 bytes). Two possibilities, both need evidence, neither is assumed:

1. `TaskType_Register` really takes a fifth argument (for example a flag or parent
   count) and the engine declaration/transcription is missing it - that would be a
   fidelity defect affecting every existing registrar call site, not just this one.
2. The two zero pushes belong to a different callee that the listing hides (for
   example a `__fastcall`/`__stdcall` variant of 0x4018F0 with different cleanup),
   in which case the extra dword is not a TaskType_Register parameter.

`QTask_GetCurrentType()` in the header is declared with no parameters, which argues
against reading the two pushes as its arguments. Until this is settled from the
decompile of 0x4D9730 (BN) plus the 0x401900 body's parameter reads, writing
0x4D9730 would either invent an argument or silently drop one. Blocked, not guessed.

## Round 35: half of the arity question settled (getter is argument-free)

`0x004018F0` is a **7-byte leaf, no prologue, no argument reads**:

```
0x004018F0  66 a1 28 f9 54 00   mov ax, word [0x54F928]
0x004018F6  c3                  ret
```

So `QTask_GetCurrentType()`'s engine declaration (no parameters, returns the 16-bit
current type at `0x0054F928`) is **correct**, and the two `push 0; push 0` before
calling it cannot be its arguments - the callee never touches the stack. Those two
dwords therefore stay on the stack and are consumed by the following `0x401900`
call, which matches `add esp,0x14` (5 dwords).

`0x00401900` body facts so far (no frame pointer; direct `[esp+...]` reads):

- `[esp+0x8]` -> `eax` at `0x0040192B` = **arg1** (name pointer)
- `[esp+0xC]` -> `ecx` at `0x00401927` = **arg2** (object size)
- after `push esi; push edi`, `[esp+0x18]` = old `[esp+0x10]` = **arg3** (parent)

Remaining question: whether the body reads **arg4 and arg5** further in, i.e.
whether the site's five pushes are all meaningful or the 5th is a cdecl extra that
the callee ignores. Next evidence needed: the remainder of `0x00401900`
(`0x00401943-0x00401A10`) listing every `[esp+N]` read, plus its stored fields in
the 0x30-byte slot at `0x00AF5EE0`.

Practical consequence for the next transcription: `0x4D9730` must reproduce the
retail's five pushes **regardless** of how many the engine declaration exposes -
dropping a push because "the signature has four parameters" would change the stack
layout at that call site, and adding a fifth parameter without evidence would
change the ABI. The body waits for the read-set to be established.

## Round 36: arity settled - TaskType_Register takes FOUR arguments

Full stack-read enumeration of `0x00401900` body (after the two `push esi/push edi`
at `0x0040192F/0x00401937`, so the window's `[esp+0x1c]` equals the caller's
`[esp+0x14]` = arg4):

| caller slot | callee read | meaning |
| --- | --- | --- |
| `[esp+0x8]` | `[esp+0x8]` -> eax | arg1 name |
| `[esp+0xC]` | `[esp+0xC]` -> ecx | arg2 size |
| `[esp+0x10]` | `[esp+0x18]` (post-push) | arg3 parent |
| `[esp+0x14]` | `[esp+0x1c]` (post-push) | arg4 |

**No fifth read exists.** So the API is 4-parameter, `engine/include/igi/retail/core/task_types.hpp:129`
is correct, and every existing registrar call site is fine. The five pushes at
`0x004D9730` (cleanup `add esp,0x14`) include one **extra cdecl dword the callee
never reads** - a retail stack artefact, not a fifth parameter.

### Consequences for the 0x4D9730 transcription

1. The body may call the 4-parameter `TaskType_Register`; passing a fifth argument
   is impossible in C++ and would change the declaration/ABI.
2. Fidelity note required in the provenance block: retail pushes a stray extra
   dword at this site (5 pushes, 20-byte cleanup) while the callee reads only four;
   the transcription calls the same API with the same four meaningful arguments.
   The extra dword is unobservable to the callee, so this is a **documented benign
   divergence in stack traffic only**, not in behaviour.
3. The two `push 0` sequence is therefore "one stray dword + arg4 = 0", not two
   meaningful arguments - and the QObj registration still receives parent from
   `0x4018F0` (arg3) and 0 (arg4), matching what the capture implies for QObj.

## Round 40: 0x4D9790 decoded (next candidate, dependencies being checked)

65-byte single-block body `0x004D9790-0x004D97D0`:

```
mov ax, word [0x548638]      ; read the QObj class id
push eax; call 0x401A20      ; some call taking the id
push 7
mov word [0x548638], 0x181   ; <-- overwrites the QObj id with the root type 0x181
call 0x4018C0                ; per-phase call, phase 7
push 4; call 0x4018C0        ; phase 4
push 5; call 0x4018C0        ; phase 5
push 6; call 0x4018C0        ; phase 6
mov cl, byte [0x54863A]
push ecx; call 0x4018C0      ; one more call with the remembered byte
add esp, 0x18; ret
```

Reading: this is the counterpart of `0x4D9730` - it consumes the QObj id it just
registered, then **restores the current-type global to the root value `0x181`**
and re-points the phase handlers 4/5/6/7 plus one extra slot at `0x4018C0`. That
is consistent with QObj being a temporary "current class" during registration and
`0x181` being the restored root type; it also confirms the earlier hypothesis
about `0x181` as the root discriminator, now with a concrete write site.

Blockers before transcribing: the callees `0x401A20` and `0x4018C0` are not
transcribed (`engine/` has no match), so this body cannot link yet and is recorded
rather than written. Both are small leaf-like helpers in the 0x4018xx/0x401A2x
task-type range and are the natural next bottom-of-tree targets.

## Round 41: 0x401A20 decoded (task-type slot teardown) - one callee still missing

50-byte cdecl function `0x00401A20-0x00401A51`, 2 blocks:

```
eax = arg & 0xFFFF
esi = eax * 3; esi <<= 4              ; idx * 0x30 -> task-type slot at 0x00AF5EE0
if (slot[0x24] != 0)                   ; 0x00AF5F04 flag byte
    call 0x004B0B70(&slot[0x0C]);      ; 0x00AF5EEC payload
slot[0x25] = 0;                        ; 0x00AF5F05 in-use byte cleared
return eax (eax semantics: low byte was overwritten by the flag read)
```

So it is the **task-type slot teardown**: conditionally frees/deletes the slot's
payload through `0x004B0B70`, then clears the in-use byte that
`TaskType_Register`'s scan at `0x00AF5F05` looks for. This explains why
`0x4D9790` calls it with the QObj id right before restoring the root type: the QObj
slot is being released, not merely re-pointed.

Dependency: `0x004B0B70` is **not** in `engine/` (nor is `0x004B0B00`, the task-list
build path already recorded as untranscribed). So `0x401A20` cannot link yet and is
recorded rather than written - the same discipline as `0x4D9790`.

Return-value caveat worth preserving: `mov al, byte [esi+0xAF5F04]` overwrites the
low byte of the `arg & 0xFFFF` value, so the returned `eax` is not simply the input
id. The transcription must reproduce the retail instruction order rather than
"return the argument", and the return type should stay unverified until a caller is
seen consuming it.

## Round 42: 0x4B0B70 decoded - reference-clearing destructor shim

24-byte cdecl `0x004B0B70-0x004B0B87`, 1 block, one callee:

```
push esi
esi = [esp+8]          ; arg1 = pointer to a pointer
eax = [esi]            ; load the referenced value
push eax; call 0x004B0D10
add esp, 4
[esi] = 0              ; clear the caller's reference
pop esi; ret
```

So it is "destroy the referenced object through `0x004B0D10`, then null the
caller's pointer" - exactly the operation `0x401A20` performs on a task-type slot's
payload at `slot+0x0C`. Not a helper to be inlined: it is a distinct retail routine
and must be its own transcription, and it cannot be written until `0x004B0D10`
(42 bytes) exists.

Dependency chain now visible and all small:
`0x4D9790` -> `0x401A20` -> `0x4B0B70` -> `0x4B0D10` (42 B, not yet decoded).
`0x4B0B00` (task-list build path) remains a separate untranscribed branch chosen by
`FlashTask`'s non-zero fourth registration argument; it is not in this chain.

Status: three decoded bodies (`0x4D9790`, `0x401A20`, `0x4B0B70`) wait on
`0x4B0D10`; recorded in dependency order so the next round starts at the leaf.

## Round 43: 0x4B0D10 decoded - two further callees, one engine global

42-byte cdecl `0x004B0D10-0x004B0D39`, 1 block:

```
eax = arg
edx = dword [0x00942300]
eax = (arg - 4) & 0xFFFFFFFC      ; add eax,-4; and al,0xFC  (only the low two bits are masked)
edx -= 1; dword [0x00942300] = edx
esi = dword [eax]
call 0x004AF960(esi)
call 0x004A6A50(esi)
add esp, 8
ret
```

Reading: it decrements a 32-bit counter at `0x00942300`, derives a pointer by
rounding `arg-4` down to a 4-byte boundary and dereferences it, then calls two
functions with that value. The masking quirk (`and al,0xFC` only) is recorded so a
transcription does not "tidy" it into a full-address mask - retail masks the low
byte only, and the high bits of `arg-4` pass through unchanged.

New blockers: `0x004AF960` and `0x004A6A50` are not transcribed, and the counter
global `0x00942300` needs a definition with its retail VA. Chain extends to:

`0x4D9790` -> `0x401A20` -> `0x4B0B70` -> `0x4B0D10` -> {`0x4AF960`, `0x4A6A50`} + `0x00942300`

This is the point where the "small leaf" strategy stops being cheap: the subtree
under the registrar path is deep, and each leaf introduces two more. That is the
honest state of the denominator, and it is why 113 of 123 ids remain unlanded.

## Round 45: candidate comparison for 0x4B0D10 - rejected, masking defect found

The catalogued candidate `scripts/GameNatives/sys/004B0D10-Mem_DeAllocFn 2.cpp`
(13 lines, "CFG score 100.0% structural only") reads:

```c
int* data_val = context - 4;
data_val &= 0xfc;          // <-- masks a POINTER to 0xFC
g_942300 -= 1;
int* source_val = *data_val;
return Camera_UpdateTransform_9(HeapList_Remove(source_val));
```

Against the retail listing (`and al, 0xFC` masks only the **low byte** of
`(arg - 4)`, leaving the high 24 bits intact), `data_val &= 0xfc` is a **semantic
defect**: it reduces the address to at most 0xFC instead of rounding it to a 4-byte
boundary. It also binds `HeapList_Remove`'s result into a further call, whereas
retail calls `0x004AF960` and `0x004A6A50` with the *same* `esi` value in sequence.

So the candidate is **rejected for reuse**; a fresh transcription from the retail
instructions is required (this is precisely why candidates need re-checking and why
a structural CFG score cannot be accepted as behaviour). The correct shape is:

```
value = (arg - 4) & ~0x3 only in the low byte   // and al,0xFC
counter 0x00942300 -= 1                          // order preserved
esi = *(uint32_t*)value
HeapList_Remove(esi)                             // 0x004AF960, already in engine/mem
0x004A6A50(esi)                                  // still untranscribed
```

Status: still blocked on `0x004A6A50`; `0x004AF960` is confirmed present as
`HeapList_Remove`. The catalogued name `Camera_UpdateTransform_9` for 0x4A6A50 is a
name-project artifact and must not be adopted without evidence.
