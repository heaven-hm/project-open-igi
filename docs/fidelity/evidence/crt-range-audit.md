# CRT-range audit of implemented natives

Written 2026-10-25 (round 245) after round 244 established that 0x004AF000-0x004B0000 is CRT support, not game code. The audit asks
the obvious follow-up: how many implemented natives sit in regions that look like compiler/runtime rather than game code?

Ranges treated as CRT-looking, each established by caller and idiom evidence rather than by address alone:

- `0x00401000-0x00402000` - CRT startup/support (its routines are called from the CRT entry sequence)
- `0x004A5000-0x004AB000` - CRT string/memory primitives (`strchr`-style bodies confirmed by decoder disagreement earlier)
- `0x004AF000-0x004B0000` - CRT pattern/list support (round 244: callers are 0x00401xxx, helpers fill class tables with 0x01010101)

## Result

**26 implemented natives** live in those ranges: 12 in `0x00401xxx`, 4 in `0x004A5xxx-0x004AAxxx`, and 15 in `0x004AFxxx`.

Of those, exactly **one** is a landing from this session: `0x004AF8F0` (already flagged in the manifest in round 244). The other 25
predate this session and were inherited from earlier branch work, which means the historical tally of "game natives" in this tree has
always been inflated by runtime code.

**82 manifest rows** in those ranges carry a verified-family status (VERIFIED, VERIFIED_WITH_DOCUMENTED_SCOPE or
STRUCTURALLY_VERIFIED). They are not evidence of game-native coverage and should not be counted as such. Combined with the 96
claims-without-recorded-evidence rows in the database, this is the second independent way the manifest overstates progress.

## What this changes in the reporting

- The session's game-native tally is **159**, not 160: the corrected row is runtime list support.
- Any future count must exclude these ranges, or state them separately, otherwise it counts CRT routines as reconstruction of the game.
- The audit does not delete anything. Deleting transcribed retail bytes would erase the work; the correction is classification, and it
  belongs in the records, which is where it now is.

## Method note for the next session

Region membership is a heuristic, not proof. The three ranges above were each justified by callers or instruction idioms, and a native
should only be moved into CRT classification with that kind of evidence - the same standard used to correct 0x004AF8F0. Address alone
would misclassify 0x00401xxx game-side routines if any exist there.
