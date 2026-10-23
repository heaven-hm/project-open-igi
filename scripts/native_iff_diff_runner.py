#!/usr/bin/env python3
"""Differential runner for the retail IFF chunk navigation and the bone rig.

The retail reads the character skeletons out of the retail animation files: the
BOBJ form, the BOBH form and then the BOSH, PLST and TLST chunks. The port
reads the same tree, and the comparison is the ordered chunk walk both sides
perform.

The retail also parses the animation clips inside the same file, so it calls
the navigation far more often than the port, which reads the skeleton only. The
oracle is therefore that the candidate's walk is the retail's walk, truncated
where the port stops.

    python3 scripts/native_iff_diff_runner.py --address 0x00517890 ...
"""

import argparse
import hashlib
import json
import os
import pathlib
import subprocess
import sys
import time

ROOT = pathlib.Path(__file__).resolve().parent.parent
PROBE = ROOT / "scripts" / "frida_mef_bones_probe.js"
SYMBOL_MAP = ROOT / "assets" / "candidate_symbol_map.json"
WINE = pathlib.Path.home() / "Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine"
GAME = pathlib.Path.home() / "Downloads/IGI"
FRIDA_HOST = "127.0.0.1:27042"
RETAIL_IMAGE_BASE = 0x400000
DURATION_SECONDS = int(os.environ.get("IGI_IFF_PROBE_SECONDS", "20"))
PROBE_DEADLINE_SECONDS = int(os.environ.get("IGI_IFF_PROBE_DEADLINE", "150"))

# The retail virtual address of every native in this family, and the name the
# port gives the same behaviour.
FAMILY = {
    "0x00517890": "Iff_FindFormChunk",
    "0x00517940": "Iff_FindChunk",
    "0x005179C0": "Iff_GetChunkPayload",
    "0x005178E0": "Iff_FindChunkBelowForm",
    "0x004D67A0": "AnimIff_LoadAnimationFile",
}


def digest(path: pathlib.Path) -> str:
    value = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1 << 16), b""):
            value.update(block)
    return value.hexdigest()


def candidate_symbols(candidate_exe: pathlib.Path) -> dict:
    entries = json.loads(SYMBOL_MAP.read_text())
    symbols = {entry["address"].lower(): entry for entry in entries}
    nm = subprocess.run(
        ["i686-w64-mingw32-nm", "--defined-only", str(candidate_exe)],
        capture_output=True, text=True, check=True, timeout=60,
    )
    candidate_addresses = {}
    for line in nm.stdout.splitlines():
        fields = line.split()
        if len(fields) == 3 and fields[1] in ("T", "t"):
            candidate_addresses[fields[2]] = f"0x{int(fields[0], 16):08X}"
    missing = []
    for address in FAMILY:
        entry = symbols.get(address.lower())
        if entry is None:
            missing.append(address)
            continue
        candidate_symbol = entry.get("candidateSymbol") or {}
        symbol = candidate_symbol.get("symbol")
        if not entry.get("implemented") or not symbol or symbol not in candidate_addresses:
            missing.append(f"{address} ({symbol or 'no symbol'})")
        else:
            entry["candidateSymbol"] = {
                **candidate_symbol,
                "vaddr": candidate_addresses[symbol],
            }
    if missing:
        raise RuntimeError(
            f"candidate executable lacks mapped IFF-family symbols: {', '.join(missing)}"
        )
    return symbols


def build_scheme(mode: str, symbols: dict, candidate_module: str = "open-igi.exe") -> dict:
    targets = []
    for address, name in FAMILY.items():
        if mode == "retail":
            targets.append({"name": name, "module": "igi.exe", "vaddr": int(address, 16)})
            continue
        entry = symbols.get(address.lower())
        if entry is None or not entry.get("candidateSymbol"):
            raise RuntimeError(f"candidate symbol mapping is missing for {address} ({name})")
        targets.append(
            {
                "name": name,
                "module": candidate_module,
                "vaddr": int(entry["candidateSymbol"]["vaddr"], 16),
                "symbol": entry["candidateSymbol"]["symbol"],
            }
        )
    return {"imageBase": RETAIL_IMAGE_BASE, "mode": mode, "targets": targets}


def windows_z_path(path: pathlib.Path) -> str:
    return "Z:" + str(path.resolve()).replace("/", "\\")


def crossover_environment() -> dict[str, str]:
    environment = os.environ.copy()
    environment.update({"LC_ALL": "C", "LC_CTYPE": "C", "LANG": "C"})
    return environment


def find_test_process(device, process_name: str, baseline_pids: set[int]):
    matching = [
        entry for entry in device.enumerate_processes()
        if entry.name.lower() == process_name and entry.pid not in baseline_pids
    ]
    if len(matching) > 1:
        raise RuntimeError(
            f"multiple new {process_name} processes appeared; refusing to guess which one this run owns"
        )
    return matching[0] if matching else None


class GameAlreadyRunningError(RuntimeError):
    """A paired run must never attach to or terminate a pre-existing game."""


def run_side(
    mode: str,
    executable: str,
    level: int,
    symbols: dict,
    candidate_exe: pathlib.Path,
) -> dict:
    import frida

    device = frida.get_device_manager().add_remote_device(FRIDA_HOST)
    game_names = {"igi.exe", "open-igi.exe"}
    baseline = device.enumerate_processes()
    existing = [entry for entry in baseline if entry.name.lower() in game_names]
    if existing:
        names = ", ".join(f"{entry.name} (pid {entry.pid})" for entry in existing)
        raise GameAlreadyRunningError(
            f"close existing game sessions before the paired probe; leaving them untouched: {names}"
        )
    if mode == "retail":
        executable_path = GAME / "igi.exe"
    else:
        executable_path = candidate_exe.resolve()
    if not executable_path.is_file():
        raise FileNotFoundError(f"test executable does not exist: {executable_path}")

    observed: list = []
    target_pid = None
    session = None
    captured: dict = {}
    try:
        windows_executable = windows_z_path(executable_path)
        # Frida spawns suspended in the existing CrossOver bottle. Attach and
        # install hooks before resume so startup/load-time calls are observed.
        # cwd is explicitly the retail game data root used by the paired test.
        target_pid = device.spawn(
            windows_executable,
            argv=[windows_executable, "window", f"level{level}"],
            cwd=windows_z_path(GAME),
        )
        session = device.attach(target_pid)
        scheme = json.dumps(build_scheme(mode, symbols, executable_path.name))
        source = PROBE.read_text().replace("Process.argv[2]", json.dumps(scheme))
        script = session.create_script(source)
        script.on("message", lambda message, _data: captured.setdefault("messages", []).append(message))
        script.load()
        device.resume(target_pid)
        time.sleep(DURATION_SECONDS)
        captured.update(script.exports_sync.stop())
        observed = captured.get("observed", [])
        escape = captured.get("escape", {})
        hooks = captured.get("hooks", [])
        if not hooks:
            hook_messages = [
                message.get("payload", {}).get("hooks", [])
                for message in captured.get("messages", [])
                if message.get("type") == "send"
                and message.get("payload", {}).get("type") == "hook-installation"
            ]
            hooks = hook_messages[-1] if hook_messages else []
        installed = sum(1 for hook in hooks if hook.get("installed"))
        print(f"{mode} hook status: {installed}/{len(hooks)} installed; details={hooks}", file=sys.stderr)
        print(f"{mode} process launch context: {captured.get('launchContext', {})}", file=sys.stderr)
        if mode == "retail" and not escape.get("posted"):
            raise RuntimeError(f"Escape was not posted to the retail game window: {escape}")
    finally:
        if session is not None:
            try:
                session.detach()
            except Exception:  # noqa: BLE001 - process may already have exited
                pass
        if target_pid is not None:
            try:
                device.kill(target_pid)
            except Exception:  # noqa: BLE001 - process may already have exited
                pass
        # The Frida device PID is the only process this run is allowed to kill.
    return {
        "observed": observed,
        "escape": captured.get("escape", {}),
        "hooks": hooks if 'hooks' in locals() else [],
        "launchContext": captured.get("launchContext", {}),
        "messages": captured.get("messages", []),
    }


# The natives that search for a chunk take the tag as their second argument.
# The payload getter and the file loader take one argument, so the observable is
# whether the call found its chunk and how many calls there were.
SEARCHES_THE_TAG = {"Iff_FindFormChunk", "Iff_FindChunk", "Iff_FindChunkBelowForm"}


def walk_for(records: list, name: str) -> list:
    """The ordered chunk walk one native performed."""
    if name in SEARCHES_THE_TAG:
        return [(record["tag"], record["found"]) for record in records if record["target"] == name]
    return [record["found"] for record in records if record["target"] == name]


def ordered_subsequence_positions(reference: list, candidate: list) -> list[int] | None:
    """Return retail indexes for every candidate call, preserving call order.

    Retail parses animation chunks in addition to the skeleton queries made by
    the current OpenIGI scene. Its trace therefore contains interleaved calls
    that have no candidate counterpart. The candidate's observed calls must
    still match a monotonically ordered subsequence of retail calls.
    """
    positions = []
    cursor = 0
    for call in candidate:
        while cursor < len(reference) and reference[cursor] != call:
            cursor += 1
        if cursor == len(reference):
            return None
        positions.append(cursor)
        cursor += 1
    return positions


def ensure_frida_server() -> None:
    """Use an existing Frida server or start one without killing other processes."""
    import frida

    try:
        frida.get_device_manager().add_remote_device(FRIDA_HOST).enumerate_processes()
        return
    except Exception:  # noqa: BLE001 - the server is restarted below
        pass
    time.sleep(2)
    server = pathlib.Path.home() / ".cache/frida-crossover/frida-server-x86.exe"
    if not server.is_file():
        raise RuntimeError(f"Frida server is unreachable and the local server binary is missing: {server}")
    process = subprocess.Popen(
        [str(WINE), "--bottle", "open-igi", f"Z:\\Users\\{pathlib.Path.home().name}\\.cache\\frida-crossover\\frida-server-x86.exe",
         "-l", FRIDA_HOST],
        cwd=str(GAME),
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        env=crossover_environment(),
    )
    for _ in range(20):
        time.sleep(2)
        try:
            frida.get_device_manager().add_remote_device(FRIDA_HOST).enumerate_processes()
            return
        except Exception:  # noqa: BLE001 - the loop retries until the server is up
            continue
    if process.poll() is None:
        process.terminate()
        try:
            process.wait(timeout=10)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait(timeout=10)
    raise RuntimeError(f"Frida server did not become reachable at {FRIDA_HOST}")


def ensure_frida() -> None:
    """The frida module lives in the pipx environment, not in the system one."""
    try:
        import frida  # noqa: F401

        return
    except ModuleNotFoundError:
        pass
    candidates = [
        pathlib.Path.home() / "Library/Application Support/pipx/venvs/frida-tools/bin/python",
        pathlib.Path.home() / ".local/bin/python",
    ]
    for candidate in candidates:
        if not candidate.is_file():
            continue
        os.execv(str(candidate),
                 [str(candidate), str(pathlib.Path(__file__).resolve()), *sys.argv[1:]])
    raise SystemExit("the frida python environment is required; set FRIDA_PYTHON to it")


def run_side_with_retries(mode: str, executable: str, level: int, symbols: dict,
                          candidate_exe: pathlib.Path, attempts: int = 3) -> dict:
    """Retry failed owned launches; never attach to or kill a pre-existing game."""
    last_error = ""
    for attempt in range(attempts):
        try:
            observed = run_side(mode, executable, level, symbols, candidate_exe)
        except GameAlreadyRunningError:
            raise
        except Exception as error:  # noqa: BLE001 - the reason is reported below
            last_error = str(error)
            print(f"the {mode} side failed ({last_error}); retrying", file=sys.stderr)
            time.sleep(20)
            continue
        if observed:
            return observed
        last_error = "no observation"
        print(f"the {mode} side observed nothing; retrying", file=sys.stderr)
        time.sleep(20)
    raise SystemExit(f"the {mode} side did not produce an observation: {last_error}")


def default_candidate_executable() -> pathlib.Path:
    """Resolve the current candidate, retaining legacy layouts as fallback."""
    configured = os.environ.get("IGI_CANDIDATE_EXE")
    if configured:
        return pathlib.Path(configured)
    candidates = [
        ROOT / "build-clean-win32" / "igi1.exe",
        ROOT / "build-win32-no-verify" / "engine" / "platform" / "windows" / "open-igi.exe",
    ]
    return next((path for path in candidates if path.is_file()), candidates[0])


def main() -> int:
    ensure_frida()
    ensure_frida_server()

    parser = argparse.ArgumentParser()
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", required=True, type=pathlib.Path)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", required=True, type=pathlib.Path)
    parser.add_argument(
        "--candidate-exe",
        type=pathlib.Path,
        default=default_candidate_executable(),
    )
    parser.add_argument("--candidate-exe-sha256", default=os.environ.get("IGI_CANDIDATE_SHA256"))
    parser.add_argument("--level", type=int, default=4)
    arguments = parser.parse_args()

    address = "0x" + arguments.address[-8:].upper()
    if address not in FAMILY:
        raise SystemExit(f"unsupported native address: {arguments.address}")
    if digest(arguments.binary) != arguments.binary_sha256:
        raise SystemExit("retail binary SHA-256 changed")
    if digest(arguments.implementation) != arguments.implementation_sha256:
        raise SystemExit("implementation SHA-256 changed")
    if not arguments.candidate_exe.is_file():
        raise SystemExit(f"candidate executable does not exist: {arguments.candidate_exe}")
    candidate_digest = digest(arguments.candidate_exe)
    if arguments.candidate_exe_sha256 and candidate_digest != arguments.candidate_exe_sha256:
        raise SystemExit("candidate executable SHA-256 changed")
    from build_attestation import require_x86_pe
    require_x86_pe(arguments.candidate_exe)

    symbols = candidate_symbols(arguments.candidate_exe)
    name = FAMILY[address]
    retail = run_side_with_retries("retail", "igi.exe", arguments.level, symbols, arguments.candidate_exe)
    print(f"retail observations: {len(retail['observed'])}; Escape={retail['escape']}", file=sys.stderr)
    candidate = run_side_with_retries(
        "candidate", arguments.candidate_exe.name, arguments.level, symbols, arguments.candidate_exe,
    )
    print(f"candidate observations: {len(candidate['observed'])}", file=sys.stderr)

    retail_walk = walk_for(retail["observed"], name)
    candidate_walk = walk_for(candidate["observed"], name)
    if not candidate_walk:
        raise SystemExit(f"the candidate never called {name}")
    if not retail_walk:
        raise SystemExit(f"the retail never called {name}")

    # Retail makes additional animation-clip queries between skeleton queries.
    # Require every candidate query/result to occur in order in the retail trace,
    # rather than incorrectly requiring the whole-game traces to share a prefix.
    matched_positions = ordered_subsequence_positions(retail_walk, candidate_walk)
    ordered_calls_match = matched_positions is not None
    matched_retail_walk = (
        [retail_walk[index] for index in matched_positions]
        if matched_positions is not None else []
    )

    cases = [
        {
            "id": f"{name}-chunk-walk",
            "categories": ["structure"],
            "input": {"level": arguments.level, "launch": "window", "same_game_data_root": True},
            "reference_output": {
                "return_value": len(matched_retail_walk),
                "side_effects": [{"chunk_walk": matched_retail_walk,
                                  "retail_call_indexes": matched_positions or []}],
                "state_delta": {"candidate_calls_matched_in_order": len(matched_retail_walk)},
            },
            "candidate_output": {
                "return_value": len(candidate_walk),
                "side_effects": [{"chunk_walk": candidate_walk,
                                  "retail_call_indexes": matched_positions or []}],
                "state_delta": {"candidate_calls_matched_in_order": len(candidate_walk)},
            },
        }
    ]

    failures = 0 if ordered_calls_match else 1
    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "result": "passed" if failures == 0 else "failed",
        "executed": True,
        "oracle": "retail-binary-execution",
        "address": address,
        "binary_sha256": arguments.binary_sha256,
        "implementation_sha256": arguments.implementation_sha256,
        "candidate_exe_sha256": candidate_digest,
        "method": (
            "The retail igi.exe and the candidate open-igi.exe ran the same scene with the "
            "same level, with the retail virtual addresses and candidate symbols hooked. "
            "Each candidate query tag and non-null result must match a monotonically ordered "
            "subsequence of the retail trace; extra interleaved retail animation queries are "
            "retained in the raw count and do not shift the skeleton-query comparison."
        ),
        "observable_fields": ["ordered_calls", "query_tag", "return_nonnull"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(cases),
        "tests_passed": len(cases) - failures,
        "tests_failed": failures,
        "cases": cases,
        "retail_execution": {
            "engine": "frida",
            "tool": "frida",
            "version": "paired-probe",
            "analysis": "retail igi.exe under the paired probe",
            "function_entry": address,
            "candidate_target": "open-igi.exe",
            "candidate_sha256": arguments.implementation_sha256,
            "binary_path": str(arguments.binary),
            "binary_sha256": arguments.binary_sha256,
            "helper_path": "scripts/frida_mef_bones_probe.js",
            "helper_sha256": digest(PROBE),
            "escape": retail["escape"],
        },
        "candidate_execution": {
            "executable_path": str(arguments.candidate_exe.resolve()),
            "executable_sha256": candidate_digest,
            "level": arguments.level,
        },
        "limitations": [
            "The retail also parses the animation clips inside the same file, so it calls the "
            "chunk navigation more often; unmatched retail calls remain visible as extra events.",
            "The probe compares the query tag and null/non-null result in order. It does not "
            "snapshot the chunk tree or compare returned-node contents, so this evidence does "
            "not establish complete IFF parser equivalence.",
        ],
    }
    print(json.dumps(report))
    return 0 if failures == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
