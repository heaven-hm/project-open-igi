# `0x00449950` passes one argument to a two-argument callee

**Status: recorded gap. Not resolved, not guessed.**

## Measured

Retail `0x00449950`:

```
0x00449950  56            push esi
0x00449951  8b742408      mov esi, [esp+8]        ; object
0x00449955  56            push esi                ; arg for 0x4168D0
0x00449956  e875cffcff    call 0x4168d0
0x0044995b  50            push eax                ; THE ONLY arg for 0x4c17c0
0x0044995c  e85f7e0700    call 0x4c17c0
0x00449961  8b4668        mov eax, [esi+0x68]
0x00449964  56            push esi
0x00449965  50            push eax
0x00449966  e8f5db0700    call 0x4c7560
0x0044996b  83c410        add esp, 0x10           ; 4 dwords cleaned
```

Callee `0x004C17C0`:

```
0x004c17c0  56            push esi
0x004c17c1  8b742408      mov esi, [esp+8]        ; arg1 = the list
0x004c17c7  57            push edi
0x004c17ce  8b7c2410      mov edi, [esp+0x10]     ; arg2 = the value to match
0x004c17d5  393a          cmp [edx], edi
```

After `push esi`, `[esp+8]` = arg1. After the additional `push edi`, `[esp+0x10]` = arg2. So
**the callee takes two arguments and reads both.**

## The contradiction

At `0x0044995C` only **one** push precedes the call. Stack arithmetic at the callee entry
(0x00449950 frame base `E`):

| step | esp | slot |
|---|---|---|
| entry of `0x00449950` | E | [E+4] = object |
| `push esi` | E-4 | [E-4] = esi |
| `push esi` | E-8 | [E-8] = object |
| `call 0x4168d0` | E-12 | [E-12] = retaddr1 |
| `push eax` | E-16 | [E-16] = eax |
| `call 0x4c17c0` | E-20 | [E-20] = retaddr2 |

Inside `0x004C17C0`, `push esi` -> E-24, so `[esp+8]` = [E-16] = **eax** (arg1, correct).
Then `push edi` -> E-28, so `[esp+0x10]` = [E-12] = **retaddr1**, the return address of the
`call 0x4168d0`.

So the value the callee reads as arg2 at this call site is a **return address**, not an argument.

## Why this is left open

Two readings exist and the bytes do not choose between them:

1. Retail genuinely reads stale stack here (a latent retail bug), and the faithful transcription
   must reproduce a garbage comparison.
2. `0x00449950` calls a different routine than the one at `0x004C17C0`, which the address
   disproves - the call target is `0x004c17c0` byte for byte.

flow-analyst reports that the callee's other 36 call sites push twice (e.g. `0x0042E968`:
`mov eax,[0x56E210]; push esi; push eax`). If that holds, this call site is the outlier.

**What is not permitted is choosing an argument value.** Writing `0` or a cast would invent a
comparison retail does not perform. The declaration therefore stays one-argument and the
undefined symbol stays in the frontier until the call-site census is complete and the outlier is
understood.

## To close it

Census every `E8` to `0x004C17C0` and record, per site, the number of pushes before the call.
If exactly this one site passes fewer arguments, the finding is a retail quirk to be reproduced
deliberately and documented at the site, not silently normalised.
