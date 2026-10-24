# StoreCommandLine — declared single-native verification scope

Retail VA: `0x0048F0C0`; retail SHA-256:
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
Candidate: `engine/src/retail/core/app_startup_natives.cpp`, symbol
`_StoreCommandLine`, state symbol `_CommandLine`.

The scope covers one cdecl 32-bit argument word, its store into the mapped
command-line global, and the identical EAX return. The complete retail body is
`8b442404 a3c88b5c00 c3`: one block, no branch, call, argument dereference,
arithmetic, or floating-point operation. Candidate bytes must have the same
load/store/return sequence with the store operand resolved to its own global.
The original private source identifiers are not recovered; StoreCommandLine is
the existing name describing the supported behavior.

Required observations are sequential retail/candidate Frida runs on the same
fresh x86 candidate: eleven controlled ABI calls per process, with all ordered
inputs, return words, before/after destination values and global restoration
checked; and a separate natural first call from WinMain, using `window level1`
and the original game-data working directory. Natural pointers are compared
through command-line bytes and store/return alias relations, not by addresses
across processes. Input bytes and the sampled adjacent global bytes must remain
unchanged. Controlled pointer-bit values are opaque words and are never
presented as valid command-line strings or dereferenced.

The controlled probe is `scripts/frida_app_startup_store_5c8bc8_probe.js`.
The natural probe is `scripts/tester/frida_store_command_line_natural.js`.
Each probe is identical across its own retail/candidate pair. The two scopes
are retained separately. `scripts/tester/check_store_command_line_pair.py`
recomputes the controlled observations and requires the natural pair as well;
`scripts/check_frida_differential.py` checks its gate-bound typed sequence.
Probe, source, image, receipt, report and gate hashes identify the tested bytes.
A stale compiler-input receipt blocks current-tree acceptance.

This document declares the scope; it does not itself grant acceptance. The
focused strict gate and typed comparison must pass before assigning
VERIFIED_WITH_DOCUMENTED_SCOPE. No universal pointer-domain proof, complete
WinMain/CRT parity, whole startup equivalence, level load, rendering, movement,
or playable-game acceptance follows. Compiler startup/CRT and Windows SDK
implementations are outside the game-native transcription scope by owner
instruction. The later untranscribed 0x00402930 startup sequence remains a
separate game-code blocker.
