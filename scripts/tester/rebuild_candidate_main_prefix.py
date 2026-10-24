#!/usr/bin/env python3
"""Rebuild the candidate main-prefix probe/profile for the currently deployed igi1.exe.

The retail contract is frozen; only candidate addresses change between builds. This
reads the previous candidate profile/probe, resolves the new build's symbols with
nm, locates WinMain and its QTask_CreateTask call with objdump, resolves the
proven string literals to VAs through the PE section table, and writes a new probe
plus profile under the target run directory. It never runs the candidate.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path

NM = ["i686-w64-mingw32-nm", "-C"]
OBJDUMP = ["i686-w64-mingw32-objdump"]


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def symbols(path: Path) -> dict[str, list[tuple[int, str]]]:
    out = subprocess.run(NM + [str(path)], capture_output=True, text=True, check=True).stdout
    table: dict[str, list[tuple[int, str]]] = {}
    for line in out.splitlines():
        parts = line.split(None, 2)
        if len(parts) == 3 and re.fullmatch(r"[0-9a-fA-F]{8}", parts[0]) and parts[1] in "TtWwVvBbDdRrIi":
            table.setdefault(parts[2].strip(), []).append((int(parts[0], 16), parts[1]))
    return table


def one(table, name: str) -> int:
    hits = table.get(name) or []
    if not hits:
        raise SystemExit(f"missing symbol {name}")
    return hits[0][0]


def section_map(path: Path) -> list[tuple[int, int, int, int]]:
    """PE section table: (virtual address, virtual size, raw offset, raw size)."""
    import struct
    blob = path.read_bytes()
    pe = struct.unpack_from("<I", blob, 0x3C)[0]
    if blob[pe:pe + 4] != b"PE\x00\x00":
        raise SystemExit("not a PE")
    image_base = struct.unpack_from("<I", blob, pe + 24 + 28)[0]
    nsections = struct.unpack_from("<H", blob, pe + 6)[0]
    optional_size = struct.unpack_from("<H", blob, pe + 20)[0]
    table = pe + 24 + optional_size
    sections = []
    for i in range(nsections):
        off = table + i * 40
        virtual_size, virtual_address, raw_size, raw_pointer = struct.unpack_from("<IIII", blob, off + 8)
        sections.append((image_base + virtual_address, virtual_size, raw_pointer, raw_size))
    return sections


_CACHE: dict[str, tuple[bytes, list[tuple[int, int, int, int]]]] = {}


def cached(path: Path):
    key = str(path)
    if key not in _CACHE:
        _CACHE[key] = (path.read_bytes(), section_map(path))
    return _CACHE[key]


def file_to_va(path: Path, offset: int) -> int | None:
    _, sections = cached(path)
    for vma, size, raw, _ in sections:
        if raw <= offset < raw + size:
            return vma + (offset - raw)
    return None


def find_strings(path: Path, text: str) -> list[int]:
    if not text:
        return 0
    blob, _ = cached(path)
    needle = text.encode() + b"\x00"
    start = 0
    hits = []
    while True:
        i = blob.find(needle, start)
        if i < 0:
            break
        va = file_to_va(path, i)
        if va is not None and 0x400000 <= va <= 0x5FFFFF:
            hits.append(va)
        start = i + 1
    return hits


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--candidate", type=Path, required=True)
    ap.add_argument("--old-profile", type=Path, required=True)
    ap.add_argument("--old-probe", type=Path, required=True)
    ap.add_argument("--out-dir", type=Path, required=True)
    ap.add_argument("--candidate-source", default="unknown")
    ap.add_argument("--candidate-commit", default="unknown")
    args = ap.parse_args()
    path = args.candidate.resolve()
    out = args.out_dir
    (out / "probes").mkdir(parents=True, exist_ok=True)
    (out / "contracts").mkdir(parents=True, exist_ok=True)
    old_profile = json.loads(args.old_profile.read_text())
    probe_text = args.old_probe.read_text()
    m = re.search(r"const CONFIG = (\{.*?\n\});\n", probe_text, re.S)
    config = json.loads(m.group(1))

    sym = symbols(path)
    entry = one(sym, "WinMain@16")
    qtask = one(sym, "QTask_CreateTask")
    winproc = one(sym, "WindowProcedure")
    fp_init = one(sym, "igi::retail::core::FP_InitExtendedPrecision()")
    dis = subprocess.run(OBJDUMP + ["-d", "-M", "intel", f"--start-address=0x{entry:08x}",
                                    f"--stop-address=0x{entry + 0x800:08x}", str(path)],
                         capture_output=True, text=True, check=True).stdout
    callsites = [int(line.split(":")[0].strip(), 16) for line in dis.splitlines()
                 if re.search(r"\bcall\b.*QTask_CreateTask", line)]
    if len(callsites) != 1:
        raise SystemExit(f"expected one QTask_CreateTask call in WinMain, got {callsites}")
    hard_stop = callsites[0]

    native_map = {
        "StoreCommandLine": one(sym, "StoreCommandLine"),
        "SetAppPath(?)": one(sym, "StoreInstanceHandle"),
        "StartupSetter_48F0F0": one(sym, "SetConfigFileName"),
        "StartupSetter_48F100": one(sym, "SetAppTitle_48F100"),
        "StartupSetter_48F140": one(sym, "AppContext_SetIsActive"),
        "StartupSetter_48F120": one(sym, "Display_SetScreenMode_48F120"),
        "StartupSetter_48F1A0": one(sym, "igi::retail::core::App_ContextSetDebugged(signed char)"),
        "StartupSetter_48F1C0": one(sym, "SetDebugCompileState_48F1C0"),
        "StartupSetter_48F1E0": one(sym, "igi::retail::core::App_ContextSetDebugtextState(signed char)"),
        "StartupSetter_48F240": one(sym, "igi::retail::core::App_ContextSetLightmapsUsed(signed char)"),
        "StartupSetter_48F260": one(sym, "AppContext_SetTerrainLightmapsUsed"),
        "StartupSetter_48F280": one(sym, "Input_SetUseWin32Input_48F280"),
        "StartupSetter_48F290": one(sym, "AppMain_Set5C8C0C_48F290"),
        "StartupSetter_48F2A0": one(sym, "AppMain_Set5C8C10_48F2A0"),
        "StartupSetter_48F210": one(sym, "Game_SetStartupMode_48F210"),
        "StartupSetter_48F2B0": one(sym, "AppContext_SetGermanVersion_48F2B0"),
        "StartupSetter_48F300": one(sym, "AppContext_SetLanguage_48F300"),
        "StartupSetter_48F200": one(sym, "AppMain_Set5C8C04_48F200"),
        "AppMain_ParseCmdLineArgs": one(sym, "AppMain_ParseCmdLineArgs_48F360"),
        "Display_GetScreenMode(?)": one(sym, "Display_GetScreenMode_48F130"),
        "StoreWindowHandle": one(sym, "SetWindowHandle"),
        "FP_InitExtendedPrecision(0x498460 substitute)": fp_init,
        "QTask_CreateTask": qtask,
    }
    imports = {
        "CreateMutexA": one(sym, "_imp__CreateMutexA@12"),
        "GetLastError": one(sym, "_imp__GetLastError@0"),
        "CoInitialize": one(sym, "_imp__CoInitialize@4"),
        "GetForegroundWindow": one(sym, "_imp__GetForegroundWindow@0"),
        "LoadIconA": one(sym, "_imp__LoadIconA@8"),
        "LoadCursorA": one(sym, "_imp__LoadCursorA@8"),
        "RegisterClassA": one(sym, "_imp__RegisterClassA@4"),
        "MessageBoxA": one(sym, "_imp__MessageBoxA@16"),
        "GetSystemMetrics": one(sym, "_imp__GetSystemMetrics@4"),
        "CreateWindowExA": one(sym, "_imp__CreateWindowExA@48"),
        "ShowCursor": one(sym, "_imp__ShowCursor@4"),
        "ShowWindow": one(sym, "_imp__ShowWindow@8"),
    }

    new_config = json.loads(json.dumps(config))
    new_config["target"] = {"va": hex(entry), "name": "main"}
    new_config["start"] = hex(entry)
    new_config["hardStop"] = hex(hard_stop)
    texts = [s["text"] for s in config["provenStrings"] if s["text"]]
    mutex_hits = find_strings(path, "IGI1MUTEX")
    if not mutex_hits:
        raise SystemExit("missing IGI1MUTEX string")
    anchor = mutex_hits[0]
    new_config["provenStrings"] = []
    for text in texts:
        hits = find_strings(path, text)
        if not hits:
            raise SystemExit(f"missing string {text!r}")
        new_config["provenStrings"].append(
            {"va": hex(min(hits, key=lambda va: abs(va - anchor))), "text": text})
    for spec in new_config["calls"]:
        if spec["kind"] == "import":
            spec["iat"] = hex(imports[spec["name"]])
        else:
            spec["va"] = hex(native_map[spec["name"]])
    chain = one(sym, "WindowHandlerChain")
    term = one(sym, "WindowHandlerTerminator")
    tail = one(sym, "WindowHandlerTail")
    window_handle = one(sym, "g_window_handle")
    loop_status = one(sym, "Loop_StatusByte")
    debug_flag = one(sym, "DebugKeyFlag")
    new_config["globals"] = [
        {"label": "window_handle_g_5c8bc4", "va": hex(window_handle), "length": 4},
        {"label": "window_handler_chain_5c8df0", "va": hex(chain), "length": 4},
        {"label": "window_handler_terminator_5c8df4", "va": hex(term), "length": 12},
        {"label": "window_handler_tail_5c8df8", "va": hex(tail), "length": 4},
        {"label": "loop_status_byte_5c8de8", "va": hex(loop_status), "length": 1},
        {"label": "debug_key_flag_5c8e04", "va": hex(debug_flag), "length": 4},
    ]
    probe = probe_text[:m.start(1)] + json.dumps(new_config, indent=2) + probe_text[m.end(1):]
    probe_path = out / "probes/probe_main_prefix_candidate.js"
    probe_path.write_text(probe)

    profile = json.loads(json.dumps(old_profile))
    profile["candidate_sha256"] = sha256(path)
    profile["candidate_source_commit"] = args.candidate_source
    profile["candidate_commit"] = args.candidate_commit
    profile["probe"] = {"path": str(probe_path), "sha256": sha256(probe_path)}
    profile["main"] = {"retail_va": "0x0048F530", "candidate_va": hex(entry),
                       "hard_stop_candidate": hex(hard_stop), "hard_stop_retail": "0x0048F7AA"}
    profile["candidate_facts"] = {
        "window_procedure": hex(winproc),
        "handler_pointers": {"Window": hex(one(sym, "AppMain_WindowCommand_48F350")),
                             "WinInput": hex(one(sym, "AppMain_WinInputCommand_48F920")),
                             "Germany": hex(one(sym, "AppMain_GermanyCommand_48F930")),
                             "USA": hex(one(sym, "AppMain_USACommand_48F950"))},
        "globals": {"window_handle": hex(window_handle), "window_handler_chain": hex(chain),
                    "window_handler_terminator": hex(term), "window_handler_tail": hex(tail),
                    "loop_status_byte": hex(loop_status), "debug_key_flag": hex(debug_flag)},
        "strings": {s["text"]: s["va"] for s in new_config["provenStrings"]},
        "pool": hex(one(sym, "WindowHandlerPool")) if "WindowHandlerPool" in sym else None,
    }
    (out / "contracts/candidate-profile.json").write_text(json.dumps(profile, indent=2) + "\n")
    (out / "rebuild-facts.json").write_text(json.dumps(dict(
        candidate=str(path), sha256=profile["candidate_sha256"], entry=hex(entry),
        qtask=hex(qtask), hard_stop=hex(hard_stop), imports=imports,
        natives=native_map, strings=profile["candidate_facts"]["strings"],
        globals=profile["candidate_facts"]["globals"]), indent=2) + "\n")
    print(f"candidate {profile['candidate_sha256']} entry={hex(entry)} qtask={hex(qtask)} hard_stop={hex(hard_stop)}")
    print(f"probe {probe_path} sha256={profile['probe']['sha256']}")
    print(json.dumps(profile["candidate_facts"], indent=1)[:1200])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
