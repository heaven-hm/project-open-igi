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
