
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

## Retail-only baseline run + declared chain instrumentation limits (2026-10-24)

1. Sequence: 19 registrar probes, one retail process at a time, spawn-suspended, 35 s idle `window level1`, cwd `/Users/haseeb-mir/Downloads/IGI`, every report bound to retail `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`. Candidate was not run; no status was promoted.
2. Results live in `build/tester/20261024T004934Z/registrar-gates/reports/RETAIL-BASELINE-MATRIX.md` (+ `.json`): 12 `RETAIL_MATCH`, 2 `RETAIL_CHAIN_PARTIAL`, 5 `RETAIL_UNOBSERVED` (0x00489200, 0x005093B0, 0x00509520, 0x00509610 not invoked; 0x004D3710 is a one-byte `c3` ret stub Frida cannot attach to).
3. Chain instrumentation limits are declared, not free-form: `registrar-gates/chain-limitations.json` is injected into each chain contract as `static_expectations.instrumentation_limitations`. It records the exact expected counts, the one-byte `0x004C1780` (`c3`) ret-stub callsites that Frida cannot intercept, and the first/tail calls whose caller return address is generated by Frida's prologue relocation or absent because of a tail `jmp`.
4. `compare_registrar_gate.py` consumes those declarations: 0x00402930 must observe exactly 152 ordered rows, 2 declared install failures and 2 declared relocated-caller events out of 156; 0x005183D0 must observe exactly 66 ordered rows, 6 declared failures and 2 relocated events out of 74. Any other missing/extra/reordered/retargeted row, or a count different from the declared one, stays `RETAIL_BASELINE_DIVERGES`. A clean partial result is `RETAIL_CHAIN_PARTIAL` — never `MATCH`, never a pass.
5. Fail-closed self-tests under `registrar-gates/fixtures/partial-chain-*`: removing a row, adding a row, removing a declared failure, moving a relocated anomaly, and altering the declared count all produce `RETAIL_BASELINE_DIVERGES`.
6. Probe fixes made while validating the baseline (all re-captured): registration events are restricted to the packet-proven callsite allowlist with the correct return address (callsite + instruction length), name bytes are read only from the declared slots, and static packet names truncated at 32 characters are accepted only as exact prefixes of the full observed retail string.

## Main-prefix probe, contract and retail baseline (2026-10-24)

1. `scripts/tester/build_main_prefix_gate.py --static ... --disasm ... --run-dir build/tester/<UTC>/main-prefix --candidate-sha <frozen>` writes `probes/probe_main_prefix.js`, `contracts/main-prefix.json`, `contracts/CONTRACT.md`, `BASELINE.md`, `README.md`, `inventory.json`. Scope is the retail main entry `0x0048F530` up to the `call QTask_CreateTask` at `0x0048F7AA`; everything after that boundary is explicitly `UNOBSERVED-until-reachable` and is never scored.
2. The probe records only calls whose caller return address lies in the prefix (`start .. hardStop + 6`, so the boundary call itself is included). It records entry argument words, typed API/native calls, the full WNDCLASSA snapshot, CreateWindowExA slots, GetSystemMetrics order and derived X/Y/style, proven strings, and QTask_CreateTask entry/call/return globals. It never dereferences an unproven pointer.
3. `scripts/tester/compare_main_prefix.py --contract ... --retail ... [--candidate ... --expected-candidate-sha ...] --out ...` checks identity/probe sha/hook, entry, the ordered 40-row call stream with callsite + instruction-length tolerance, typed arguments and relational values (in-process only, never pointer addresses across processes), WNDCLASSA fields, CreateWindowExA slots and derived geometry, ShowCursor/ShowWindow/StoreWindowHandle, clear-range and pointer-chain snapshots, [0x5C8BC4]==hwnd, static absences, and QTask_CreateTask EAX 1.
4. Retail baseline (authorized lease, 30 s): `main-prefix/reports/retail-main.json` and `retail-main-comparison.json` → `PREFIX_RETAIL_MATCH` 178/179 (only the not-authorized candidate report missing). Values are summarized in `main-prefix/reports/MAIN-PREFIX-BASELINE.md`: CreateMutexA name `IGIMUTEX`, GetLastError 0/0, GetSystemMetrics 1050 then 1680, X=520/Y=285, style 0x37C80000, hwnd 0x00AF00A4, [0x5C8BC4]=hwnd, clear range zero, pointer chain [0x5C8DF0]=0x5C8DF4/[0x5C8DF4]=0/[0x5C8DF8]=0x5C8DF0, and **QTask_CreateTask EAX 1**.
5. Candidate remained unrun. When authorized, the candidate probe must only reach QTask_CreateTask entry (RegisterClasses still throws) and that is boundary evidence, never a pass. The vision-corrected candidate identity for the next candidate binding is `6d39aa53216f2d0db4edb50afac1e217fa7dd8b5881f3f073404ba4621973e2e` (source `42490c2e`, commit `6f019e12`).
6. Driver robustness fixes: the main probe's summary returns only plain values (Listener objects over RPC stall Frida on Wine), `session.detach()` is bounded by a 5 s watchdog because Wine can hang it while the game is idle, and driver dispatch recognizes both `observed` and `entry`/`return` event shapes.

## Candidate main-prefix capture (authorized one-off, 2026-10-24)

1. `build_main_prefix_gate.py` writes the retail probe and contract; the candidate run uses a generated `probes/probe_main_prefix_candidate.js` (probe sha `1b4f76…`) built from the same template with candidate addresses: main `0x004180A0` (= `Main@16`), hard stop `0x004183B7` (`call QTask_CreateTask`), candidate IAT slots, candidate symbol addresses for every native, candidate literal addresses and the candidate globals (`g_window_handle` 0x005DCE60, `WindowHandlerChain` 0x005DCE6C, `Loop_StatusByte` 0x005DCDF0). The mapping and declared divergences live in `main-prefix/contracts/candidate-profile.json`.
2. `compare_main_prefix_candidate.py --profile ... --retail retail-main.json --candidate candidate-main.json --out ...` compares per observable and relation, not raw cross-process addresses. Verdicts are `PREFIX_CANDIDATE_MATCH` / `PREFIX_CANDIDATE_MISMATCH` / `PREFIX_CANDIDATE_BLOCKED`; never a native pass.
3. Single authorized run (sha `6d39aa53…`): candidate entered main, executed the prefix, entered QTask_CreateTask at candidate callsite `0x4183BC`, threw inside QTask_RegisterClasses and exited 2.56 s after spawn. Result `PREFIX_CANDIDATE_MISMATCH` with exactly one undeclared divergence: `WindowHandlerChain` is all zero at the QTask entry while retail has `[0x5C8DF0]=0x5C8DF4/[+4]=0/[+8]=0x005C8DF0`; summary in `main-prefix/reports/CANDIDATE-MAIN-BASELINE.md`, typed result in `candidate-main-comparison.json`.
4. Declared, not-hidden candidate differences: `IGI1MUTEX` name (testing switch), one GetLastError call tested against 0xB7 and 0x6, absent `0x00498480` DX gate, `0x00498460` substituted by `FP_InitExtendedPrecision`, relocated code/data addresses, and no candidate equivalent of the retail BSS clear range. `LoadCursorA`'s IDC_ARROW handle is process-scoped under Wine, so only the WNDCLASSA.hCursor relation is compared.
5. Boundary: QTask entry is boundary evidence; its return/EAX, the pump, WM_QUIT/exit-code path and shutdown are UNOBSERVED-until-reachable and unscored. The candidate process was confirmed gone through Frida before releasing the lease. No registrar candidate run was performed.

## Release screenshot: working Wine invocation for the internal build (TESTER notes, 2026-10-24)

The CrossOver `winewrapper` rejects `igi1.exe window level1` as one string. The working launch for the internal build is the `igi1` bottle through **Wine Stable** with an explicit `WINEPREFIX` and separate argv:

```sh
cd /Users/haseeb-mir/Downloads/IGI
WINEPREFIX="/Users/haseeb-mir/Library/Application Support/CrossOver/Bottles/igi1" \
  "/Applications/Wine Stable.app/Contents/Resources/wine/bin/wine" ./igi1.exe window level1 &
wine_pid=$!
sleep 2
screencapture -x /Users/haseeb-mir/Documents/Code/project-open-igi/screenshots/v0.0.1-candidate-startup-2s.png
wait "$wine_pid"; echo "wine exit=$?"
```

- Use the absolute `WINEPREFIX` above; relative paths and the CrossOver wrapper fail.
- Reliable screenshot offset: **2.0 s after starting `wine`**. The current candidate reaches its window before 2.0 s and exits ~3.0 s after spawn when QTask_RegisterClasses throws; 2.0 s is inside that window and is the offset used for `screenshots/v0.0.1-candidate-startup.png`. Safe band is roughly 1.2-2.4 s; if a frame is missed, retry at 1.5 s rather than increasing past 2.5 s.
- `screencapture -x` is silent; keep the filename under `screenshots/`. The command returns only after Wine exits, so the exit code is the documented registrar throw exit, not the screenshot result.

## Freeze-before-notify rule (TESTER, 2026-10-24)

The Lead freezes each candidate before notifying the Tester. The Tester only tests the frozen artifact, never the live build directory:

1. Freeze the artifact under `build/artifacts/` and give the Tester its sha256.
2. Tester copies it into the run directory (`main-prefix/frozen/`), records the hash before and after the run, and passes that copy to the driver (`--exe`).
3. Never test `build-win32-no-verify-gate/igi1.exe` or the deployed copy unless its hash equals the frozen hash at that moment.
4. If the live build tree changes after the freeze, the receipt describes the live tree, not the frozen artifact; report the lineage mismatch and ask for a new frozen pair instead of regenerating evidence silently.
5. Regenerate a receipt only with `scripts/candidate_build_receipt.py` (it runs the configured clean build and hashes the configured output); never hand-edit `candidate-receipt.json`.

## Receipt/freeze ordering (TESTER, 2026-10-24)

The PE is not rebuild-reproducible: each clean build changes the PE `TimeDateStamp`, so a receipt only matches the build it just performed. The correct order is:

1. build the candidate;
2. run `scripts/candidate_build_receipt.py` (it rebuilds and hashes the configured output);
3. freeze **both** the receipt's binary and the receipt JSON together under `build/artifacts/`;
4. deploy that exact binary;
5. notify the Tester with the frozen pair and its sha256.

The Tester verifies `receipt.candidate_sha256` equals the frozen binary's sha256, copies the pair into `main-prefix/frozen/`, and tests only that copy. Never pair a frozen binary with a later receipt; report the mismatch and wait for a new frozen pair.
