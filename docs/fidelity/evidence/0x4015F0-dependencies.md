# 0x004015F0 destructor: direct dependencies (extracted from the committed raw listing)

Source: docs/fidelity/evidence/0x4015F0-retail-listing.txt (radare2 6.2.0, retail igi.exe SHA256 dea34892d4...).

Direct call targets inside the listing:

| target | count | what it is in this tree |
| --- | --- | --- |
| `0x004015F0` | 2 | itself - the destructor recurses per child |
| `0x004B0D10` | 1 | memory deallocation entry (already transcribed as a mem native) |
| `0x004AF7B0` | 1 | `Debug_Error`, the same callee the event allocator's exhaustion path uses |
| `0x004015D0` | 1 | the unlink native transcribed in round 175 |

The listing also contains calls whose target is a register (the phase-3 and phase-2 handler invocations taken from the
handler table); those are not textual call targets and are the parts that need the block-by-block transcription.

Why this matters: every direct dependency of the destructor is already present in the tree. It is therefore not blocked
on new natives - it is blocked only on the transcription effort itself, which is why it can be scheduled as one
self-contained increment rather than a chain.

## CORRECTION (round 189, accepted from §18)

This note was built from a `pd 200` listing that extends 148 bytes past the function, so two of its five entries are
not the destructor's:
  "second self-call"  -> 0x4017A4, inside the NEXT function 0x401780
  "Debug_Error 0x004AF7B0" -> 0x4017E0, inside QTask_AllocateEvent 0x4017C0
Real inventory inside 0x4015F0: THREE direct callees - self at 0x40165F, the unlink native 0x4015D0 at 0x4016C5, and
the free entry 0x4B0D10 at 0x40176A - plus two indirect handler calls at 0x40164F and 0x401682. The conclusion that no
new dependency is needed still holds, but for three callees, not five.

The instruction table generated alongside this note was also withdrawn: it had 88 rows for a 391-byte function and its
"mean 4.44 bytes/instruction" check was circular (391/88 by construction). The complete decode is 134 instructions
tiling 0x4015F0-0x401776, confirmed by r2 pdj and NASM ndisasm agreeing on every address and byte string; it lives in
evidence sheet §16.2.2 and §18 maps onto it 1:1.
