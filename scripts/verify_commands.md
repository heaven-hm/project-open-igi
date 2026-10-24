# Verification commands

The commands the project uses to check a native, record its evidence and run the
game. Every command here has been run from the repository root.

```bash
cd /Users/haseeb-mir/Documents/Code/project-open-igi
```

## The native gate

The gate is the authority. It checks the manifest record, the Binary Ninja and
Radare2 CFG evidence, the angr entry recovery, the semantic review, the build
report and the live behavioural differential.

### One native

```bash
python3 scripts/native_gate.py --root . --address 0x4AF7B0 \
    --threshold STRUCTURALLY_VERIFIED --cfg-threshold 0.85
```

The success line is:

```
native_gate: OK — 1 changed native(s) passed required binary/source/evidence checks
```

### A group of natives

`--address` repeats, so one area is checked in one run:

```bash
python3 scripts/native_gate.py --root . \
    --address 0x00517890 --address 0x00517940 --address 0x005179C0 \
    --address 0x005178E0 --address 0x004D67A0 \
    --threshold STRUCTURALLY_VERIFIED --cfg-threshold 0.85
```

### Everything the tree touches

```bash
python3 scripts/native_gate.py --root . --changed \
    --threshold VERIFIED --cfg-threshold 0.85 \
    --require-semantic-evidence --require-behavioral-tests
```

### The strict feature check

The default the project requires for a feature, from `AGENTS.md` section 10.7:

```bash
python3 scripts/native_gate.py --root . --changed --threshold VERIFIED \
    --cfg-threshold 0.85 --require-semantic-evidence --require-behavioral-tests
```

Do not replace `--changed` with `--production-tree`: `--changed` selects the
edited native sources, and the required addresses come from that report.

## angr

The gate runs angr itself. To check one address on its own:

```bash
.venv-angr/bin/python scripts/angr_analyze.py \
    --binary /Users/haseeb-mir/Downloads/IGI/igi.exe --address 0x00517890
```

It prints `result: pass` and, per address, the recovered `function_entry`, the
`recovery_mode` (`automatic` or `seeded`) and `has_unresolved_jumps`.

The interpreter is selected with `IGI_ANGR_PYTHON`; the project default is
`$ROOT_DIR/.venv-angr/bin/python`. The analysis uses a **copy** of the binary in
its own directory, because angr keeps the knowledge base next to the file it is
given and two runs that share it replace each other's functions.

Two concurrent runs are safe now:

```bash
.venv-angr/bin/python scripts/angr_analyze.py --binary .../igi.exe \
    --address 0x00517890 --address 0x00517940 &
.venv-angr/bin/python scripts/angr_analyze.py --binary .../igi.exe \
    --address 0x005179C0 --address 0x004D67A0 &
```

## The evidence

### Record the evidence for a set of natives

The generator writes the Binary Ninja and Radare2 CFG evidence, the semantic
review and the build report, and it records the native in the manifest. It
preserves the behavioural runner and the paired evidence an earlier run
recorded.

```bash
cat > /tmp/natives.json <<'EOF'
[{"address": "0x00517890", "name": "Iff_FindFormChunk",
  "implementation": "engine/src/retail/model/iff_chunk_tree.cpp",
  "game_natives": "scripts/GameNatives/terrain/00517890-Iff_FindFormChunk.cpp",
  "signature": "void*(void*, uint32_t)"}]
EOF

python3 scripts/generate_native_evidence.py /tmp/natives.json
```

The input is a list of:

| Field | Meaning |
| ----- | ------- |
| `address` | the retail virtual address |
| `name` | the name the native carries |
| `implementation` | the engine file that implements it |
| `game_natives` | the GameNatives reference file |
| `signature` | the calling convention and the argument list |

To write a shard instead of the shared manifest, so two agents do not write the
same file:

```bash
python3 scripts/generate_native_evidence.py /tmp/natives.json \
    --manifest build/manifest-shard-a.json
```

### Repair the manifest references

Refreshes the recorded hashes of the implementation file, the GameNatives file
and the evidence files, and realigns the names with the confirmed catalog. Run it
after every source change and before the gate:

```bash
python3 scripts/repair_manifest_references.py
```

### Merge the manifest shards

```bash
python3 scripts/merge_manifest_shards.py build/manifest-shard-*.json
```

## The decompilers

### Binary Ninja MCP

```bash
python3 scripts/bn_mcp_call.py bn_binary_view_info '{}'
python3 scripts/bn_mcp_call.py bn_function_decompile '{"function":"0x00517890"}'
python3 scripts/bn_mcp_call.py bn_function_basic_blocks '{"function":"0x00517890"}'
python3 scripts/bn_mcp_call.py bn_function_callees '{"function":"0x004D67A0"}'
python3 scripts/bn_mcp_call.py bn_function_callers '{"function":"0x004D67A0"}'
python3 scripts/bn_mcp_call.py bn_function_info '{"function":"0x004D67A0"}'
```

The server is at `http://127.0.0.1:24642/mcp`.

### Ghidra Headless MCP

The server is **line-delimited JSON-RPC over a raw TCP socket**, not the HTTP
transport the Binary Ninja MCP uses, so it needs its own client. Start it:

```bash
GHIDRA_INSTALL_DIR=/opt/homebrew/opt/ghidra/libexec \
    /opt/homebrew/bin/python3.12 -m ghidra_headless_mcp \
    --transport tcp --host 127.0.0.1 --port 8771 &
```

Then call it:

```bash
python3 scripts/ghidra_mcp_call.py tools/list
python3 scripts/ghidra_mcp_call.py tools/call '{"name":"health.ping","arguments":{}}'
python3 scripts/ghidra_mcp_call.py tools/call \
    '{"name":"program.list_open","arguments":{}}'
python3 scripts/ghidra_mcp_call.py tools/call \
    '{"name":"function.at","arguments":{"address":"0x004D67A0"}}'
python3 scripts/ghidra_mcp_call.py tools/call \
    '{"name":"decomp.function","arguments":{"function_start":"0x004D67A0"}}'
```

It has 212 tools. The decompiler for a target that Binary Ninja has not been run
on, or the second opinion the project wants on a leaf.

### Radare2

The gate compares the Binary Ninja CFG with the Radare2 CFG, both read from the
evidence files. To re-derive the r2 facts independently:

```bash
python3 scripts/r2_cfg_recheck.py
r2 -q -e bin.relocs.apply=true -c "s 0x00517890; af-*; af; afbj" \
    /Users/haseeb-mir/Downloads/IGI/igi.exe
```

`af-*` before `af` matters: an earlier `af` leaves a function that swallows the
next address.

## The runtime evidence

### The retail game and the candidate, side by side

Both take the retail's IGIMUTEX and only one of them can drive the display, so
the two runs are **sequential**, never concurrent.

```bash
cd /Users/haseeb-mir/Downloads/IGI
env LC_ALL=C LANG=C \
    /Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine \
    --bottle open-igi 'Z:\Users\haseeb-mir\Downloads\IGI\igi.exe' window level4

# wait for the first process to exit
ps -eo pid,command | grep -E "[i]gi\.exe|[o]pen-igi\.exe"

env LC_ALL=C LANG=C \
    /Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine \
    --bottle open-igi 'Z:\Users\haseeb-mir\Downloads\IGI\open-igi.exe' window level4
```

The scene log is `open-igi-model-scene.log` in the game directory.

### The frida server

It lives in the CrossOver bottle and wedges when a game is killed under it:

```bash
cd /Users/haseeb-mir/Downloads/IGI
env LC_ALL=C LANG=C \
    /Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine \
    --bottle open-igi 'Z:\Users\haseeb-mir\.cache\frida-crossover\frida-server-x86.exe' \
    -l 127.0.0.1:27042 &

export PATH="$HOME/Library/Application Support/pipx/venvs/frida-tools/bin:$PATH"
frida-ps -H 127.0.0.1:27042
```

### The paired probe for the MEF bones

```bash
"$HOME/Library/Application Support/pipx/venvs/frida-tools/bin/python" \
    scripts/mef_bones_paired_probe.py --level 4 --duration 25
```

It runs the same probe against `igi.exe` and `open-igi.exe` and writes
`build/mef_bones_paired_probe.json`.

### The IFF differential runner

The runner the gate calls for the five IFF natives:

```bash
"$HOME/Library/Application Support/pipx/venvs/frida-tools/bin/python" \
    scripts/native_iff_diff_runner.py --address 0x005179C0 \
    --binary /Users/haseeb-mir/Downloads/IGI/igi.exe \
    --binary-sha256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01 \
    --implementation engine/src/retail/model/iff_chunk_tree.cpp \
    --implementation-sha256 $(shasum -a 256 engine/src/retail/model/iff_chunk_tree.cpp | awk '{print $1}') \
    --root .
```

It prints one JSON report and exits zero when the candidate's chunk walk is the
retail's walk.

## The build and the tests

```bash
# the gated build
./build.sh

# the ungated diagnostic build, for a quick loop
./build_no_verify.sh

# the host build without the gate
cmake -S . -B build-host -DIGI_ENABLE_NATIVE_GATE=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-host -j"$(sysctl -n hw.ncpu)"

# the tests
cd build-host && ctest -j4
cd build && ctest -R igi_native_verification_gate
```

## The tool health check

```bash
python3 scripts/bn_mcp_call.py bn_binary_view_info '{}'
python3 scripts/ghidra_mcp_call.py tools/call '{"name":"health.ping","arguments":{}}'
r2 -v | head -1
.venv-angr/bin/python scripts/angr_analyze.py \
    --binary /Users/haseeb-mir/Downloads/IGI/igi.exe --address 0x00517890 | head -3
export PATH="$HOME/Library/Application Support/pipx/venvs/frida-tools/bin:$PATH"
frida-ps -H 127.0.0.1:27042
```

## Cleaning up

A retail instance that is left behind holds the mutex and blocks the next one:

```bash
pkill -9 -f igi.exe
pkill -9 -f open-igi.exe
```

The angr knowledge bases the old shared project left in the game folder:

```bash
rm -rf /Users/haseeb-mir/Downloads/IGI/igi.exe_angr_rtdb*
```
