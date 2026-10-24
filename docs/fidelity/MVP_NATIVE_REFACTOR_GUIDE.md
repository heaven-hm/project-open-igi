# MVP native refactor guide

This is the work guide for the Project Open IGI tree. It covers the goal, the
state of the tree, the five kinds of gate failure and the fix for each, and the
exact split of the work so the three agents do not edit the same file.

The agents are:

- **Agent A (Cline)** — the models, the bones and the controls.
- **Agent B (CmdCode)** — the terrain, the render, the resources and the script core.
- **Agent C (OpenCode)** — the human player, the configuration and the playable demo.

Read `AGENTS.md` section 10 first. It is the rulebook: the retail executable is
the source of truth, and a native is only as good as the evidence behind it.

## The goal

Every native that the MVP implements must pass the gate at the strict
threshold, with the implementation being an exact transcription of the retail
code rather than something that only looks like it.

```
python3 scripts/native_gate.py --root . --address 0x4AF7B0 \
    --threshold STRUCTURALLY_VERIFIED --cfg-threshold 0.85
```

`--address` is repeatable, so a whole area can be checked in one run:

```
python3 scripts/native_gate.py --root . \
    --address 0x004C6240 --address 0x004C5F00 --address 0x004D14E0 \
    --threshold STRUCTURALLY_VERIFIED --cfg-threshold 0.85
```

The MVP is the 3-D models, the textures, the terrain, the human player, the
`config.qvm` controls and `LOD.qvm`. The gate covers the natives behind them.

## The state of the tree right now

190 natives have an implementation record. **62 pass, 128 fail.** The failures
fall into five kinds:

| Count | The gate says | The fix |
| ----- | ------------- | ------- |
| 94 | `missing verification.behavioral_runner; strict mode needs a live differential-test harness` | The record has no `behavioral_runner` block. Add one, and make sure `scripts/native_diff_runner.py` can run that address. |
| 20 | `implementation changed since its evidence record was reviewed` | The source moved on since the evidence was written. Re-record the evidence. |
| 13 | `status TRANSCRIBED is below required STRUCTURALLY_VERIFIED` (also `ANALYZED`) | The record only claims an analysis. The evidence has to be raised to a structural check. |
| 11 | `angr CFGFast result is inconclusive or failed` | The angr recovery of that exact entry did not work. Either the angr project is stale or the address is not an exact function entry. |
| 7 | `binary_ninja/Radare2 CFG graph similarity 0.175 is below required 0.85` | The two decompilers disagree about the function's shape. One of the two CFG evidence files is wrong. |

Five natives also fail because the port has no real implementation and throws
instead:

```
0x004C6240 Terrain_Load                    "is not transcribed; native loading is unavailable."
0x004C5F00 Terrain_LoadTexturesAndLightmaps "is not transcribed; native texture loading is unavailable."
0x004D14E0 Terrain_TraverseContourQuadtree  "is not transcribed; native traversal is unavailable."
0x00510010 Terrain_DecodeChunkCommands       "is not transcribed; native CMD decoding is unavailable."
0x00512C30 Terrain_Draw                      "is not transcribed; native terrain rendering is unavailable."
```

Those five are the "look alike" code the refactor has to replace with the
transcribed retail behaviour.

## The five fixes in detail

### 1. The missing `behavioral_runner`

Every record needs a `behavioral_runner` block, like the one the other records
already carry:

```json
"behavioral_runner": {
  "path": "scripts/native_diff_runner.py",
  "args": ["--address", "{address}", "--binary", "{binary}",
           "--binary-sha256", "{binary_sha256}",
           "--implementation", "{implementation}",
           "--implementation-sha256", "{implementation_sha256}",
           "--root", "{root}"],
  "timeout_seconds": 360
}
```

The runner refuses an address it does not know:

```
behavioral runner exited with code 1; stderr='unsupported native address: 0x00517890'
```

So for each address the runner has to learn, add it to the tables at the top of
`scripts/native_diff_runner.py` (`NATIVES`, `GETTERS` and the named address
constants) and give it the harness it needs. The runner builds a small x86
harness that calls the candidate and compares it with the retail under Frida;
the families that are already supported show the pattern.

Two runner problems were environmental, not code:

```
stderr='set IGI_ANGR_PYTHON to a Python environment with angr installed'
stderr='angr Python is unavailable; configure IGI_ANGR_PYTHON'
stderr="[Errno 2] No such file or directory: '/Users/haseeb-mir/Documents/Code/re-agent/.venv/bin/python'"
```

**This one is already fixed.** The three runner scripts
(`native_diff_runner.py`, `native_abi_diff_runner.py` and
`native_heaplist_diff_runner.py`) looked for angr in the re-agent checkout only,
while the gate uses the project's own `.venv-angr`. They now look for
`$ROOT_DIR/.venv-angr/bin/python` first, which unblocked the allocator, heap
list and string natives:

```
python3 scripts/native_gate.py --root . --address 0x004AA670 --address 0x004AA682 \
    --address 0x004AA6AE --address 0x004AF8F0 --address 0x004AF960 \
    --address 0x004B1E90 --threshold STRUCTURALLY_VERIFIED --cfg-threshold 0.85
```

`IGI_ANGR_PYTHON` still overrides the choice when it is set.

### 2. The stale evidence hash

The record pins the sha256 of the implementation file. When the file changes,
the evidence is stale and the gate says so. `scripts/repair_manifest_references.py`
refreshes the recorded hashes of the implementation, the GameNatives file and
the evidence files:

```
python3 scripts/repair_manifest_references.py
```

Run it after every source change, and re-run the gate afterwards.

### 3. The status below the threshold

`TRANSCRIBED` and `ANALYZED` are not enough. The record has to reach
`STRUCTURALLY_VERIFIED`, which needs the decompiler and Radare2 CFG evidence,
the semantic review and the build report. `scripts/generate_native_evidence.py`
produces all of them for a list of natives:

```
python3 scripts/generate_native_evidence.py /tmp/my-natives.json
```

The input is a JSON list; each entry needs the address, the name, the
implementation file, the GameNatives file and the signature:

```json
[{"address": "0x00517890", "name": "Iff_FindFormChunk",
  "implementation": "engine/src/retail/model/iff_chunk_tree.cpp",
  "game_natives": "scripts/GameNatives/terrain/00517890-Iff_FindFormChunk.cpp",
  "signature": "void*(void*, uint32_t)"}]
```

The generator writes the Binary Ninja and Radare2 CFG evidence, the semantic
review and the build report, and it records the native in the manifest. It also
keeps the paired runtime evidence an earlier run recorded.

The generator writes the whole manifest, so **do not run it at the same time as
the other agent**. Use `--manifest` to write a shard instead, and merge the
shards when both are done:

```
python3 scripts/generate_native_evidence.py /tmp/my-natives.json \
    --manifest build/manifest-shard-b.json
```

Merge the shards into the real manifest when both agents are done:

```
python3 scripts/merge_manifest_shards.py build/manifest-shard-*.json
```

### 4. The angr recovery

```
angr CFGFast result is inconclusive or failed: CFGFast did not recover an exact function entry
```

angr recovers the entries from a project. When it fails for one address, check
whether the address really is a function entry in `igi.exe` (a jump target into
the middle of a function is not) and whether the angr project is the one for
the retail image:

```
python3 scripts/angr_analyze.py --binary /Users/haseeb-mir/Downloads/IGI/igi.exe \
    --address 0x004AD004
```

`IGI_ANGR_PYTHON` selects the interpreter that has angr, and it has to be a
different environment from the re-agent checkout.

### 5. The CFG similarity below 0.85

The gate compares the Binary Ninja CFG with the Radare2 CFG and wants a Jaccard
similarity of at least 0.85 on the nodes and on the edges. A score of 0.175
means one of the two evidence files does not describe the function:

* The node list has one entry per basic block, with the block's start address.
* The edge list has `source`, `target` and `kind` per edge.

The same address often has two evidence files, one per tool, and only one of
them is wrong. Compare them before re-recording.

## The work split

The tree is shared. These are the areas; do not cross them.

**Agent A (Cline) — the models, the bones and the controls**

| Area | Files |
| ---- | ----- |
| MEF models and bones | `engine/src/retail/model/**`, `engine/include/igi/retail/model/**` |
| The IFF and bone rig | `engine/src/retail/model/iff_*.cpp`, `mef_bone_rig.cpp` |
| The input port and the controls | `engine/src/retail/input/**` |
| The QVM object reader | `engine/src/retail/script/qvm_object_calls.cpp` |
| The scene | `engine/platform/windows/retail_model_scene.cpp` |
| Their natives | `0x00517890`, `0x00517940`, `0x005179C0`, `0x005178E0`, `0x004D67A0`, `0x004037C0`, `0x00403870`, `0x004038E0`, `0x004108C0`, `0x00408460` |
| Their GameNatives | `scripts/GameNatives/terrain/005178*`, `scripts/GameNatives/terrain/005179*`, `scripts/GameNatives/render/004D67A0*`, `scripts/GameNatives/go/004037*`, `scripts/GameNatives/go/004038*`, `scripts/GameNatives/humanplayer/**` |
| Their documents | `docs/fidelity/mef_bone_rig.md`, `docs/fidelity/retail_input_chain_and_camera.md` |

**Agent B (CmdCode) — the terrain, the render, the resources and the script core**

| Area | Files |
| ---- | ----- |
| The terrain | `engine/src/retail/terrain/**` |
| The render and the camera | `engine/src/retail/render/**`, `engine/src/retail/core/**` |
| The resources, the device and the textures | `engine/src/retail/resource/**`, `engine/src/retail/sys/**`, `engine/src/retail/texture/**` |
| The memory | `engine/src/retail/mem/**` |
| The dynamic objects and the packages | `engine/src/retail/dynobj/**` |
| The script core | `engine/src/retail/script/qvm_loader.cpp`, `qvm_compiled_program.cpp`, `qvm_native_loader.cpp` |
| Their natives | everything in the manifest that is not in Agent A's list, in particular the five `Terrain_*` natives that are not transcribed: `0x004C6240`, `0x004C5F00`, `0x004D14E0`, `0x00510010`, `0x00512C30` |
| Their GameNatives | every `scripts/GameNatives/**` directory except the ones Agent A owns and the ones Agent C owns |

**Agent C (OpenCode) — the human player, the configuration and the playable demo**

The demo is `open-igi.exe window levelN` played next to `igi.exe window levelN`:
the human walks, runs, jumps and crouches over the ported terrain, driven by the
retail `config.qvm` bindings and the `humanplayer.qvm` parameters, with the
camera on the retail eye position. Agent C owns the human movement chain and the
configuration that feeds it.

| Area | Files |
| ---- | ----- |
| The human movement | `engine/src/retail/human/**`, `engine/include/igi/retail/human/**` |
| The configuration and the archive aliases | `engine/src/retail/config/**`, `engine/src/retail/qfile/**` and their headers |
| The host player loop | `engine/platform/windows/main.cpp`, `player_controller.cpp`, `player_controller.hpp`, `developer_controls.hpp` |
| Their natives | `0x004614B0` `Human_MovementIntegrator`, `0x00462390` `Human_GroundQuery`, `0x00461A70` `Human_WallSweep`, `0x00462BA0` `Human_Separation`, `0x00462CF0` `Human_AttachedMovement`, `0x00463920` body-footstep handler, the recorded `0x00463180`, `0x00465820`, `0x004659E0`, and the `Config_*` set `0x00404590`, `0x00404F00`, `0x00404F60`, `0x00405530`, `0x004055C0`, `0x00405620` |
| Their GameNatives | `scripts/GameNatives/human/**`, `scripts/GameNatives/config/**`. **Not** `scripts/GameNatives/humanplayer/**` — that is Agent A's |
| Their documents | `docs/fidelity/human_player_movement.md`, this guide |
| The verification tooling | `scripts/native_gate.py`, `scripts/angr_analyze.py`, `scripts/native_verification_queue.py`, `scripts/repair_manifest_references.py`, `scripts/merge_manifest_shards.py` |

Agent C does not edit `engine/src/retail/{model,input,terrain,render,core,resource,sys,texture,mem,dynobj}/**`,
`engine/src/retail/script/qvm_object_calls.cpp`, `qvm_loader.cpp`,
`qvm_compiled_program.cpp`, `qvm_native_loader.cpp`, the `humanplayer` GameNatives,
or Agent A's and Agent B's natives. **Exception, authorized by the user on
2026-10-20:** Agent C wired the config/humanplayer call site in
`engine/platform/windows/retail_model_scene.cpp` (§6.1 of
`human_player_movement.md`) because it was the only place the bindings could reach the
input port; do not change that block again without coordinating with Agent A. Agent C
may include A's model and skeleton types read-only and B's script reader read-only;
when a change is needed elsewhere, Agent C asks the owner.

**The movement contract**

1. `config.qvm` supplies the action bindings and the mouse sensitivity. The
   binding read is Agent C's `engine/src/retail/config/**` module; its result is
   installed on Agent A's input port at the `Scene::load` call site wired with
   the user's authorization on 2026-10-20. Agent C owns the `config/**` module
   for the non-input rows too, and coordinates any API change with Agent A.
2. `humanplayer.qvm` `DefineHumanPlayerGeneral` supplies the general scale
   `1.75`, the jump `17.5` horizontal / `27` vertical, and the air control `0.5`
   recorded in the file, plus the per-state factors. Agent C decodes them from
   the file; they are not typed into the controller.
3. `LOD.QVM` supplies the view-distance thresholds. Agent C owns decoding them;
   Agent B's terrain consumes them and does not re-read the file.
4. Ground displacement is the retail animation root motion at `human+0x520`,
   scaled by the `DefineHumanPlayerGeneral` general factor and passed to
   `Human_MovementIntegrator` (`0x004614B0`) with the velocity, exactly as
   `Human_RunUpdate` (`0x004109A0`) does. There is **no fallback speed**. The
   provisional `8.75` / `13.125` km/h stand-in in
   `docs/fidelity/retail_input_chain_and_camera.md` must not return; the
   controller keeps failing loudly until the clip drives it.
5. The demo is accepted only on a paired retail/candidate run of the same level
   with the same inputs: feet, eye, yaw, pitch, ground contact and crouch state
   compared, plus the rendered frame. Standing still, walking into a wall, a
   slope, a jump, a landing and a crouch are the minimum five steps.

**Agent C's order**

1. Decode `config.qvm`, `humanplayer.qvm` and `LOD.QVM` into typed parameter
   structures with tests against the original files. **Done**:
   `engine/src/retail/config/retail_config.cpp` and `retail_input_bindings.cpp`,
   tested by `engine/tests/config/retail_config_test.cpp` against the shipped
   files; see `docs/fidelity/human_player_movement.md`. The decoded file hashes
   and text live in `docs/fidelity/evidence/agent-c-config-20261020/`. The scene
   now applies the 27 bindings and installs the four `humanplayer.qvm`
   parameters; runtime-observed on `level1` in
   `docs/fidelity/evidence/agent-c-runtime-20261020/`.
2. Transcribe the human movement natives into `engine/src/retail/human/`, and
   extend `scripts/native_diff_runner.py` with the `human/` and `config/`
   address blocks only.
3. Connect the animation root-motion track to the controller, so the ground
   displacement is the clip's root-bone delta and not a constant.
4. Record the paired runtime comparison in `docs/fidelity/human_player_movement.md`
   and only then raise the movement records above `STRUCTURALLY_VERIFIED`.
5. Keep the gate tooling honest while doing it: the environment, the angr
   recovery, the CFG evidence and the shard merge are Agent C's to fix, not to
   weaken.

**Shared, so coordinate before editing**

| File | Rule |
| ---- | ---- |
| `docs/fidelity/native-verification.json` | One file for all three agents. Use `--manifest` shards and merge at the end. Never edit the same record at the same time. |
| `scripts/native_diff_runner.py` | Agent B adds the address families it needs; Agent A adds the IFF and bone ones (`0x00517890`, `0x00517940`, `0x005179C0`, `0x005178E0`, `0x004D67A0`); Agent C adds the `human/` and `config/` ones. Different blocks in the same file, so apply the change and do not reformat the rest. |
| `scripts/generate_native_evidence.py` | Agent A owns it. Agent B and Agent C ask for a record to be generated rather than editing the script. |
| `scripts/merge_manifest_shards.py` | Shared. All three agents write shards and merge at the end; never two agents write the main manifest at once. |
| `engine/CMakeLists.txt` | Each agent adds only its own sources. |

## The rules that matter

1. **Transcribe, do not approximate.** The implementation has to be the retail
   code. The decompiler output is the starting point, not the answer: check it
   against the disassembly, and check the disassembly against the retail
   behaviour.
2. **No look alike code.** A helper that throws "not transcribed" is honest but
   it is not an implementation. Where the port has a host helper, it either
   becomes the transcribed native or it is clearly marked as a host adapter
   with the reason it cannot be the native yet.
3. **Keep the interface.** The calling convention, the argument order, the
   return value and the memory layout stay as the retail has them.
4. **Every change needs evidence.** The disassembly reference, the Radare2 CFG,
   the angr entry, the semantic review, the build result and the runtime
   observation.
5. **The name comes from the retail body.** A decompiler's auto name is a
   guess. The names in `scripts/GameNatives` and in the catalog have to be
   justified by what the code does.
6. **Do not bypass the gate.** No `--no-verify`, no commented-out checks, no
   lowered threshold to get a green build.

## Running the two agents at the same time

The retail game and the reconstruction both take the retail's IGIMUTEX, and only
one of them can drive the display. Two agents that run the games at the same
time starve each other: the retail instance that starts second never reaches the
natives, the probe sees nothing, and the behavioural runner fails or times out.

**Take turns.** The rule that works:

* The agent that is checking runtime evidence owns the display. The other agent
  does static work, writes code and records evidence.
* Before a runtime run, check that no game is running:
  `ps -eo pid,command | grep -E "[i]gi\.exe|[o]pen-igi\.exe"`.
* A runner must never kill somebody else's game. The IFF runner
  (`scripts/native_iff_diff_runner.py`) retries the side instead, so a busy
  display costs a retry rather than a failure.
* A retail instance that is left behind holds the mutex and blocks the next one,
  so a run that is abandoned has to be cleaned up:
  `pkill -9 -f igi.exe; pkill -9 -f open-igi.exe`.

## The tools, and what was fixed in them

The four analysis tools the project uses, and the state of each:

| Tool | How it is reached | State |
| ---- | ----------------- | ----- |
| Binary Ninja MCP | `python3 scripts/bn_mcp_call.py <tool> '<json>'` over `http://127.0.0.1:24642/mcp` | working; the retail PE is open as `view_1` |
| Radare2 | `r2` 6.2.0, and the CFG evidence files | working |
| Ghidra Headless MCP | `python3 scripts/ghidra_mcp_call.py <method> '<json>'` over `127.0.0.1:8771` | working; 212 tools |
| angr | `IGI_ANGR_PYTHON=.venv-angr/bin/python python3 scripts/angr_analyze.py ...` | working; 10.0.4.post1 |
| Frida | `frida-ps -H 127.0.0.1:27042`, the server in the CrossOver bottle | working, but it wedges when a game is killed under it |

**The Ghidra MCP needs starting by hand.** It was down. It speaks line-delimited
JSON-RPC over a raw TCP socket, not the HTTP transport the Binary Ninja MCP
uses, so `scripts/ghidra_mcp_call.py` was added for it:

```
GHIDRA_INSTALL_DIR=/opt/homebrew/opt/ghidra/libexec \
    /opt/homebrew/bin/python3.12 -m ghidra_headless_mcp \
    --transport tcp --host 127.0.0.1 --port 8771 &
```

**angr no longer collides.** The analysis used the retail binary itself as the
project path, and angr keeps the knowledge base next to the file it is given,
so two runs shared one knowledge base and the second CFGFast replaced the
first one's functions. The first run then reported nothing. Each run now
analyses its own copy in its own directory. Two concurrent runs both pass:

```
.venv-angr/bin/python scripts/angr_analyze.py --binary .../igi.exe \
    --address 0x00517890 --address 0x00517940 &
.venv-angr/bin/python scripts/angr_analyze.py --binary .../igi.exe \
    --address 0x005179C0 --address 0x004D67A0 &
```

The old knowledge bases in the game folder (`igi.exe_angr_rtdb*`) are left over
from the shared project and were removed.

**The frida server restarts itself.** The IFF runner restarts the server when
it cannot connect, because the server wedges when a game is killed under it and
the next run then fails to attach.

## The order that works

1. Fix the environment first: `IGI_ANGR_PYTHON` has to point at an interpreter
   with angr, otherwise 94 records cannot be checked at all.
2. Extend `scripts/native_diff_runner.py` for the address families you own.
3. Add the `behavioral_runner` block to the records you own.
4. Re-record the evidence for the records whose source moved
   (`scripts/repair_manifest_references.py`, then
   `scripts/generate_native_evidence.py`).
5. Raise the status of the records that only claim an analysis.
6. Work through the angr and CFG failures one address at a time.
7. Replace the five `Terrain_*` stubs with the transcribed behaviour.
8. Re-run the gate for your addresses and confirm the OK line.

The end state for each address is:

```
native_gate: OK — N changed native(s) passed required binary/source/evidence checks
```
