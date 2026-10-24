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
