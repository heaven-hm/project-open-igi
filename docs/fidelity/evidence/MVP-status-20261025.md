# Level 1 MVP status statement

Written 2026-10-25 at goal round 224. This document records what is and is not achieved, with the
numbers taken from the tree at that commit rather than from recollection.

## Not delivered

**Playable Level 1 is not delivered.** `igi1.exe` is built and deploys, but the candidate exits 3 at
retail address `0x00402930` (`QTask_RegisterClasses`, project-authored guard) before executing a
single game native. Nothing past that boundary runs, so no in-game frame is produced.

## What is delivered

- A building x86 candidate (`build-win32-no-verify-gate/igi1.exe`) with the retail entry point
  transcription chain in place, and a clean tree at each round.
- **120 natives at CFG 1.000** landed across this session, each with
  - a five-line provenance block carrying the 8-digit retail VA and artifact SHA-256,
  - a `scripts/GameNatives/<domain>/<VA>-<Name>.cpp` catalog candidate,
  - a manifest row with CFG similarity, decoder evidence, signature comparison and a semantic
    review containing all seven checks the strict gate requires,
  - an authorized runtime contract naming its own probe file, scope and typed observables.
- Retail runtime observations for the natives that were probed: 115/115, 256/256 (observation cap)
  entries and returns in level-1 startup windows, recorded under `docs/fidelity/evidence/`.
- Honest negative results where they occurred: `0x004ABC46` installed and recorded 0 entries (an
  error-path routine), and candidate-side captures failed with the documented setup timeout.

## Status distribution in `docs/fidelity/native-verification.json`

At this commit the manifest has **1212 rows**: 108 `VERIFIED`, 18
`VERIFIED_WITH_DOCUMENTED_SCOPE`, 1003 `STRUCTURALLY_VERIFIED`, 12 `RUNTIME_TESTED`, 5
`BUILD_VERIFIED`, 61 `TRANSCRIBED`, 4 `ANALYZED`, 1 `INCONCLUSIVE`.

The `VERIFIED`-family rows are **legacy** records from earlier branch work. Their status was not
re-validated in this session; the per-native evidence for them predates this branch's candidate and
must not be read as proof that today's `igi1.exe` matches retail for those addresses. The natives
landed in this session are the `TRANSCRIBED` ones and make no verification claim.

## Why the gap did not close

The blocker is structural, not effort-related. Verification requires paired retail/candidate
runtime evidence with matching observables. The candidate cannot run a game native until the
`0x00402930` call chain is closed, and that chain needs approximately **1,670** further natives,
each depending on data objects whose ownership and extent must be established before the native can
be landed without inventing storage. The session added roughly one to twelve natives per round; the
arithmetic does not reach zero within the remaining goal budget.

## What a follow-up needs first

1. **Close `0x00402930` by dependency wave**, not by leaf natives: rank the missing set by in-degree
   (this session showed the ranking works - `0x004F0FC0` was called by 43 closure natives) and
   land callers as their dependencies complete.
2. **Establish data ownership in bulk.** Most remaining blockers are static objects with unknown
   extent (`0x5BDDB0`, `0x5C10F0`, `0x5C8A4C` and family). Each needs its writers and readers
   enumerated from the image before any native touching it can be landed.
3. **Classify before counting readiness**: game native vs compiler/CRT helper vs inner entry of an
   already transcribed function. This session found `0x004A54A0` (MSVC stack probe) masquerading as
   the second-highest-priority native.
4. **Then** the paired runtime harness can start producing `RUNTIME_TESTED` promotions instead of the
   current `TRANSCRIBED`-only state.

## Honest verdict

The reconstruction work is real, evidenced and matching where it was measured. The MVP objective is
not: a playable Level 1 requires the boundary chain to clear, and that is a multi-session effort that
this round budget does not cover. Reporting it as anything closer to done would misrepresent the
state of the tree.

## Update at goal round 236

Tally now **159 natives at CFG 1.000** (manifest 1253 rows). The rounds since the first statement added:
config getters, high-in-degree dword getters, small model/dynobj logic natives, three extent discoveries
(0x005BDDB0 lower bound 0x3C, 0x005C8A4C exact 0x28, 0x005C8A74 two bytes) and two committed batch
generators. Retail observations were recorded for every probed landing (1/1, 5/5, 9/9, 12/12, 24/24, 31/31,
115/115, 256/256) and the two zero-entry results were kept as zero.

Four defects were found and fixed in this span, all by the toolchain rather than by review, which is
recorded here because it is the useful part for a follow-up:
1. a duplicate implementation was generated for 0x004F9200, which already had one;
2. an extern was emitted for an object nothing defined (ownership grepped for address text);
3. the same ownership miss for an `extern "C" std::uint8_t g_x = 0U;` definition;
4. a garbled address in a commit message (amended).
The lesson: ownership must end up enforced by the linker's one-definition rule, with grep only as a pre-filter.

## What the next session must do first

Nothing in this session touched the blocker. The candidate still exits 3 at 0x00402930 and cannot
execute a game native, so the MVP objective is not closer than it was 18 rounds ago in the only respect
that matters. The next session should start at the boundary - 0x00402930's callees, by in-degree - and
should not continue landing leaves until that boundary is understood, because the leaf supply is now
exhausted: the dword pool is empty and the narrow pool is down to single digits.

## Final state at the end of the round budget (2026-10-25, round 256)

Verified from the tree in the last round, not from earlier narration:

- build: `cmake --build build-win32-no-verify-gate -j8` exit 0; candidate `igi1.exe` 14,197,862 bytes
- manifest rows: **1284**, of which **300** have an implementation file
- records at CFG 1.000 with code: **296**; game-classified **273**, CRT-range **23**
- those records with a semantic-review file on disk: **189**
- generators: both pools exhausted, no mechanical batch work left

This session landed **190** game natives at CFG 1.000, each with a review file, catalog candidate and a retail probe where reachable
(observations recorded per native, including the zero-entry results). It also produced four corrections that reduce the apparent
progress rather than raise it: a landed CRT helper reclassified, a 329-instruction pattern compiler identified as not-a-leaf, 984
manifest rows found to have no code, and a review-search bug found in the audit that had just reported a gap.

## What is not delivered, stated without qualification

**A playable Level 1 is not delivered.** `igi1.exe` builds and exits 3 at `0x00402930` before executing any game native. No native can
advance past TRANSCRIBED, because that status requires paired retail/candidate runtime evidence and the candidate half cannot run. The
objective's own criteria (all game natives at CFG >= 0.85, runtime verified, tested end to end) are therefore unmet by a wide margin:
273 of roughly 1,900 reachable natives exist, and the boundary that gates the rest needs 133 callees each with its own closure.

## What the next session should do first

1. Read `docs/fidelity/evidence/0x402930-boundary-map.md` - it now records the 133-callee table, the in-degree reasoning, the CRT
   exclusions and the corrected classification of the apparent shortcuts.
2. Start at the boundary, not at the leaves. The leaf supply is spent and no further batch generation is possible.
3. Keep `docs/fidelity/native-database.md` regenerated: it is the only surface that distinguishes code from records.
4. Do not mark anything VERIFIED without the paired runtime half; the protocol and the owner rules forbid it, and this branch's history
   shows how easy the row count makes that mistake.
