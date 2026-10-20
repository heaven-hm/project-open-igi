# Repeatable native-fix workflow

Keep `build.sh` as the strict acceptance entry point. It retains `native_gate.py --changed`, semantic checks, behavioral checks and the VERIFIED/0.85 requirements. A diagnostic build is explicitly separate:

```sh
./build.sh
./build_no_verify.sh
python3 scripts/native_verification_queue.py --gate-report build/native-verification.json > build/native-queue-summary.json
```

The queue groups duplicate source/address records without discarding failures. For a single engine method, use its focused gate report, not a report containing multiple reference copies of that address:

```sh
python3 scripts/native_verification_queue.py --gate-report build/runtime-traces/texture-modifier-20261020/runtime-tested-gate.json --address 0x00505E40 --run --resume --output build/modifier-focused-queue
```

The optional runner executes exact methods sequentially at VERIFIED/0.85 with semantic and behavioral checks enabled. It preserves logs/reports and rejects missing requested addresses. Reuse requires unchanged source/evidence, retail identity, compiler flags and executable tool hashes. Missing tool identity disables reuse. It never edits native code or statuses, launches games or creates a release attestation.

For the reviewed TextureModifier_Push live fixture scope, build the diagnostic executable and use the same probe sequentially in both games:

```sh
FRIDA_HOST=127.0.0.1:27043 '/Users/haseeb-mir/Library/Application Support/pipx/venvs/frida-tools/bin/python' scripts/capture_texture_modifier_probe.py --candidate build-win32-no-verify/engine/platform/windows/open-igi.exe --level 1 --fixtures docs/fidelity/evidence/texture-modifier-push-20261020/after.json
```

Repeat with levels 4 and 7. The local 27043 server is independently owned; the shared 27042 endpoint was unresponsive in this session. Existing game sessions block capture; only explicitly authorized sessions may be closed. Both executables receive `window levelN` from the IGI data directory. Only owned test processes are cleaned up, and native globals are restored before game resume.

This probe covers explicit nonfatal agent-thread fixtures. It separately reports natural call counts; zero calls never establish scene integration. The bounded x86 replay covers the fatal caller loop using an explicit Error_Show boundary model. Neither replay nor fixture equality proves terrain appearance, locomotion, collision, or whole-game equivalence. Preserve those gaps until matching observations exist.
