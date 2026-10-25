# Link vs reachability audit, 2026-10-25

Scope: the 23 natives added to the Windows x86 target in this change (22 from the
handler/AIGraph closure plus the surviving `0x004F9E70` implementation), measured against
the candidate built at `build-win32-no-verify-gate/igi1.exe` (14,283,882 bytes, sha256
`8f0de1c9fe0448538613975cf44b6c4be89368c32c71f6d3f34cba2f4a14eeb6`).

Method: `i686-w64-mingw32-objdump -t` for the linked symbol table, `objdump -d` for direct
call sites and tail jumps, `objdump -s -j .rdata`/`-j .data` for 32-bit pointer tables.

## Result

| measure | count |
| --- | --- |
| records rebound to the linked image | 23 |
| symbols resolved in the linked image | 23 |
| statically reachable (call, tail jump or data pointer) | 20 |
| linked but unreferenced anywhere in the image | 3 |

A build proves compilation and linking. It does not prove reachability, and this audit
separates the two.

## Why a call-site count alone was misleading

The first pass counted only direct `call` instructions and reported 17 of 23 as
unreachable. That was wrong. The AIGraph node writer `0x004F9EB0` dispatches through a jump
table — `jmp *0x574ed4(,%ebx,4)` — whose entries are **tail jumps** into the tag writers:

```
421014: jmp    *0x574ed4(,%ebx,4)
42102f: jmp    421490 <_GraphStream_WriteTag9String_4FA730>
42104b: jmp    421540 <_GraphStream_WriteTagABlob_4FA7A0>
421062: jmp    421180 <_GraphStream_WriteTag0Byte_4FA4E0>
```

So the tag writers are reachable by tail jump, not by call. Counting one edge kind and
concluding "unreachable" would have been a false negative of exactly the kind this project
must avoid. The recorded `addressability` block therefore carries `direct_call_sites`,
`tail_jumps` and `data_pointers_in_rdata` separately, not one aggregate.

## The three genuinely unreferenced natives

| retail VA | symbol | retail caller |
| --- | --- | --- |
| `0x004F9EB0` | `GraphStream_AddNode_4F9EB0` | `call 0x4f9eb0` at `0x00509559`, in the function starting `0x00509520` |
| `0x004F9CD0` | `GraphNode_Process_4F9CD0` | `call 0x4f9cd0` at `0x004FAF0F`, `0x004FAF72`, `0x004FAF8C`, `0x004FAFAC`, in the function starting `0x004FAEE0` |
| `0x004663D0` | `Task_DefineComputerStartup` | `call 0x4663d0` at `0x00402A66`, inside the startup sequence `0x00402930` |

None of the three is dead in retail; each has a retail caller that the candidate does not
implement yet, so nothing in the candidate can reach them.

## The single structural blocker

`0x00402930` is the retail startup init sequence: 780 bytes, 156 five-byte instructions,
155 ordered `call`s and one tail `jmp 0x004CEF30`, one basic block. The candidate
implements it in `engine/src/retail/core/class_registration.cpp` as a **fail-closed stub**
that logs and throws `std::logic_error` rather than executing the sequence.

Every transcribed registrar hangs off that sequence. Measured in the linked image:

| symbol | references in the candidate image |
| --- | --- |
| `QTask_Register_4688B0` | 0 |
| `QTask_Register_46B510` | 0 |
| `QTask_Register_468E20` | 0 |

Those registrars were committed as BUILD_VERIFIED in earlier changes and they do compile
and link, but the call chain that reaches them is the stub. This is why the candidate
creates no window: it throws before the registrars run. It also means a green build and a
clean gate can coexist with a candidate that reaches almost none of its transcribed
natives.

Consequence for the goal: the frontier is not the number of transcribed natives, it is the
position of the fail-closed guard inside the 155-call sequence. Playable Level 1 requires
that sequence to be transcribed in retail order, with each callee implemented rather than
skipped, until the chain reaches the display and level-load path.

## Limits of this audit

- Reachability here is static. A native can be reachable statically and never executed, and
  a tail-jump entry does not prove which table slot retail selects at run time.
- Pointer scanning covers `.rdata` and `.data` 32-bit words only; a computed table address
  or a 16-bit offset table would not be seen.
- No runtime capture is claimed for any of these 23 addresses; the honest status ceiling
  remains BUILD_VERIFIED.
