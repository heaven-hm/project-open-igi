# HeightMap_Enter investigation, 2026-10-20

Retail entry: `0x00506F00`, PE32 at `0x00400000`, SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
The executable remains the source of truth. This is a candidate correction,
not a claim that the whole terrain or player feature is complete.

## Findings and correction

Binary Ninja was the sole decompiler used in this pass. RE-Agent's Radare2
disassembly confirms the source-row test, DWORD stores at context `+0xE8`,
stack write, reload of context `+0xF8`, active-index update and counter store.
The old candidate omitted the source-row test, wrote one flag byte and added
a 64-entry cap absent from these retail instructions. Those differences were
removed. Neither retail nor the corrected candidate accepts a null context.

`angr.json` independently recovers the exact entry automatically: four blocks,
four edges and no unresolved jumps. `radare2-cfg.json` retains the retail graph.
These graphs describe retail structure, not universal behavioral equivalence.

## Reproducible checks

`scripts/heightmap_enter_diff_runner.py` compiles the actual source into an x86
DLL and executes both that DLL's native body and retail instructions in Unicorn
with identical concrete contexts, rows and global state. It compares the entire
512-byte context, normalized return, active index, counter and first 16 stack
words. There is no copied reference implementation.

- `before.json`: 0/8 matching cases on the previous candidate.
- `after.json`: 8/8 matching cases after the correction, also validated by the
  native gate's real behavioral-report validator.
- The focused host terrain loader executable passed, including Level 1 mesh
  loading and DWORD flag/source-row regression assertions. It was linked with
  the existing host archive; this is not a fresh whole-project build.
- The normal `./build.sh` completed its verification stage and blocked 105
  changed natives at `VERIFIED` / CFG 0.85. No release binary was produced.

The previous generic semantic report incorrectly said this leaf had no global
writes. `semantic-review.json` replaces that claim with an independently
reviewed, explicitly limited memory/state analysis. The manifest now selects
this review and the native-specific runner; its verification status was not
promoted to `VERIFIED`.

The reports bind the exact tested workspace file, which also contains other
agents' terrain changes. Only this native's correction is owned by this patch;
these reports do not attest a clean checkout or authorize other terrain changes.

## Runtime and review limits

`retail-runtime-summary.json` summarizes a Level 1 suspended-startup Frida
capture: 1,599 native returns, six distinct entry states and no probe errors.
The raw capture is under the recorded `build/runtime-traces/` directory.
OpenIGI presented a scene through its host path but did not call its
`HeightMap_Enter` symbol in the paired observation. Therefore paired game
runtime parity is **unverified**.

An independent read-only reviewer checked the correction and harness. Its
findings were addressed by adding nominal/boundary case categories, deriving
missing candidate symbols from `nm`, and propagating failed captures through
the capture CLI's exit status. Counter fixtures remain limited to 0..4;
counter >=64, invalid memory, arbitrary aliases and overlapping retail globals
remain untested. The cap's removal follows disassembly and is not covered by
an overflow regression case. The fixed-size host global backing is still a
portability limitation. No `VERIFIED` promotion or source-hash restamping was
performed to conceal these gaps.

RE-Agent MCP supplied hash-bound Radare2 evidence. Its autonomous command plan
was attempted but rejected because it requires the original command to be the
hash-bound binary itself; this macOS test executes the Windows PE through an
emulator. No successful autonomous-loop integration is claimed. The separate
upstream REA repository was not integrated by this change.
