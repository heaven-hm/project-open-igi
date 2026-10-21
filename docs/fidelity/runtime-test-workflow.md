# IGI runtime comparison workflow

This is the required paired runtime workflow for native reconstructions and game features. It compares the retail game with OpenIGI using the same level and launch options, one process at a time because both acquire `IGIMUTEX`.

For runtime acceptance, the paired Frida probes are the check: run the same scripted scenario against retail and the newly built OpenIGI candidate, then compare the captured native calls and typed state. Do not make a separate human gameplay check a prerequisite; record any probe coverage limits as unresolved evidence instead.

For a controlled parallel Frida experiment only, retain retail's `IGIMUTEX` and inject an OpenIGI-only `CreateMutexA` name replacement before OpenIGI resumes. Use `OPENIGI_FRIDA_TEST_MUTEX`, record the injected override, terminate both owned processes after capture, and never describe that override as release behavior or parity evidence.

## Local targets and tools

| Item | Local path or command |
| --- | --- |
| Repository | `/Users/haseeb-mir/Documents/Code/project-open-igi` |
| Retail executable | `/Users/haseeb-mir/Downloads/IGI/igi.exe` |
| OpenIGI executable | `/Users/haseeb-mir/Downloads/IGI/open-igi.exe` |
| Game data root | `/Users/haseeb-mir/Downloads/IGI` |
| Level scripts | `/Users/haseeb-mir/Downloads/IGI/MISSIONS/location0/LEVEL<n>/OBJECTS.QVM` (some extracted levels also have `objects.qsc`; the loader resolves the compiled `.QVM`) |
| Player parameters | `/Users/haseeb-mir/Downloads/IGI/humanplayer/humanplayer.qvm` |
| Controls | `/Users/haseeb-mir/Downloads/IGI/config.qvm` |
| CrossOver bottle | `open-igi` |
| CrossOver launcher | `/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine` |
| Frida CLI | `/Users/haseeb-mir/.local/bin/frida` |
| Frida process list | `/Users/haseeb-mir/.local/bin/frida-ps` |
| Frida server in bottle | `Z:\Users\haseeb-mir\.cache\frida-crossover\frida-server-x86.exe -l 127.0.0.1:27042` |
| Reusable probe | `/Users/haseeb-mir/Documents/Code/project-open-igi/scripts/frida_level_probe.js` (generated manifest targets plus explicit HumanPlayer/config/InputPort runtime anchors; entry hits plus raw x86 argument words and EAX returns for the first four calls) |
| Scene log | Repository working directory `open-igi-model-scene.log`; platform log is `open-igi.log` |
| Runtime evidence | `build/runtime-traces/<date>-level<n>-{retail,open-igi}/` |

## Paired run procedure

1. Start the Frida server in the `open-igi` bottle if `frida-ps -H 127.0.0.1:27042` cannot list processes. In CrossOver select bottle `open-igi` → **Run Command** and run the server command above. Confirm the output includes `frida-server-x86.exe`.
2. Confirm no `igi.exe` or `open-igi.exe` process is present using `frida-ps -H 127.0.0.1:27042`.
3. Start retail from the game data directory. The executable reads `YMBE.AFP` relative to its working directory, so launching from the repository or spawning through a Frida server whose working directory differs from the game directory fails the retail startup check. Use the exact game executable and `window level<n>` arguments:

   ```sh
   cd /Users/haseeb-mir/Downloads/IGI
   env LC_ALL=C LANG=C /Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine \
     --bottle open-igi 'Z:\Users\haseeb-mir\Downloads\IGI\igi.exe' window level<n>
   ```

   In a second terminal, attach the probe immediately after the game appears:

   ```sh
   /Users/haseeb-mir/.local/bin/frida -H 127.0.0.1:27042 -n igi.exe \
     -l /Users/haseeb-mir/Documents/Code/project-open-igi/scripts/frida_level_probe.js \
     --no-auto-reload -q -t 60 > /Users/haseeb-mir/Documents/Code/project-open-igi/build/runtime-traces/<date>-level<n>-retail/frida.log 2>&1
   ```

   This attach observes calls after attachment and can miss startup/QVM calls. Record that limitation; do not describe this as full startup coverage. Record executable SHA-256, level-script hash, and Frida versions beside the raw output. Do not use Frida `-f` unless its remote spawn working directory is explicitly set to the game data root and the retail relative-file check succeeds.
5. Exit retail normally. Confirm `igi.exe` is absent from the Frida process list before continuing. Do not leave either game in the background.
6. Start OpenIGI from the same game data directory with the same level arguments:

   ```sh
   cd /Users/haseeb-mir/Downloads/IGI
   env LC_ALL=C LANG=C /Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine \
     --bottle open-igi 'Z:\Users\haseeb-mir\Downloads\IGI\open-igi.exe' window level<n>
   ```

   Attach the same probe to `open-igi.exe` and retain its module record beside the scene log. The script deliberately does not install retail VAs into a different binary. Pair its module record with `/Users/haseeb-mir/Downloads/IGI/open-igi-model-scene.log`, which records host scene/model/terrain work. Empty retail-address hooks on OpenIGI are not host method-parity evidence.
7. Exit OpenIGI, confirm it is absent, and compare the two runs for the same level script: startup/load sequence, native calls, parameter/state changes, object/terrain counts, camera orientation/eye height, modifier coverage, player movement/jump/landing, and interaction results. A missing or unhooked function is unknown evidence, not a pass.
8. Repeat the exact retail/OpenIGI pair for three to five levels selected from 1–14, including the active feature level and levels with different terrain, modifier, and object content. A useful initial spread is levels 1, 4, 7, 10, and 14; if a level fails retail startup, preserve the exact failure and replace it with another available level. Never change level scripts between a pair.
9. Run `./build.sh` after code changes. The native CFG gate proves structure only; host CTest covers explicit regressions; only paired game observations establish the declared runtime scope. Preserve failures and rerun the same pair after a fix.

## Frida behavior and recovery

- Use `--no-auto-reload` for recorded CLI captures. A 2026-10-19 capture with its log beside the loaded script repeatedly emitted startup records and reset timers; rerunning with auto-reload disabled avoids this capture reset. Preserve incomplete attempts and require a final summary before treating an interval as complete.

- Server unreachable: restart the server in the `open-igi` CrossOver bottle using the exact `Z:\...\frida-server-x86.exe -l 127.0.0.1:27042` command, then retry `frida-ps`. Do not mistake Wine/CoreSimulator warnings for Frida's endpoint result; the decisive result is whether the process list succeeds.
- Target absent: verify the windowed level command and bottle, then list processes again. Attach with `frida -H 127.0.0.1:27042 -n igi.exe` or `-n open-igi.exe` only when that exact process is visible.
- Attach fails: confirm the module name in `frida-ps`, server/client versions, and that the target has not exited. Restart only the target that is meant to run; preserve the other executable's evidence first.
- Frida reports no calls: verify the image base is `0x00400000`, addresses are retail virtual addresses, hooks installed successfully, and the observation included the relevant action. A late attach can miss startup; spawn before the first event or mark startup coverage incomplete. For OpenIGI, zero retail-address hooks are expected; use host scene logs separately.
- Unknown ABI: initially record entry counts and raw register/stack argument words only. Decode pointers only with independently established layouts and safe memory checks. Do not assign semantics from a plausible pointer or return value.
- Do not use an entry-count trace as an `igi-native-runtime-trace` build input. The latter must be a complete IGI-Probe schema-1 trace bound to the exact retail image.

## Evidence record for each pair

## Native candidate accounting

`./build.sh` runs the strict `native_gate.py --changed` feature gate by default,
followed by source-semantic and paired runtime acceptance for that exact scope.
See [native-feature-verification.md](native-feature-verification.md). Use `./build.sh
--development` for a non-release build whose native candidate set is derived
from address-bound implementations referenced by C/C++ translation units in
the engine and Windows-platform trees; comments, strings, and header
declarations do not count. The set is generated, not maintained as a fixed
count. Adding a referenced implementation with a manifest address increases
it, and removing its last source reference decreases it. Selected addresses
must pass the required native evidence checks before the development build
continues. Development builds are not copied into the game directory.

For a complete IGI-Probe load capture, run
`./build.sh --runtime-trace build/runtime-traces/<capture>.json` selects the
development path and checks
the exact observed retail address set in that capture. Capture retail first,
then OpenIGI, one process at a time; combine evidence from the selected levels
instead of treating a single level's entry counts as complete coverage. Frida
hit counts identify execution in that capture only; they do not prove behavior.

Store a small `comparison.json` beside raw output with: date/time and operator; level and exact script paths/hashes; exact commands and CrossOver bottle; retail/OpenIGI SHA-256 values; Frida client/server versions; loaded image names/bases; attempted/installed/failed hooks; per-method counts and observed values; startup and interaction steps; scene log paths; captures; comparison result; unresolved limitations. Keep facts separate from inferred behavior. Do not promote a native to `VERIFIED` from visual similarity or call counts alone.

## Current scope

The active feature scope is the retail player camera, terrain textures/modifier masks, loading positioned world objects, player movement/jump/fall/ground collision, and evidenced object/terrain interactions. Weapons, AI, damage systems, and sound are excluded. A prior OpenIGI Level 7 scene-log capture loaded positioned tasks and terrain but hit `Skinned MEF palette evaluation is not ported` before its first frame. Treat that as a failing blocker; do not hide it by skipping a skinned object. Do not accept guessed movement constants, invented camera defaults, or substitute collision/interaction behavior as retail parity.

## RE-Agent and analysis tools

The project-specific RE-Agent checkout is `/Users/haseeb-mir/Documents/Code/re-agent`. Its `rea setup --client codex --skill-client codex` registers the checkout's MCP server in Codex and installs its bundled `reverse-engineer-anything` skill. The Codex setup was run on 2026-10-19; restart Codex before expecting the updated MCP registration to replace the server tools already loaded in an existing session. Its environment selects Ghidra 12.1.2, OpenJDK 23, and the writable Swift module cache at `/private/tmp/rea-ghidra-swift-modules`. RE-Agent is an orchestration/evidence layer; its autonomous reconstruction loop can generate candidate code, so do not accept generated logic as the port. Use it to organize evidence, analysis, runtime comparison, and review; native code still must follow the retail implementation and its recorded evidence.

The environment has Binary Ninja MCP, Ghidra MCP, Radare2, angr CFGFast, and Frida integrations. For each native, use only one decompiler: Binary Ninja first, Ghidra only when Binary Ninja is unavailable. The build gate selects hash-valid evidence per native using this preference and checks that selection against Radare2 and angr. RE-Agent and upstream REA are separate projects; report their actual readiness independently from the toolchain paths configured by this repository. Do not install duplicate MCP servers for tools already supplied to this Codex session.

## Verified suspended-startup capture (2026-10-19)

The installed Frida 17.24.0 Python API supports `Device.spawn(..., cwd=...)` on this CrossOver endpoint. A successful retail-only matrix now exists for levels 1, 4 and 7 under `build/runtime-traces/retail-suspended-level*-20261019T*/`. See `docs/fidelity/evidence/retail-suspended-startup-matrix.json` for exact capture paths. This supersedes the assumption that every remote capture must attach after startup; it does not establish OpenIGI parity.

Use the Frida pipx interpreter at `/Users/haseeb-mir/Library/Application Support/pipx/venvs/frida-tools/bin/python`; the default `python3` does not contain Frida. The tested driver is `build/gate-repair/capture_retail_suspended.py`, with retained copies beside the Level 4/7 captures. Invoke it from the repository with a level argument, for example:

```sh
env LC_ALL=C LANG=C '/Users/haseeb-mir/Library/Application Support/pipx/venvs/frida-tools/bin/python' \
  build/gate-repair/capture_retail_suspended.py 4
```

The driver refuses to run when either game is present. It spawns only retail with `window level<n>` and an explicit Windows game-data working directory, attaches while suspended, calls the known Windows `GetCurrentDirectoryW` API to observe the actual directory, and checks all 98 current probe targets before resuming. A failed directory/hook check prevents resume. The driver records 25 seconds, takes an explicit final snapshot, terminates only its owned test process, and confirms no game remains before the next level. Its 98-target expectation is specific to this checkpoint; regenerate and review it when the manifest changes. Do not reuse an old count as evidence of complete current coverage.

Level 1 retained raw entry/return observations. Levels 4/7 additionally observed statically established app-context DWORD globals at `0x005C8BF8`, `0x005C8BF4`, `0x005C8BE4`, and `0x005C8BE8`; these are known scalar addresses, not dereferenced unknown argument pointers. Six app-context entries occurred, with 847 captured scalar observations and no observed argument/EAX/state mismatch. `App_ContextGetDebugged` did not occur and remains unobserved in these game captures. Full negative-byte coverage comes from the separate 1024-case setter differential suite, not from these live startup inputs.

These are Frida observations, not an IGI-Probe schema-1 load trace accepted by `build.sh --runtime-trace`. The finite interval, stationary scene, instrumentation overhead, absence of game-window captures, and missing paired OpenIGI scene run remain explicit limitations. Do not treat the captures as movement, collision, visual, or 30 FPS acceptance.
