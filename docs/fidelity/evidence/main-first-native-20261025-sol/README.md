# WinMain's first game-native call: StoreCommandLine

Retail `0x0048F5F4` calls `0x0048F0C0`. The ten-byte body loads its one
cdecl word argument, stores it at `0x005C8BC8`, and returns the same bits in EAX.
Binary Ninja is the selected decompiler; Radare2 and angr independently recover
the entry. BN/r2 graph agreement is 1.0, one block and no edges. That number is
retail-tool agreement, not a candidate semantic-equivalence score.

`capture.json` retains the raw static responses. The normalized evidence and
semantic review are new records for source SHA-256
`0cd758f2ad54db1e376f379e730f907a4c5dfb986d519302642cb948d41fdc08`.
The prior manifest record is preserved unchanged in `prior-record.json`.
Its stale hashes and unrelated chunk-walk review do not authorize this native.
The active status was reduced to RUNTIME_TESTED, not promoted to verified.

`natural-*.json` and `controlled-pair.json` retain the completed observations
on image `9d901ded6761ce5b959a0d2f64f53da030bdf6a7e020ab81dff1a26acbd58e4a`:
one real WinMain call per game and eleven controlled NativeFunction calls per
game. Natural pointers are compared by complete command-line bytes and alias
relations. Controlled inputs are opaque 32-bit words and are never dereferenced.
The latter tests return bits, destination before/after values and restoration;
it does not resume the game's main thread. Natural startup is tested separately.
The old image was superseded by a later clean build; these are retained results,
not acceptance of a new binary.

Later rebuild/capture attempts and gate rejections are retained in compressed
logs. The undefined `g_a94a64` link error was subsequently fixed by the other
implementation writer. Concurrent source edits can still invalidate the entire
compiler-input receipt. The strict refreshed gate correctly rejected the
RUNTIME_TESTED status at threshold VERIFIED. No flag or gate requirement was
reduced. Compiler CRT startup and Windows SDK implementations are outside the
game-native scope per the owner's clarification; the ordered work starts at
WinMain, then StoreCommandLine.

The verification scripts are outside engine/. `verify_store_command_line.py`
runs the build, separate natural and controlled captures, proposed scoped gate,
and typed comparison without model-turn gaps. It never promotes the active
manifest or deploys a game. `check_store_command_line_pair.py` checks every
ordered recorded case against the instruction effect, requires the natural
pair, validates hashes/receipts, and exports a typed differential for the
existing acceptance checker. A proposed scope document is a declaration only;
it cannot replace a passing gate and comparison.

All results are limited to the declared single native and scenarios. No full
WinMain, startup, level rendering, movement or playable-game parity is claimed.
The game-code startup sequence at 0x00402930 remains incomplete. An independent
human/agent critic has not been performed in this run. `files.json` hashes the
retained files; raw logs are gzip-compressed to preserve their bytes through Git
line-ending normalization.

## Scoped acceptance completed

`acceptance.json` records the final declared-scope pass on retained image
`d750a86cf648627bae10f8b8c52813d4cf7acd74b2f28e1b5a779a8ec4243250`.
The strict focused gate passed with CFG 1.0 and a fresh live 11/11 replay.
The corrected typed comparator and existing runtime checker both passed;
a tampered recorded return was rejected. The initial adapter-hash failure is
retained, not relabeled as a pass. The active record is now
VERIFIED_WITH_DOCUMENTED_SCOPE. This supersedes the provisional status above
for this exact tested source/image/scope only. No engine code was added.
