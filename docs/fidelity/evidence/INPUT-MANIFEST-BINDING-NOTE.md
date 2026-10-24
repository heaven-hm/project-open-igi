# Input-layer natives: manifest binding is the current gate blocker

Run 2026-10-24 (round 10), repo state after commit `30282cc5`:

```
$ python3 scripts/native_gate.py --root . --changed --threshold STRUCTURALLY_VERIFIED --cfg-threshold 0.85
native_gate: v4 catalog consistency checked for 0 selected entries
native_gate: OK - 0 changed native(s) passed ...
```

**That OK is an empty scope, not a pass.** The changed-native gate selects
natives through `docs/fidelity/native-verification.json`; the input-layer natives
landed this session (`0x4AF740`, `0x4AE730`, `0x48F090`, `0x48F0A0`, `0x48FA80`,
`0x48FD90`, `0x48FED0`, `0x490120`, `0x490190`, `0x48F270`, `0x509CC0`,
`0x48F0A0`, `0x509E90`, `0x509F00`, `0x490340`, `0x490350`, `0x509CC0`) have **no
manifest records**, so the gate cannot see them and cannot report CFG >= 0.85 or
semantic/behavioral evidence for them.

`scripts/build_native_records.py` cannot create the records yet either:

```
FileNotFoundError: docs/fidelity/evidence/0x0048F090-binaryninja.json
```

So each record requires a per-native Binary Ninja evidence artifact at
`docs/fidelity/evidence/0x<VA>-binaryninja.json` in the schema that script
expects, plus the semantic/behavioral evidence the gate demands at
`STRUCTURALLY_VERIFIED` (CFG >= 0.85) and beyond.

## Consequence

- The natives are honestly at TRANSCRIBED. No CFG/angr/runtime claim exists for
  them, and the empty gate result must never be cited as one.
- Objective compliance ("all game natives match CFG >= 0.85 and are runtime
  verified") is blocked on: (1) per-native BN evidence JSONs, (2) manifest
  records generated from them, (3) the runtime path that can observe them.

## Next actions

1. Produce `0x<VA>-binaryninja.json` evidence for each landed input native from
   the already-captured Binary Ninja analysis (function extent, blocks, edges,
   calls, data refs) - no re-decompilation of a different tool.
2. Run `scripts/build_native_records.py <VAs>` then the gate; record per-native
   CFG scores; fix any native below 0.85 before it is used in a body that runs.
3. Only then pursue runtime observation, which still requires the registrar
   chain to reach the input path.

## Round 11 update: evidence-artifact chain, one step further

Created the Binary Ninja disassembly artifacts for the seven single-block natives
(`0x0048F090`, `0x0048F0A0`, `0x0048F270`, `0x004AF740`, `0x004AE730`,
`0x00490340`, `0x00490350`) at `docs/fidelity/evidence/0x<VA>-binaryninja.json`
in the existing schema (schema_version 1, kind native-disassembly, tool
binary_ninja, retail SHA-256, cfg nodes/edges).

`build_native_records.py` then fails on the **next** artifact in the chain:

```
FileNotFoundError: docs/fidelity/evidence/0x0048F090-radare2.json
```

So a record needs at minimum BN + radare2 (+ semantic-review/behavioral/build)
artifacts per VA. None of those can be fabricated: radare2 must be an actual
capture of the retail CFG for that entry, and semantic-review/behavioral records
must reflect review actually performed. The remaining steps are therefore tool
runs, not authorship:

1. radare2 CFG capture per VA -> `0x<VA>-radare2.json` (same schema family as the
   existing `0048F1A0-radare2.json`).
2. semantic-review artifact per VA against the transcription just written.
3. behavioral/build artifacts from the real candidate and probe runs.
4. `build_native_records.py` -> gate -> per-native CFG score.

Until then the input natives stay TRANSCRIBED and the changed-native gate
continues to report an empty scope.
