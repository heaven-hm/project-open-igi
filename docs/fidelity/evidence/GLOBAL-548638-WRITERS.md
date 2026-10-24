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
