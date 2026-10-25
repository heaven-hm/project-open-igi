# Registrar structure: the frame is the task object (verified 2026-10-25)

Status: structural finding, meaning of individual fields still unproven. Recorded because it
turns six independent transcriptions into one constrained layout, and because it is checkable.

## The finding

In every transcribed task registrar, the stack frame the routine allocates equals the object
size it passes to `TaskType_Register`:

| registrar | frame | object size passed | equal |
| --- | --- | --- | --- |
| `0x00488BB0` LevelFlow | `0x128` | `0x128` | yes |
| `0x00488800` EditVariable | `0xE0` | `0xE0` | yes |
| `0x00488620` SubLevel | `0x120` | `0x120` | yes |
| `0x00487BE0` GuardGenerator | `0x80` | `0x80` | yes |
| `0x004872C0` ConditionalContainer | `0x114` | `0x114` | yes |
| `0x00485330` StatusMessage | `0x1E0` | `0x1E0` | yes |

Verified by me for `0x00485330` directly in the image, not taken from the report:

```
0x00485330   sub  esp, 0x1e0
0x00485340   push 0x1e0          ; the same value, as the TaskType_Register size argument
0x0048534A   call 0x401900       ; TaskType_Register
```

So the frame is not scratch space: it is the task object being constructed, and the offsets the
registrars record while appending container entries are **field offsets inside that object**,
not arbitrary constants.

## Two independent confirmations

1. The offsets land on fields the registrar's own handlers write. For `0x00485330`, the event-0
   handler at `0x00485570` writes, verified in the image:

   ```
   0x0048558E   mov byte [esi+0x1c4], bl
   0x0048559A   mov byte [esi+0x1c6], bl
   0x004855D4   mov dword [esi+0x1d8], 0x40000000
   ```

   and the registrar's recorded container deltas include exactly `0x1C4`, `0x1C6` and `0x1D8`.

2. The offsets are non-uniform in a way only field offsets explain. StatusMessage's recorded
   deltas are `0x20, 0x38, 0x60, 0xB0, 0xB4, 0x1B4, 0x1C4, 0x1C6, 0x1D8`. Two of them are two
   bytes apart (`0x1C4`, `0x1C6`), which cannot be a stride or an array index.

## What stays unproven

The meaning of any individual field, the meaning of the fourth container argument (plausibly a
field type or size code, since the deltas are offsets), and the meaning of the fifth argument.
Two constants recur and are still only raw stores: argument 2 is the object base in all 25
appends transcribed so far, and argument 5 is `7` in all 25.

`0x00485330` also writes the pair `0` and `0x42C80000` to `0x005C8A30`/`0x005C8A34`, the same
two-value shape `0x004F1CB0` writes to `0x00A76BE8`/`0x00A76BEC`. That is a second independent
witness for a (0.0f, 100.0f)-looking pair, but both sites store raw dwords, so the port keeps
them as `std::uint32_t` and says so rather than typing them `float`. The handler above writes
`0x40000000` (2.0f) into a field, which is consistent with float fields but proves nothing on
its own. A third site, `0x004813E0`, writes `0`, `0x42C80000`, `0`, `0x3E8` to
`0x005C1170`/`74`/`78`/`7C`, adding a 1000 next to the same pair.

## Correction to an earlier over-generalisation

An earlier statement in this project's working notes said the two "inert" pushes before the
parent getter are always zero. That is wrong, and it was caught by transcription rather than by
review. Verified in the image at `0x004801B0`:

```
0x004801B0   push 0
0x004801B2   push 0x20        ; not zero
```

and the same registrar uses `push 0; push 0x60`, `push 0; push 0xA0` and `push 0; push 0x40`
for its other three registrations. The correct statement is "two inert pushes whose second value
is sometimes non-zero and whose meaning is unproven". The values are recorded verbatim, not
normalised to the earlier all-zero shape. This is the second pattern-level correction in this
work, and both came from a worker reading retail bytes instead of applying the pattern it had
already derived.

Also from the same registrar: `0x004801B0` has **no** container and registers **four** task
types in one body (`"Explosion0"`, `"FireBall"`, `"Fragment"`, `"ExplosionSmoke"`), so it
contributes no appends to the counts above. `"Fragment"` has no event-0 link while its siblings
do, and four of its links pass a NULL handler - both are retail facts, verified in the body, not
omissions.

## Why this matters for the goal

It is the first real constraint on the registrar layout, and it is derived from retail bytes
that anyone can re-check. It also means the 25 appends across six registrars are not
independent guesses: they share one layout, and a transcription that gets a delta wrong will
now show up as a mismatch against a handler's field write.
