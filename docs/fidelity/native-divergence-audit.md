# Native divergence audit — 2026-10-19

This is an incomplete reconstruction checkpoint, not acceptance of terrain or player parity.

## Confirmed repairs

- Removed host terrain LOD distance bands at 250/600/1200 metres. A synthetic regression tree reaches LOD 14 at shifted distance 200 within 250 metres; the single host threshold 163 selects its payload, while the former threshold 320 descends to an empty child. This tests the removal only, not the complete retail traversal.
- Documented String_ToUpperChar at 0x004AD73D as implementing only the zero-locale-global branch. See scopes/0x004AD73D.md. Its status is TRANSCRIBED, not full-function structural verification.
- The five native terrain placeholders now throw an explicit unimplemented error instead of reporting success. Their manifest statuses are ANALYZED. Host_LoadLevelTerrain and Host_BuildRenderMesh still run separately in the diagnostic scene; they are not implementations of these native entry points.

## Retail LOD producer: correction to the proposed constant

The retained pseudocode at scripts/GameNatives/render/004D14E0-Render_ShouldHaveAllocatedD.cpp reads g_b16c6c, not an unconditional literal 163. Radare2 live disassembly of the exact retail image confirmed that 0x004D2540 reads quality at 0x0054844C and FOV state at 0x00BCAB20, derives the threshold, stores it at 0x00B16C6C (0x004D2627), and clamps it to 70 (0x004D2624–0x004D2638). The on-disk quality word is 115; a runtime setter exists at 0x004D2C20. The host constant 163 remains provisional. No new decompiler was used for this finding; it combines the existing catalog pseudocode and live r2 disassembly.

## Remaining work (must not be accepted as fixed)

| Entry/path | Required work |
| --- | --- |
| 0x004C6240 Terrain_Load | Recover native context layout/ownership, resource calls, QVM registration/execution and CTR relocation; then reconstruct the body. Do not cast a retail context to TerrainLevelData. |
| 0x004C5F00 Terrain_LoadTexturesAndLightmaps | Reconstruct texture/palette/lightmap load order and state, with failure cases. Calling the combined host loader is not the same operation. |
| 0x004D14E0 Terrain_TraverseContourQuadtree | Full frustum tests, dynamic threshold producer, orientation/queue state and ordering. Host mesh generator currently ignores camera_target/fov inputs. |
| 0x00510010 Terrain_DecodeChunkCommands | Recover global queue/buffer consumer ABI and state; preserve CMD/HMP/material/cache paths. Existing pseudocode has no context argument; current declaration remains unverified. |
| 0x00512C30 Terrain_Draw | Resolve actual calling convention/arguments, render-state sequencing, texture stages, queue decode and batch submission. |
| Ground locomotion | The jump-speed-derived run/walk/crouch fractions were removed. Missing authored root-motion now throws; walking is unavailable until real track integration. Unsupported-motion failure is not transactional: discard/reset the controller after the error. Implement authored animation root-motion intervals next. Reference lookup locates BANM loader 0x004CF910, translation sampler 0x004D4840, movement integrator 0x004614B0 and HumanPlayer parameter loader 0x00410070. No reference-project code was copied. |

All remaining paths need current native evidence, independent CFG recovery and paired sequential retail/OpenIGI traces before verification. A host build or this regression test cannot establish runtime parity.

## Tool identity observed in this checkpoint

The live `binary_session` MCP call identified upstream `rea-agents` 6.4.0 from `/Users/haseeb-mir/.npm/_npx/dea9475797100b20/node_modules/rea-agents`, with retail igi.exe open through Ghidra 12.1.2. This is not the project-specific RE-Agent fork. The configured `re-agent/node_modules/.bin/rea` is also a symlink to `../rea-agents/scripts/rea.mjs`; its parent directory name does not prove use of the local fork. No new function was decompiled through this session, avoiding a second decompiler for the retained Binary Ninja targets. Binary Ninja MCP view inventory/activation succeeded; live Radare2 was used for the threshold producer disassembly. Do not claim the local RE-Agent orchestration is active from these observations.

## Validation of this checkpoint

Windows x86 open-igi target builds. Host build and 37/37 CTest tests pass, including the LOD regression and refusal to fabricate missing root motion. `./build.sh` remains BLOCKED on full production native evidence (see build/native-divergence-gate.log). No runtime acceptance, game-folder deployment or feature completion is claimed. Independent read-only review confirmed the LOD fixture and identified stale ABI/CFG assertions in the manifest; those assertions were withdrawn and failure-state behavior documented.


## Attached gate-log repairs

The attached 91-entry gate log exposed both missing verification plumbing and unfinished natives. The missing shell `fi` in build.sh is repaired. Integer-valued outputs in the Unicorn differential comparator now require exact equality; floating-point tolerance no longer permits incorrect flags or descriptors. Failed fixtures now give a nonzero process exit. Four regression tests cover that comparator. All 28 Python regression tests pass. The restored CFG topology helper is diagnostic only; production retains its strict score.

Seventeen existing differential runners are now registered: nine Unicorn adapters and eight additional ABI/resource/list fixtures. The isolated runs passed 96 and 117 cases respectively. Their limitations explicitly exclude live-game integration and complete Windows ABI parity. Nine existing semantic-review references were connected only where their recorded implementation hashes matched. No native status was promoted, and stale hashes were not rebound to claim acceptance.

Resource_RegisterIlffContainer (0x004B60F0) had a real error-path divergence: malformed input returned after Error_Show, whereas retail calls the error function then executes `EB FE` at 0x004B637D. The reconstruction now preserves that fatal loop. The resource harness distinguishes actual return from the observed post-error self-loop; malformed fixtures remain enabled. Before the native fix, two of ten cases failed; afterwards all ten passed. The DBUG-sibling fixture now follows the observed NAME/BODY/DBUG record order. The tested MinGW x86 `-O3 -DNDEBUG` object retains the fatal self-loop; this is not a whole-release runtime result. Independent read-only review found no scoped issue in these changes or the eight registrations.

Reproduction artifacts are under build/gate-repair/: container-before-fix.json, additional-summary.json, per-address stdout/stderr, resource-release.asm, and full-build-after-resource-fix.log. These are generated local artifacts. Executable-memory emulation requires execution outside the local sandbox: Unicorn mem_map traps with SIGILL inside it, while the same minimal mapping and differential checks succeed outside. This environment limitation must not be worked around by disabling behavioral checks. The strict full production gate remains the acceptance criterion.


The completed strict rerun (full-build-after-resource-fix.log) checked 91 entries and recorded 883 passing differential cases across 45 runners, but still BLOCKED with 225 findings. These include 20 sources without bound native identities, 68 provisional statuses, 52 missing semantic references, 43 missing runners, and 42 other findings. Three allocator reports intentionally remain supplemental: they do not measure state/memory side effects and fail the complete behavioral schema. Passing their return/allocation-size fixtures is not acceptance.

That rerun also found resource_sys_diff_runner.py was not executable when launched directly by the gate. The executable permission is corrected. The gate's actual run_live_behavioral_runner launch path was then exercised for 0x004AAD10, 0x004B59F0, and 0x004B60F0: all three passed with no launcher/report errors (32 cases; resource-gate-launch.json). This focused confirmation does not replace the full report or clear stale source hashes/semantic reviews. No executable was deployed.


## Live baseline observation and Release configuration

Frida was restored in the existing open-igi CrossOver bottle, with no second game launched. The existing installed PE32 process (Windows PID 204) loaded ddraw.dll and wined3d.dll. Its saved log reports a first frame with 889 objects, 3774 model batches, and 359 uploaded textures; those counts do not prove correct scene placement or visual parity. A symbol-bound probe of the installed Scene::draw measured 37 completed calls in 8002 ms, median 193 ms, mean 195.7 ms. This is an observed performance failure under instrumentation, not a 30 FPS result. Identity, raw captures and summary are under build/runtime-traces/20261019-baseline-observation/.

The first timing captures repeatedly restarted the script and never produced a final summary. With Frida --no-auto-reload, one uninterrupted summary was captured. The runtime workflow now requires this option. build.sh now selects CMAKE_BUILD_TYPE=Release for all four host/Windows configure paths; fresh isolated CMake configurations passed and Windows flags contain -O3 -DNDEBUG. Independent review approved the narrow configuration change and the integrity digest was refreshed; all four integrity checks pass. No Release link, deployment, screenshot or paired retail comparison is claimed. The full production native gate remains blocking.


## App-context inventory and return-register repair

Seven real code entries in app_context.cpp are now provisionally bound in the manifest (98 total records). Data-global addresses were not treated as functions. Radare2 confirmed the mapped leaf instructions and angr recovered all seven exact entries. Four setter declarations now expose the observed sign-extended EAX result instead of declaring void; this is an inferred interface, not a claim about retail source types. MinGW -O3 -DNDEBUG emits the observed MOVSX/store/RET sequence. All seven differential checks pass: 1024 exhaustive setter-byte cases plus 36 getter-state cases. The native_diff_runner uses retail ESIL execution against freshly compiled host candidates; optimized x86 code was inspected separately. Independent review found no scoped issue. Status remains TRANSCRIBED; live integration and required semantic/build evidence are incomplete. See evidence/app-context-inventory-checkpoint.json. No release acceptance or deployment.


## Selected decompiler and CMake bridge checks

The native gate now validates one explicitly selected decompiler plus Radare2. Existing Binary Ninja-bound targets retain that selection; new/unbound records require an explicit selection. A second decompiler cannot rescue missing selected evidence or a failed selected CFG. Five integration regressions cover these conditions. Independent review approved the change and protected-file digest refresh. Full strict rerun: 98 entries, 55 passing live differential runners, 1975 passing cases; release still BLOCKED. No runner/schema failure remains in that report, but 267 evidence/status/coverage findings remain. Report: build/gate-repair/full-build-selected-decompiler.log.

Review also exposed CMake bridge failures: weaker production thresholds, ignored capture/probe errors and unreliable Radare2 lookup. These now fail closed, with five regression tests. Same-file references are no longer dropped by discovery: ordinary own definitions/declarations are excluded while non-declaration uses remain candidates. Four further regressions cover this lexical filter; it is not a full C++ call graph. Current static candidate count is 34. The combined Python suite passes 42 tests.

## RE-Agent registration repair

The active MCP initially identified upstream rea-agents under re-agent/node_modules, not the fork launcher. The fork's setup dry-run selected only a backed-up Codex MCP command update and bundled skill refresh. Both authorized actions completed with status ready. The fork's direct MCP doctor passes. The already connected server has not been proven reloaded; do not claim it changed identity. Exact paths and limitations: evidence/re-agent-registration-repair.json. The fork CLI remains usable without stopping work for a client restart. Its documented host-64-bit Mach-O Frida profile does not replace the project's Windows x86 remote Frida workflow.

## Focused app-context gate and pre-resume retail evidence

The seven app-context leaves pass the focused structural gate with semantic and behavioral checks enabled: exact-entry angr7/7, CFG1.0, 1060 passing cases, zero blocking findings. All seven remain STRUCTURALLY_VERIFIED, not VERIFIED. The three getters now use newly captured Binary Ninja evidence, while the four setters retain their previously selected Ghidra snapshots. The complete Binary Ninja raw view matches the 1,384,448-byte retail file byte-for-byte and by SHA-256. Independent review approved only the declared leaf structural/signature assessment. See evidence/app-context-structural-checkpoint.json.

RE-Agent's live binary_session now identifies package re-agent at the fork checkout and its own scripts/rea.mjs launcher; the saved configuration repair has therefore taken effect in the active MCP connection. The registration evidence record contains this observation.

Retail-only suspended startup captures completed sequentially for Levels1/4/7, with actual CWD checked and all98 hooks installed before resume. Levels4/7 provide847 known scalar app-context observations with no observed mismatch; the debugged getter was not called. All owned test processes were closed and absence confirmed. These captures improve startup coverage but do not establish OpenIGI scene parity. No game-window screenshot, 30FPS result, or release deployment is claimed. Exact paths and limitations are in evidence/retail-suspended-startup-matrix.json and runtime-test-workflow.md. Full strict build rerun is tracked by build/gate-repair/full-build-after-app-context.log.
