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
