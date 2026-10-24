# Audit of the independent decoder capture set (2026-10-25)

Purpose: before the generated per-native `binaryninja.json` / `radare2.json` captures are treated as independent
agreement evidence, check them against each other across the whole set and against the values this session's
transcriptions were measured at. Both decoders run on the same retail image
(`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`), so for any address where both recovered a
function the node and edge counts are a direct cross-check.

## Result

- 557 addresses carry both captures.
- **552 agree** on blocks and edges.
- **5 disagree**, listed below.
- The 27 natives landed this session that carry captures all have both decoders present and in agreement, and each
  matches the counts measured when the transcription was built (`0x004B1620` 13/18, `0x004B13A0` 13/18,
  `0x004B1A90` 6/8, `0x004B0E00`/`0x004B0E70` 6/7, `0x004B0F30` 3/3, `0x004B0EE0` 5/6, the rest 1/0).

## The five disagreements (blocks/edges: binaryninja vs radare2)

| VA | binary_ninja | radare2 | reading |
| --- | --- | --- | --- |
| 0x00490120 | 5 / 6 | 5 / 3 | same blocks, different edge count: one decoder is merging or dropping successor edges. |
| 0x00401D30 | 1 / 0 | 6 / 7 | the two decoders disagree about the function's extent, not just its edges; this needs a byte-level tiling before either capture is used. |
| 0x0048FED0 | 10 / 11 | 10 / 3 | same blocks, large edge disagreement. |
| 0x0048FA80 | 59 / 64 | 59 / 58 | same blocks, six edges apart. |
| 0x00490190 | 10 / 10 | 10 / 2 | same blocks, edge disagreement. |

## Consequence

These five addresses must not be cited as "two decoders agree" evidence until the disagreement is resolved by tiling the
bytes or by a third method. The other 552 pairs are usable as mutual-agreement evidence, with the usual caveat that
agreement between two decoders on a CFG is structural evidence about the image, not behavioural equivalence.
