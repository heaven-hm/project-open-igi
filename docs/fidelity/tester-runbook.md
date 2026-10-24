
## Frozen-candidate matrix procedure (2026-10-24, added for dee2974c)

Run dir: `build/tester/<UTC>/` with `probes/`, `static/`, `runtime/`, `individual/`, `evidence/`.

1. Verify deployed sha and record identity: `shasum -a 256 /Users/haseeb-mir/Downloads/IGI/igi1.exe` must equal the frozen sha; capture mtime, `i686-w64-mingw32-nm -C` symbol VAs, and `strings -a | grep -E '^IGI1?MUTEX$'` (testing build shows `IGI1MUTEX`, production `IGIMUTEX`; `-DIGI_TESTING_MUTEX=ON`).
2. `python3 scripts/tester/run_frozen_matrix.py --sha <sha> --run-dir build/tester/<UTC>`: retail first, then candidate, sequential, cwd `/Users/haseeb-mir/Downloads/IGI`, candidate command exactly `window level1` (never pass `retailmain`); `--expect-exe-sha256` on the candidate side; scenarios globals/methods/parser/trio/window/germany/wininput/usa/hardcoregerman/qtask.
3. Typed comparisons via `compare_native_capture.py --default-args 1 --address-args 0x0048F130=0 --string-address ...` (C-string pointer natives: 0x48F0C0, 0x48F0F0, 0x48F100, 0x48F300, 0x48F350, 0x48F360, 0x48F920, 0x48F930, 0x48F950, 0x48F960, 0x4A5A0C).
4. Termination evidence: direct CrossOver Wine run (no Frida) `wine --bottle open-igi 'Z:\Users\haseeb-mir\Downloads\IGI\igi1.exe' window level1` from the game dir; record exit code and stderr; plus the qtask capture.
5. Records: `build_individual_record.py` per linked native (30 total incl. UnlimitedAmmo manual record); `MATRIX.md` records earliest divergence, the SetLanguage fix confirmation, verdicts, and the manifest-currency blocker.
6. Candidate symbols must be re-resolved on the exact deployed image (`rebind_candidate_facts.py --map ...`) and the manifest snapshot rebound (`rebind_manifest_facts.py`); stale manifest digests keep the static axis FAIL and must be reported, not hidden.

### Batch (combined-session) verification

7. `python3 scripts/tester/run_batch_verification.py --run-dir build/tester/<UTC> --sha <frozen-sha>`: one retail session then one candidate session with every accepted native hooked in the same process, scenario `window level1 WinInput Germany USA`, spawn-suspended hooks installed before resume. Writes `runtime/batch-retail.json`, `runtime/batch-candidate.json`, `runtime/batch-comparison.json`; per-native records get a `batch` block. Compare ordered calls/counts/returns with one ABI argument word per native plus C-string contents for string natives; any batch MISMATCH reopens that native's individual record. Only accepted natives are included; blocked/UNOBSERVED natives are listed as exclusions, never as batch passes.

## Registrar static-packet procedure (analysis-only, 2026-10-24)

Used to hand the Lead ordered retail registrar evidence for the QTask registration chain; it does not build, run, or promote anything.

1. New run dir: `build/tester/<UTC>/registrars/` plus `static/` and `raw/`.
2. Per address (`0x00402930` and the five registrar entries `0x00508F80`, `0x00489560`, `0x00407D60`, `0x00488BB0`, `0x00402FF0`, plus delegated bodies `0x00508FA0`, `0x005183D0`): `python3 scripts/tester/static_probe_capture.py --address <VA> --run-dir build/tester/<UTC>` (Binary Ninja blocks/decompile, r2 CFG, angr exact entry).
3. `python3 scripts/tester/build_registrar_packets.py --run-dir build/tester/<UTC>` writes per-address `.json`/`.md` packets plus `CHAIN.md` (full ordered 0x402930 target list) and `SUMMARY.md` (one-page chain + unresolved blocks). Binary Ninja is the only decompiler used; no Ghidra on the same functions.
4. Packet rules: addresses, immediate bytes, strings, call order and return-value use are the evidence; Binary Ninja function/API names are auto-generated placeholders and must be re-confirmed before transcription. Argument decoding is instruction-level; the embedded full disassembly is authoritative. A tail `jmp` is a tail call (0x402930's 156th target), and r2 block counts can differ from BN when the tail jump is split.
5. This step is analysis only: no runtime capture, no manifest edits, no status changes. Runtime gating of any resulting transcription stays with the normal individual + batch procedures above.

## Registrar-chain gate scaffolding (analysis-only, 2026-10-24)

Prepares per-native probes + typed contracts for the QTask registration chain so captures can start the moment a transcriptions build exists. It launches nothing.

1. Static packets first: per-address BN/r2/angr capture plus `build_registrar_packets.py` (previous section), including the delegated bodies and nested callbacks.
2. `python3 scripts/tester/build_registrar_gate.py --packets-dir build/tester/<packet-run>/registrars --static-dir build/tester/<gate-run>/registrar-gates/static --run-dir build/tester/<gate-run>/registrar-gates --candidate-sha <Lead-frozen-sha>` writes 19 typed probes (`probes/probe_<addr>.js`), per-address contracts, `contracts/CONTRACTS.md`, `BASELINE.md`, `README.md`, `inventory.json`. Probe configs are generated from `scripts/tester/frida_registrar_chain_probe.js`; every probe sha is recorded in its contract and inventory.
3. Probe evidence: target entry/return, bounded argument words, caller return address, EAX from `this.context.eax` (proven Frida pattern), range-checked side-effect snapshots (id stores, AL stores, 0x569474..0x56E174 clear range), ordered registration-API events (argument words + C-strings only for packet-proven literal addresses + returned EAX), and ordered chain calls filtered by expected callsite. API/chain events are suppressed outside the target's invocation depth so unrelated engine calls cannot pollute a capture. No unknown pointer is dereferenced.
4. Runtime order: retail first, one process at a time, then candidate with `--expect-exe-sha256 <frozen sha>`; candidate must be the Lead-frozen image (currently `6a64cf2c1e84ce95f02a97a734a10cf7d04dddb55952df6f3d8f419dd16e1aa0`). No lease overlap.
5. `compare_registrar_gate.py --contract ... --retail ... --candidate ... --expected-candidate-sha ... --out ...` checks identity (executable sha, probe sha, target hook), retail-vs-static expectations (entry/return counts, expected caller callsite, ordered registration API/name bytes, ordered chain (target, callsite) tuples, readable side effects), then candidate-vs-retail typed equality. Verdicts `MATCH`/`MISMATCH`/`UNOBSERVED`/`BLOCKED` only; zero observations are never a pass and no manifest status is changed.
6. Self-test: synthetic fixtures under `registrar-gates/fixtures/` exercise MATCH, UNOBSERVED, swapped-chain MISMATCH and side-effect MISMATCH without launching the game.
7. Order proof without BN names: each registrar/chain probe records the caller return address and the probe keeps only callsites listed in the static packet; the comparator requires the ordered (callsite, target) tuple list to equal retail. Names are labels only.
