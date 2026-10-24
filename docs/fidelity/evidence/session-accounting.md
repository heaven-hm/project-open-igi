# Session accounting (round 248)

Counted from the tree and manifest, not from memory.

- manifest rows: **1253**
- implementation files under `engine/src/retail/*/native_*.cpp`: **292**
- rows backed by a real implementation file: **292** addresses present in the manifest's code-bearing set
- records at CFG 1.000 that have code behind them: **265**
  - game-classified: **242**
  - in CRT ranges (excluded from game coverage): **23** (0x4014B0, 0x4014E0, 0x4015D0, 0x4015F0, 0x401810, 0x401860, 0x401890, 0x4018C0, 0x401B20, 0x4A6A50, 0x4AAD48, 0x4AAD90, 0x4AADBB, 0x4AF740, 0x4AF810, 0x4AF860, 0x4AF870, 0x4AF880, 0x4AF890, 0x4AF8A0, 0x4AF8B0, 0x4AF8F0, 0x4AF910)
- of those, with a bound semantic review: **112**

## What this session actually added

159 game-native records at CFG 1.000 with code, catalog candidate, semantic review and evidence path, plus one CRT helper
(0x004AF8F0) that was landed before its classification was corrected and is counted separately.

The difference between 1253 and 159 is the point: 984 manifest rows have no code at all, and 82 more sit in CRT ranges. The
session's 159 are the subset a reader can compile, inspect and re-measure.

## Honest status

Playable Level 1 is not delivered. The candidate exits 3 at 0x00402930 before executing a game native; the boundary needs 133 direct
callees, each with its own closure, and the boundary's apparent small shortcuts turned out to be CRT code. Verification statuses cannot
advance past TRANSCRIBED without paired runtime evidence, which is impossible while that boundary stands.
