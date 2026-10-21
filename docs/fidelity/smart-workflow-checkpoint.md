# Codex smart workflow checkpoint

Branch: `codex-smart-workflow`. Current executable: diagnostic `build-clean-win32/igi1.exe`; no model scene or retail native implementation is accepted on this branch yet.

Observed tool checks: Binary Ninja MCP, radare2 6.2.0, angr 10.0.4.post1, remote Frida, RE-Agent CLI 6.4.0, RE-Agent JavaScript Frida module, x86 compiler, pinned retail SHA-256, and historical v4 catalog. The doctor report is `build/tool-health.json`. An actual RE-Agent MCP Frida process listing is retained in `docs/fidelity/evidence/codex-frida-provider-health.json`. Ghidra was not used for decompilation; an attempted RE-Agent Ghidra open was rejected because Binary Ninja was already selected.

The main entry at retail 0x0048F530 was freshly captured with Binary Ninja, radare2 and angr. BN/r2 each recovered 26 blocks, with recomputed CFG similarity 1.0. Angr recovered the exact entry without unresolved jumps. Sampled entry bytes match the selected retail image. Full raw output is in `build/native-analysis/0x0048F530/capture.json`; this is structural analysis only, not candidate verification.

The report checker passed 10 infrastructure cases, covering a complete validator fixture and deliberate omissions/corruptions. Generated parser fixtures are not retail game tests. Default `./build.sh` was exercised and correctly blocked the current empty native scope after compiling the candidate and binding its inputs. `./build.sh --diagnostic` builds only the shell.

The Frida headless shell smoke passed: explicit `Process.findModuleByName` resolved `igi1.exe` as ia32/pointerSize 4. The first smoke attempt failed because CrossOver exposed ucrtbase.dll through `Process.mainModule`; the check reported failure and terminated its owned process. The corrected smoke terminates the owned candidate before main resumes, without touching the pre-existing open-igi.exe session. This establishes scripting plumbing, not game behavior.

Independent read-only review found missing `--file` in the per-address wrapper, an ungated default build, and an inconsistent compiler override; these were corrected. All new ignored helpers/configuration/catalog files must be explicitly tracked.

Next increment: evidence-guided retail startup and model/resource/QVM flow, textured Level 1 models, and the explicitly authorized previous developer free camera. Historical source statuses and placement heuristics are not carried forward as accepted behavior. Full paired runtime capture remains subject to availability of the existing game session; do not terminate another agent's run.

Strict intake update: `scripts/igi_workflow.py intake <VA> --probe <reviewed-probe> --level <N>` is now the fail-closed entry for new-native increments; it accepts only on a full static+runtime pass. `--static-only` in `scripts/smart_end_to_end_verification.py` is fail closed and cannot be accepted. `scripts/workflow_self_check.py` regression-covered both reject paths (12/12 checks pass).
