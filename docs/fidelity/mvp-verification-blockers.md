# What blocks each MVP native from end-to-end verification

Generated from the level-5 paired capture (`build/runtime-traces/mvp-frida-pair-level5.json`),
the manifest and the record audit. The machine-readable table is
[`mvp-verification-blockers.json`](mvp-verification-blockers.json); this file explains the
categories and what each one needs.

| blocker | natives | meaning |
| --- | --- | --- |
| candidate-unreachable | 69 | retail calls it, the candidate program never does: the subsystem is not built into the scene |
| unlinked | 20 | the port has no symbol at all for the native |
| retail-unobserved | 18 | neither run at this level reached it |
| end-to-end | 8 | both sides observed; 6 pass every pillar, 2 need a runner case |
| paired-mismatch | 5 | both sides observed, observed classes disagree |

Level 1 and level 5 produce the same candidate picture - 18 linked natives observed,
8 matched - so the shortfall is structural, not a matter of which level is loaded.

## The flow fact that frames everything

Retail `main` (0x0048F530) itself calls only 7 of the 120 MVP natives:
`QTask_OpenEvent`, `AppContext_SetIsActive`, `AppContext_GetIsActive`,
`AppContext_SetTerrainLightmapsUsed`, `Display_ClearSurfaces_16`, `sprintf` and
`Error_Show`. The candidate observes three of those; the other four are startup calls
the MVP transcription skips. So "1:1 from main to the end of the frame" cannot be
established by transcribing main alone: 113 of the 120 natives sit below it in
subsystems the candidate does not run.

## Order of work implied by the table

1. `paired-mismatch` (5) - each is a concrete behavioural difference against a known
   retail body, so each is a real bug with a fix and a re-capture. `GOInRemap
   0x004037C0` is the clearest: retail returns the destination buffer it fills, the
   candidate returns a masked argument.
2. `unlinked` (20) - link the already-ported units into the executable (five IFF
   natives and `AnimIff_LoadAnimationFile` exist under other names without the
   `extern "C"` binding; `humanplayer_defines.cpp` is not in the root CMake list).
3. `candidate-unreachable` (69) - porting work in subsystem order: script/QVM, then
   resource, then animation/human/weapon.
4. `retail-unobserved` (18) - widen the retail run or record them as not exercised at
   MVP levels.

## Decoded contract for the first mismatch: GOInRemap 0x004037C0

Read from the full retail body (161 bytes, `0x004037C0..0x00403861`), not from the
current implementation comment, which understates it. The retail signature is
`uint32_t* __cdecl GOInRemap(void* destination, void* frame)` - two stack pointers,
not a single QVM context:

1. `count = [0x00BC2380]`.
2. `action = [0x00BC2358 + count * 345 * 4]` (stride 345 dwords), `slot = (count - 1) * 115 + action`,
   `entry = 0x00BC25BC + slot * 12` (three dwords).
3. `entry[0] = ReadArgument(frame, 0)`, `entry[1] = ReadArgument(frame, 1)`,
   `entry[2] = ReadArgument(frame, 2) & 0x3FFFFFFF`, through retail 0x004B8A20.
4. `[0x00BC2358 + count * 345 * 4] += 1` - the increment lands on the same slot the read
   came from, not on the remap row.
5. A 24-byte (six dword) record is copied to `destination` with `rep movsd`. Four of its
   dwords are written first: three zeros and the constant `0x00567C74`; two dwords are
   never written, so the retail copies whatever those stack slots held.
6. Returns `destination`, not a masked argument.

Two consequences for the port: the signature and the caller convention have to change
(the QVM dispatcher passes three script arguments today), and the record copy cannot be
bit-exact because two of its dwords are uninitialized on the retail side - the port must
either define them and document the divergence or the comparison must scope them out.
That makes this a transcription task with a scope decision, not a one-line repair.
