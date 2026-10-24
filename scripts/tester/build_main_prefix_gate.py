#!/usr/bin/env python3
"""Build the main-prefix probe + runtime contract (analysis only, no launch).

Scope: retail main 0x0048F530 from entry up to the 0x0048F7AA QTask_CreateTask
call. The probe is generated from scripts/tester/frida_main_prefix_probe.js and
records only calls whose return address is inside that prefix. Everything after
0x0048F7AA (pump exit path, exit_code/[0x5C8DE8]/WM_QUIT, shutdown) is declared
UNOBSERVED-until-reachable in the contract: it is not captured and never passed
while QTask_RegisterClasses still throws.

Inputs are the static capture for 0x0048F530 (BN/r2/angr) plus the BN
disassembly. No game is launched; no manifest/status is edited.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TEMPLATE = ROOT / "scripts/tester/frida_main_prefix_probe.js"
RETAIL_SHA = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
CANDIDATE_SHA = "6d39aa53216f2d0db4edb50afac1e217fa7dd8b5881f3f073404ba4621973e2e"
CANDIDATE_SOURCE = "42490c2e"
CANDIDATE_COMMIT = "6f019e12"
MAIN = "0x0048F530"
START = "0x0048F530"
HARD_STOP = "0x0048F7AA"  # call QTask_CreateTask

# Packet/static-proven literal strings. Probes only ever read a C string when an
# observed word equals one of these addresses.
PROVEN_STRINGS = [
    ("0x00541ECC", "IGIMUTEX"),
    ("0x00541E60", "Error"),
    ("0x00541DBC", "IGI is already running."),
    ("0x00541E48", "Couldn't open window"),
    ("0x00541E68", "This application requires DirectX version %d.%d or greater to run."),
    ("0x0053B534", "Innerloop"),
    ("0x00541EC0", "filesys.cfg"),
    ("0x00541EB8", "Window"),
    ("0x00541EAC", "WinInput"),
    ("0x00541D60", "Germany"),
    ("0x005382C8", "USA"),
    ("0x00541E38", "HardcoreGerman"),
    ("0x00541DD4", "Invalid FPU rounding mode. May be caused by running a debug session "
                   "with ftol optimize turned on."),
    ("0x00567C74", ""),  # g_script_ret_type: SetLanguage("") empty string
]

GLOBALS = [
    dict(label="clear_range_5c8d28", va="0x005C8D28", length=0xC0),
    dict(label="ptr_chain_5c8df0", va="0x005C8DF0", length=12),
    dict(label="window_handle_5c8bc4", va="0x005C8BC4", length=4),
    dict(label="task_id_byte_5c8de8", va="0x005C8DE8", length=1),
]

# One hook per unique target. Imports are hooked through their IAT slot; native
# calls are hooked at their retail VA. Arg word counts come from the disassembly.
CALLS = [
    dict(name="CreateMutexA", kind="import", iat="0x00533124", argWords=3),
    dict(name="GetLastError", kind="import", iat="0x00533128", argWords=0),
    dict(name="CoInitialize", kind="import", iat="0x0053321C", argWords=1),
    dict(name="GetForegroundWindow", kind="import", iat="0x005331D8", argWords=0),
    dict(name="LoadIconA", kind="import", iat="0x005331A4", argWords=2),
    dict(name="LoadCursorA", kind="import", iat="0x005331A8", argWords=2),
    dict(name="RegisterClassA", kind="import", iat="0x005331AC", argWords=1,
         struct=dict(slot=0, offset=0, length=0x28, label="wndclass")),
    dict(name="MessageBoxA", kind="import", iat="0x005331B0", argWords=4),
    dict(name="GetSystemMetrics", kind="import", iat="0x005331E8", argWords=1),
    dict(name="CreateWindowExA", kind="import", iat="0x005331B4", argWords=12),
    dict(name="ShowCursor", kind="import", iat="0x005331B8", argWords=1),
    dict(name="ShowWindow", kind="import", iat="0x005331BC", argWords=2),
    dict(name="StoreCommandLine", kind="native", va="0x0048F0C0", argWords=1),
    dict(name="SetAppPath(?)", kind="native", va="0x0048F0D0", argWords=1),
    dict(name="StartupSetter_48F0F0", kind="native", va="0x0048F0F0", argWords=1),
    dict(name="StartupSetter_48F100", kind="native", va="0x0048F100", argWords=1),
    dict(name="StartupSetter_48F140", kind="native", va="0x0048F140", argWords=1),
    dict(name="StartupSetter_48F120", kind="native", va="0x0048F120", argWords=1),
    dict(name="StartupSetter_48F1A0", kind="native", va="0x0048F1A0", argWords=1),
    dict(name="StartupSetter_48F1C0", kind="native", va="0x0048F1C0", argWords=1),
    dict(name="StartupSetter_48F1E0", kind="native", va="0x0048F1E0", argWords=1),
    dict(name="StartupSetter_48F240", kind="native", va="0x0048F240", argWords=1),
    dict(name="StartupSetter_48F260", kind="native", va="0x0048F260", argWords=1),
    dict(name="StartupSetter_48F280", kind="native", va="0x0048F280", argWords=1),
    dict(name="StartupSetter_48F290", kind="native", va="0x0048F290", argWords=1),
    dict(name="StartupSetter_48F2A0", kind="native", va="0x0048F2A0", argWords=1),
    dict(name="StartupSetter_48F210", kind="native", va="0x0048F210", argWords=1),
    dict(name="StartupSetter_48F2B0", kind="native", va="0x0048F2B0", argWords=1),
    dict(name="StartupSetter_48F300", kind="native", va="0x0048F300", argWords=1),
    dict(name="StartupSetter_48F200", kind="native", va="0x0048F200", argWords=1),
    dict(name="AppMain_ParseCmdLineArgs", kind="native", va="0x0048F360", argWords=2),
    dict(name="Display_DinputDll", kind="native", va="0x00498480", argWords=2),
    dict(name="sprintf", kind="native", va="0x004A53B3", argWords=3),
    dict(name="Camera_ApplyBandFilter_6", kind="native", va="0x004A5AD5", argWords=1),
    dict(name="Display_GetScreenMode(?)", kind="native", va="0x0048F130", argWords=0),
    dict(name="StoreWindowHandle", kind="native", va="0x0048F0E0", argWords=1),
    dict(name="Display_ClearSurfaces_16", kind="native", va="0x00498460", argWords=0),
    dict(name="QTask_CreateTask", kind="native", va="0x00401000", argWords=0,
         snapshotTag="qtask"),
]


def arg(slot: int, kind: str, value=None) -> dict:
    row = {"slot": slot, "kind": kind}
    if value is not None:
        row["value"] = value
    return row


def string_arg(slot: int, va: str, text: str) -> dict:
    return {"slot": slot, "kind": "string", "va": va, "text": text}


def rel_arg(slot: int, relation: str) -> dict:
    return {"slot": slot, "kind": "relation", "relation": relation}


def row(order, callsite, name, target_kind, target, args, returns=None, note=None) -> dict:
    return {"order": order, "callsite": callsite, "name": name, "target_kind": target_kind,
            "target": target, "expected_args": args, "returns": returns, "static_note": note}


# Ordered static call stream inside 0x48F530..0x48F7AA (from BN disassembly).
SEQUENCE = [
    row(1, "0x0048F544", "CreateMutexA", "import", "0x00533124",
        [arg(0, "int", 0), arg(1, "int", 1), string_arg(2, "0x00541ECC", "IGIMUTEX")],
        "nonzero handle; NULL -> duplicate/failure MessageBoxA path"),
    row(2, "0x0048F560", "GetLastError", "import", "0x00533128", [],
        "first call; compare eax 0xB7 -> fatal MessageBoxA path 0x48F8F0"),
    row(3, "0x0048F56D", "GetLastError", "import", "0x00533128", [],
        "second call; compare eax 0x6 -> fatal MessageBoxA path 0x48F8F0"),
    row(4, "0x0048F579", "CoInitialize", "import", "0x0053321C", [arg(0, "int", 0)]),
    row(5, "0x0048F57F", "GetForegroundWindow", "import", "0x005331D8", [],
        "return stored as previous foreground window"),
    row(6, "0x0048F5B2", "LoadIconA", "import", "0x005331A4",
        [rel_arg(0, "entry_arg1"), arg(1, "int", 0x7F00)], "return -> WNDCLASSA.hIcon"),
    row(7, "0x0048F5C2", "LoadCursorA", "import", "0x005331A8",
        [arg(0, "int", 0), arg(1, "int", 0x7F00)], "return -> WNDCLASSA.hCursor"),
    row(8, "0x0048F5E6", "RegisterClassA", "import", "0x005331AC",
        [arg(0, "stack-struct", "wndclass")], "return BOOL recorded; retail ignores failure"),
    row(9, "0x0048F5F4", "StoreCommandLine", "native", "0x0048F0C0",
        [rel_arg(0, "entry_arg2")], "arg2 @ entry stack word 2 (0xc)"),
    row(10, "0x0048F5FA", "SetAppPath(?)", "native", "0x0048F0D0",
         [rel_arg(0, "entry_arg1")], "BN placeholder name; address is the evidence"),
    row(11, "0x0048F604", "StartupSetter_48F0F0", "native", "0x0048F0F0",
         [string_arg(0, "0x00541EC0", "filesys.cfg")]),
    row(12, "0x0048F60E", "StartupSetter_48F100", "native", "0x0048F100",
         [string_arg(0, "0x0053B534", "Innerloop")]),
    row(13, "0x0048F615", "StartupSetter_48F140", "native", "0x0048F140", [arg(0, "int", 1)]),
    row(14, "0x0048F61C", "StartupSetter_48F120", "native", "0x0048F120", [arg(0, "int", 1)]),
    row(15, "0x0048F622", "StartupSetter_48F1A0", "native", "0x0048F1A0", [arg(0, "int", 0)]),
    row(16, "0x0048F629", "StartupSetter_48F1C0", "native", "0x0048F1C0", [arg(0, "int", 1)]),
    row(17, "0x0048F62F", "StartupSetter_48F1E0", "native", "0x0048F1E0", [arg(0, "int", 0)]),
    row(18, "0x0048F636", "StartupSetter_48F240", "native", "0x0048F240", [arg(0, "int", 1)]),
    row(19, "0x0048F63D", "StartupSetter_48F260", "native", "0x0048F260", [arg(0, "int", 1)]),
    row(20, "0x0048F643", "StartupSetter_48F280", "native", "0x0048F280", [arg(0, "int", 0)]),
    row(21, "0x0048F649", "StartupSetter_48F290", "native", "0x0048F290", [arg(0, "int", 0)]),
    row(22, "0x0048F64F", "StartupSetter_48F2A0", "native", "0x0048F2A0", [arg(0, "int", 0)]),
    row(23, "0x0048F656", "StartupSetter_48F210", "native", "0x0048F210", [arg(0, "int", 1)]),
    row(24, "0x0048F65C", "StartupSetter_48F2B0", "native", "0x0048F2B0", [arg(0, "int", 0)]),
    row(25, "0x0048F669", "StartupSetter_48F300", "native", "0x0048F300",
         [string_arg(0, "0x00567C74", "")], "SetLanguage(\"\") empty string"),
    row(26, "0x0048F66F", "StartupSetter_48F200", "native", "0x0048F200", [arg(0, "int", 0)]),
    row(27, "0x0048F6A6", "AppMain_ParseCmdLineArgs", "native", "0x0048F360",
         [string_arg(0, "0x00541EB8", "Window"), arg(1, "code-pointer", "0x0048F350")],
         "handler CombatState_Invoke23"),
    row(28, "0x0048F6B5", "AppMain_ParseCmdLineArgs", "native", "0x0048F360",
         [string_arg(0, "0x00541EAC", "WinInput"), arg(1, "code-pointer", "0x0048F920")],
         "handler CombatState_ProcessGermany"),
    row(29, "0x0048F6C4", "AppMain_ParseCmdLineArgs", "native", "0x0048F360",
         [string_arg(0, "0x00541D60", "Germany"), arg(1, "code-pointer", "0x0048F930")],
         "handler CombatState_ProcessGermany_48F930"),
    row(30, "0x0048F6D3", "AppMain_ParseCmdLineArgs", "native", "0x0048F360",
         [string_arg(0, "0x005382C8", "USA"), arg(1, "code-pointer", "0x0048F950")],
         "handler Input_CheckDIErrCode"),
    row(31, "0x0048F6E2", "Display_DinputDll", "native", "0x00498480",
         [arg(0, "stack-out", "var_464"), arg(1, "stack-out", "hWnd")],
         "arg0 receives display mode compared against 0x700; pointer values not comparable"),
    row(32, "0x0048F739", "GetSystemMetrics", "import", "0x005331E8", [arg(0, "int", 1)],
         "SM_CYSCREEN first; result -> (eax-0x1e0) floor /2"),
    row(33, "0x0048F747", "GetSystemMetrics", "import", "0x005331E8", [arg(0, "int", 0)],
         "SM_CXSCREEN second; result -> (eax-0x280) floor /2"),
    row(34, "0x0048F754", "Display_GetScreenMode(?)", "native", "0x0048F130", [],
         "return feeds CreateWindowExA style: 0 -> 0x37C80000 else 0x96000000"),
    row(35, "0x0048F770", "CreateWindowExA", "import", "0x005331B4",
         [arg(0, "int", 0), string_arg(1, "0x0053B534", "Innerloop"),
          string_arg(2, "0x0053B534", "Innerloop"), arg(3, "derived-style", "0x48F130"),
          arg(4, "derived-x", "SM_CXSCREEN"), arg(5, "derived-y", "SM_CYSCREEN"),
          arg(6, "int", 0x280), arg(7, "int", 0x1E0), arg(8, "int", 0),
          arg(9, "int", 0), rel_arg(10, "entry_arg1"), arg(11, "int", 0)],
         "return hwnd; NULL -> 'Couldn't open window' MessageBoxA path"),
    row(36, "0x0048F790", "ShowCursor", "import", "0x005331B8", [arg(0, "int", 0)]),
    row(37, "0x0048F799", "ShowWindow", "import", "0x005331BC",
         [rel_arg(0, "createwindow_return"), arg(1, "int", 10)]),
    row(38, "0x0048F7A0", "StoreWindowHandle", "native", "0x0048F0E0",
         [rel_arg(0, "createwindow_return")], "writes [0x005C8BC4] = hwnd"),
    row(39, "0x0048F7A5", "Display_ClearSurfaces_16", "native", "0x00498460", []),
    row(40, "0x0048F7AA", "QTask_CreateTask", "native", "0x00401000", [],
         "hard stop: returns task pointer/EAX (retail); candidate still throws inside "
         "QTask_RegisterClasses, so entry-only on the candidate side is expected boundary evidence"),
]

WNDCLASS_EXPECTED = dict(
    struct_offset=0, length=0x28,
    fields=[
        dict(offset=0x0, name="style", value=3),
        dict(offset=0x4, name="lpfnWndProc", value="0x0048F970"),
        dict(offset=0x8, name="cbClsExtra", value=0),
        dict(offset=0xC, name="cbWndExtra", value=0),
        dict(offset=0x10, name="hInstance", relation="entry_arg1"),
        dict(offset=0x14, name="hIcon", relation="LoadIconA_return"),
        dict(offset=0x18, name="hCursor", relation="LoadCursorA_return"),
        dict(offset=0x1C, name="hbrBackground", value=7),
        dict(offset=0x20, name="lpszMenuName", value="0x0053B534"),
        dict(offset=0x24, name="lpszClassName", value="0x0053B534"),
    ],
    strings=[dict(field="lpszMenuName", text="Innerloop"), dict(field="lpszClassName", text="Innerloop")],
    relation="lpszMenuName == lpszClassName; both point at the proven 'Innerloop' bytes",
)

CREATE_WINDOW_EXPECTED = dict(
    args=12,
    slots=[
        dict(slot=0, name="dwExStyle", value=0),
        dict(slot=1, name="lpClassName", text="Innerloop"),
        dict(slot=2, name="lpWindowName", text="Innerloop"),
        dict(slot=3, name="dwStyle", derived="Display_GetScreenMode_return == 0 ? 0x37C80000 : 0x96000000"),
        dict(slot=4, name="X", derived="trunc((SM_CXSCREEN - 0x280) / 2)"),
        dict(slot=5, name="Y", derived="trunc((SM_CYSCREEN - 0x1E0) / 2)"),
        dict(slot=6, name="nWidth", value=0x280),
        dict(slot=7, name="nHeight", value=0x1E0),
        dict(slot=8, name="hWndParent", value=0),
        dict(slot=9, name="hMenu", value=0),
        dict(slot=10, name="hInstance", relation="entry_arg1"),
        dict(slot=11, name="lpParam", value=0),
    ],
)

ABSENCES = [
    dict(what="MessageBoxA", detail="normal startup path; only reached on mutex failure/0xB7/0x6 "
         "or DirectX<0x700 or CreateWindowExA==NULL"),
    dict(what="sprintf", detail="only in the DirectX<0x700 branch at 0x48F6F6"),
    dict(what="Camera_ApplyBandFilter_6", detail="only after the DirectX failure MessageBoxA at 0x48F71F"),
]

POST_BOUNDARY = [
    "0x0048F7AF AppMain_ParseCmdLineArgs(\"HardcoreGerman\", 0x48F9...)" ,
    "task byte [0x005C8DE8] store (0x48F7B9) and the non-quit re-test",
    "loop body 0x48F7E5: FPU rounding-mode check (0xBFE00000/0xFFFFFFFF), "
    "Display_ClearSurfaces_16 0x48F80D, FX_DebrisSpawn_23 test, QTask_OpenEvent 0x48F81B, "
    "Sleep(0x1F4) 0x48F827, DestroyWindow 0x48F83B",
    "message pump 0x48F841: PeekMessageA, GetMessageA; GetMessageA==0 -> exit_code=msg.wParam, "
    "[0x005C8DE8]=0 (Lead 6a64cf pump fix), WM_QUIT leaves the loop unconditionally; "
    "post-pump status re-test governs only the non-quit path",
    "shutdown 0x48F8AC: SetForegroundWindow, UnregisterClassA(g_541d68, arg1), "
    "CoUninitialize, ReleaseMutex, return result",
    "failure paths 0x48F781/0x48F8FC: 'Couldn't open window'/'IGI is already running.' MessageBoxA",
]


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--static", type=Path, required=True,
                    help="main-prefix/static/0x0048f530-static.json")
    ap.add_argument("--disasm", type=Path, required=True, help="BN disassembly text for 0x0048F530")
    ap.add_argument("--run-dir", type=Path, required=True, help="main-prefix run directory")
    ap.add_argument("--candidate-sha", default=CANDIDATE_SHA)
    args = ap.parse_args()

    run_dir = args.run_dir.resolve()
    probes_dir = run_dir / "probes"
    contracts_dir = run_dir / "contracts"
    reports_dir = run_dir / "reports"
    static_dir = run_dir / "static"
    for path in (probes_dir, contracts_dir, reports_dir, static_dir):
        path.mkdir(parents=True, exist_ok=True)

    static_path = args.static.resolve()
    static = json.loads(static_path.read_text())
    disasm_path = args.disasm.resolve()
    evidence_disasm = static_dir / "main-0x0048f530-disasm.txt"
    if disasm_path != evidence_disasm:
        shutil.copyfile(disasm_path, evidence_disasm)

    config = dict(
        target=dict(va=MAIN, name="main"),
        moduleName="igi.exe",
        preferredBase="0x00400000",
        start=START,
        hardStop=HARD_STOP,
        argWords=4,
        maxEvents=8192,
        provenStrings=[{"va": va, "text": text} for va, text in PROVEN_STRINGS],
        globals=GLOBALS,
        calls=CALLS,
    )
    probe_path = probes_dir / "probe_main_prefix.js"
    template = TEMPLATE.read_text()
    probe_path.write_text(template.replace(
        "const CONFIG = __MAIN_PREFIX_CONFIG_JSON__;",
        "const CONFIG = " + json.dumps(config, indent=2) + ";"))
    probe_sha = sha256(probe_path)

    contract = dict(
        kind="igi-tester-main-prefix-contract",
        schema_version=1,
        address=MAIN,
        scope=dict(start=START, end=HARD_STOP,
                   boundary="call QTask_CreateTask at 0x0048F7AA",
                   prefix_only=True,
                   post_boundary_status="UNOBSERVED-until-reachable",
                   post_boundary_reason="QTask_CreateTask still throws inside QTask_RegisterClasses in the "
                                        "candidate, so the pump exit path (exit_code/[0x005C8DE8]/WM_QUIT) and "
                                        "shutdown cannot execute. This contract never scores them."),
        identity=dict(retail_sha256=RETAIL_SHA,
                      candidate_sha256=args.candidate_sha,
                      candidate_source_commit=CANDIDATE_SOURCE,
                      candidate_commit=CANDIDATE_COMMIT,
                      image_base=0x400000,
                      arch="x86 windows"),
        probe=dict(path=str(probe_path.relative_to(run_dir)), sha256=probe_sha),
        static_evidence=dict(
            bn_function=str(static_path.name),
            disassembly=str(evidence_disasm.relative_to(run_dir)),
            disassembly_sha256=sha256(evidence_disasm),
            bn=dict(
                name=((static.get("binary_ninja") or {}).get("info") or {}).get("stdout", "").split('name: "')[-1].split('"')[0]
                if (static.get("binary_ninja") or {}).get("info") else None,
                prototype=static.get("binary_ninja", {}).get("prototype"),
                blocks=static.get("cfg_cross_check", {}).get("left_nodes"),
                edges=static.get("cfg_cross_check", {}).get("left_edges"),
            ),
            radare2=dict(nodes=len((static.get("radare2", {}).get("cfg") or {}).get("nodes") or []),
                         edges=len((static.get("radare2", {}).get("cfg") or {}).get("edges") or [])),
            angr=static.get("angr"),
            cfg_cross_check=static.get("cfg_cross_check"),
        ),
        static_expectations=dict(
            entry=dict(
                signature="WPARAM __stdcall(HINSTANCE arg1, int32_t arg2 @ 0xc); retn 0x10",
                observed_words=4,
                notes="word0=[esp+4]=hInstance arg1; word2=[esp+0xc]=arg2 passed to StoreCommandLine. "
                      "Stack arg words are recorded but cross-process pointer equality is never required; "
                      "hInstance is proven relationally via LoadIconA/RegisterClassA/CreateWindowExA args.",
            ),
            expected_sequence=SEQUENCE,
            wndclass=WNDCLASS_EXPECTED,
            create_window=CREATE_WINDOW_EXPECTED,
            static_absence=ABSENCES,
            globals=[
                dict(label="clear_range_5c8d28", va="0x005C8D28", length=0xC0,
                     expectation="all zero at QTask_CreateTask entry (rep stosd 0x30 dwords at 0x48F674)"),
                dict(label="ptr_chain_5c8df0", va="0x005C8DF0", length=12,
                     expectation="[0x5C8DF0]==0x005C8DF4; [0x5C8DF8]==0x005C8DF0; "
                                 "[0x5C8DF4]==0 at QTask_CreateTask entry"),
                dict(label="window_handle_5c8bc4", va="0x005C8BC4", length=4,
                     expectation="== CreateWindowExA return (StoreWindowHandle 0x48F0E0) at QTask entry"),
                dict(label="task_id_byte_5c8de8", va="0x005C8DE8", length=1,
                     expectation="pre-call value recorded; the post-return store at 0x48F7B9 is AFTER the "
                                 "boundary and therefore UNOBSERVED-until-reachable"),
            ],
            get_last_error=dict(
                static_branches="0x48F562 cmp eax,0xB7 je 0x48F8F0; 0x48F56F cmp eax,0x6 je 0x48F8F0",
                warning="retail disassembly sends BOTH 0xB7 (ERROR_ALREADY_EXISTS) and 0x6 "
                        "(ERROR_INVALID_HANDLE) to the fatal 'IGI is already running.'/'Error' "
                        "MessageBoxA; the Lead's description said 0x6 continues. Static evidence "
                        "takes precedence; normal idle startup skips both branches because CreateMutexA "
                        "succeeds, so the runtime capture records the observed GetLastError values and "
                        "marks both error branches unprovable in this scenario.",
            ),
            hard_stop=dict(callsite=HARD_STOP, target="QTask_CreateTask 0x00401000",
                           action="stop comparing at the call; record entry and, on retail, the return"),
        ),
        runtime_contract=dict(
            scenario="retail `window level1`, idle observation; candidate only with Lead authorization",
            required_observables=dict(
                entry="exactly one main entry with 4 argument words",
                ordered_calls="every prefix row in static order with matching callsite and typed arguments",
                wndclass="style/proc/cb*/hbr numeric equality; hInstance/hIcon/hCursor relational equality; "
                         "menu==class pointer and 'Innerloop' text",
                create_window="12 slots: numeric constants, 'Innerloop' class/title, derived X/Y/style, "
                              "hInstance relation; return hwnd != 0",
                metrics="GetSystemMetrics args 1 then 0 in that order, returns feed X/Y",
                side_effects="clear range all zero; pointer chain relations; [0x5C8BC4]==hwnd before QTask",
                boundary="QTask_CreateTask entry on both sides; retail return EAX recorded; candidate may be "
                         "entry-only because it throws inside, which is boundary evidence and not a pass",
                absences="MessageBoxA/sprintf/Camera_ApplyBandFilter_6 absent on the normal path",
            ),
            verdicts=["PREFIX_MATCH only when every in-scope retail observable matches the static contract "
                      "and the candidate matches retail on the same observables",
                      "UNOBSERVED when a side was not captured or the boundary was not reached",
                      "BLOCKED on identity/probe-sha/hook failure; MISMATCH otherwise",
                      "post-0x48F7AA is always UNOBSERVED-until-reachable and can never be scored as a pass"],
            fail_closed=["missing/reordered prefix call", "wrong literal string bytes", "different numeric "
                         "window/style/metrics values", "MessageBoxA on the normal path", "unreadable side "
                         "effect", "candidate entry without retail-comparable prefix", "stale sha"],
        ),
        reachability=dict(
            prefix="retail observable today",
            candidate_prefix="only after Lead authorizes a candidate run; QTask_CreateTask entry is the last "
                             "comparable point",
            post_boundary="UNOBSERVED-until-reachable (pump exit path incl. Lead's 6a64cf pump fix, shutdown)",
        ),
    )
    contract_path = contracts_dir / "main-prefix.json"
    contract_path.write_text(json.dumps(contract, indent=2) + "\n")

    contract_md = [
        "# Main prefix contract — 0x0048F530..0x0048F7AA (prefix only)", "",
        f"Retail `{RETAIL_SHA}`; candidate `{args.candidate_sha}` (source `{CANDIDATE_SOURCE}`). Scope ends at the "
        f"`call QTask_CreateTask` at `{HARD_STOP}`; no invented continuation.", "",
        "## Static identity", "",
        f"- BN: `{(contract['static_evidence']['bn']['name'] or '')}`, "
        f"{contract['static_evidence']['bn']['blocks']} blocks / "
        f"{contract['static_evidence']['bn']['edges']} edges; r2 nodes/edges "
        f"{contract['static_evidence']['radare2']['nodes']}/{contract['static_evidence']['radare2']['edges']}; "
        f"angr `{(contract['static_evidence']['angr'] or {}).get('result')}` exact entry "
        f"`{(contract['static_evidence']['angr'] or {}).get('function_entry')}`, unresolved jumps "
        f"`{(contract['static_evidence']['angr'] or {}).get('has_unresolved_jumps')}`.",
        f"- Disassembly evidence: `{contract['static_evidence']['disassembly']}` "
        f"(sha256 `{contract['static_evidence']['disassembly_sha256']}`).", "",
        "## Ordered observables", "",
        "| # | callsite | target | expected typed arguments | static note |",
        "| - | -------- | ------ | ------------------------ | ----------- |",
    ]
    for item in SEQUENCE:
        args_text = []
        for expected in item["expected_args"]:
            if expected["kind"] == "string":
                args_text.append(f"slot{expected['slot']}=&quot;{expected['text']}&quot; @{expected['va']}")
            elif expected["kind"] == "int":
                args_text.append(f"slot{expected['slot']}={expected['value']}")
            elif expected["kind"] == "relation":
                args_text.append(f"slot{expected['slot']}~{expected['relation']}")
            else:
                args_text.append(f"slot{expected['slot']}:{expected['kind']}")
        contract_md.append(
            f"| {item['order']} | {item['callsite']} | {item['target_kind']} {item['target']} | "
            f"{'; '.join(args_text) or '-'} | {(item['static_note'] or '').replace('|', '/')} |")
    contract_md += [
        "", "## WNDCLASSA (bounded snapshot at RegisterClassA arg0)", "",
        "| offset | field | expectation |", "| ------ | ----- | ----------- |",
    ]
    for field in WNDCLASS_EXPECTED["fields"]:
        expectation = field.get("value", field.get("relation", ""))
        contract_md.append(f"| 0x{field['offset']:X} | {field['name']} | {expectation} |")
    contract_md += ["", "## CreateWindowExA slots", "",
                    "| slot | name | expectation |", "| ---- | ---- | ----------- |"]
    for slot in CREATE_WINDOW_EXPECTED["slots"]:
        expectation = slot.get("value", slot.get("text", slot.get("derived", slot.get("relation", ""))))
        contract_md.append(f"| {slot['slot']} | {slot['name']} | {expectation} |")
    contract_md += ["", "## Side effects", ""]
    for global_row in contract["static_expectations"]["globals"]:
        contract_md.append(f"- `{global_row['label']}` @ {global_row['va']} len 0x{global_row['length']:X}: "
                           f"{global_row['expectation']}")
    contract_md += ["", "## Static absences on the normal path", ""]
    for absence in ABSENCES:
        contract_md.append(f"- `{absence['what']}`: {absence['detail']}")
    contract_md += ["", "## GetLastError branch note (static evidence wins)", "",
                    contract["static_expectations"]["get_last_error"]["static_branches"], "",
                    contract["static_expectations"]["get_last_error"]["warning"], "",
                    "## Post-boundary (UNOBSERVED-until-reachable, never scored)", ""]
    contract_md += [f"- {item}" for item in POST_BOUNDARY]
    (contracts_dir / "CONTRACT.md").write_text("\n".join(contract_md) + "\n")

    baseline = [
        "# Main prefix static baseline (no launch performed)", "",
        f"Main `{MAIN}` is BN `{(contract['static_evidence']['bn']['name'] or '')}`, "
        f"0x0048F530-0x0048F911, 994 bytes, 26 blocks, r2/BN CFG jaccard 1.0, angr exact entry.",
        "Prefix scope ends at `0x0048F7AA call 0x00401000 (QTask_CreateTask)`; the probe records only calls "
        "whose return address is inside 0x0048F530..0x0048F7AA.", "",
        "## Resolved static values",
        "",
        f"- Mutex name `IGIMUTEX` @ 0x00541ECC; duplicate text `IGI is already running.` @ 0x00541DBC, "
        f"caption `Error` @ 0x00541E60.",
        "- `g_541d68` @ 0x00541D68 holds 0x0053B534 -> `Innerloop` (used for WNDCLASSA menu/class and both "
        "CreateWindowExA name arguments).",
        "- WNDCLASSA: style 3, lpfnWndProc 0x0048F970, cbClsExtra/cbWndExtra 0, hInstance entry arg1, "
        "hIcon LoadIconA return, hCursor LoadCursorA return, hbrBackground 7, menu==class==0x0053B534.",
        "- Window proc 0x0048F970 (Input_GetDIErrStr); handlers registered in order: "
        "`Window`->0x0048F350, `WinInput`->0x0048F920, `Germany`->0x0048F930, `USA`->0x0048F950.",
        "- Startup call stream: StoreCommandLine(arg2), 0x0048F0D0(arg1), then the 16 literal setters in "
        "disassembly order (`filesys.cfg`, `Innerloop`, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, "
        "SetLanguage(\"\") via push 0x00567C74, 0).",
        "- GetSystemMetrics order: arg 1 (SM_CYSCREEN) at 0x0048F739, then arg 0 (SM_CXSCREEN) at 0x0048F747; "
        "X=trunc((cx-0x280)/2), Y=trunc((cy-0x1e0)/2).",
        "- CreateWindowExA 12 args: exstyle 0, class/title `Innerloop`, style = getter 0x0048F130 return == 0 "
        "? 0x37C80000 : 0x96000000, X/Y as above, 0x280x0x1E0, parent/menu 0, hInstance arg1, lpParam 0.",
        "- StoreWindowHandle 0x0048F0E0 writes [0x005C8BC4]=hwnd; memset 0x005C8D28..+0xC0 clears 0x30 dwords; "
        "0x005C8DF0=0x005C8DF4, 0x005C8DF8=0x005C8DF0, [0x005C8DF4]=0.",
        "- 0x00567C74 (`g_script_ret_type`) is a zero byte -> SetLanguage takes the empty string.",
        "- QTask_CreateTask entry at 0x0048F7AA is the last comparable point; its return stores AL to "
        "[0x005C8DE8] at 0x0048F7B9, i.e. after the boundary.", "",
        "## Runtime values still to capture (retail first, Lead-authorized lease only)",
        "",
        "1. Entry 4 stack words and caller return address 0x004A51BE (main named/CRT caller 0x4A50F5).",
        "2. CreateMutexA/GetLastError results and duplicate-path absence in the normal run.",
        "3. CoInitialize/GetForegroundWindow/LoadIconA/LoadCursorA/RegisterClassA args and returns.",
        "4. Full WNDCLASSA bytes (0x28) with decoded fields and `Innerloop` text.",
        "5. The 16 startup setters' argument words and exact literal bytes in order.",
        "6. ParseCmdLineArgs four (name, handler VA) pairs.",
        "7. Display_DinputDll output mode, Display_GetScreenMode getter return, GetSystemMetrics returns, "
        "derived X/Y/style.",
        "8. CreateWindowExA 12 args and hwnd; ShowCursor(0)/ShowWindow(hwnd,10); [0x005C8BC4]==hwnd.",
        "9. QTask_CreateTask entry and retail return value (candidate expected entry-only while it throws).",
        "",
        "## Historical context (not re-launched)",
        "",
        "The earlier QTask captures in build/tester/20261024T002148Z cover QTask_CreateTask/"
        "QTask_RegisterClasses, not main. They remain referenced for the registrar chain only; this main "
        "contract starts from fresh retail capture when the Lead grants the lease.",
    ]
    (run_dir / "BASELINE.md").write_text("\n".join(baseline) + "\n")

    readme = [
        "# Main prefix probe/contract (analysis only, nothing launched)", "",
        f"Scope: `{MAIN}` entry to `{HARD_STOP}` (`call QTask_CreateTask`). "
        "Post-boundary is explicitly UNOBSERVED-until-reachable.", "",
        f"- Probe: `probes/probe_main_prefix.js` (sha256 `{probe_sha}`), generated from "
        "`scripts/tester/frida_main_prefix_probe.js`.",
        f"- Contract: `contracts/main-prefix.json`, summary `contracts/CONTRACT.md`.",
        f"- Baseline: `BASELINE.md`; static evidence `static/0x0048f530-static.json` + copied disassembly.", "",
        "## Capture commands (retail first, one process at a time; lease from Lead)", "",
        "```",
        "python3 scripts/tester/frida_probe_driver.py \\",
        "  --exe /Users/haseeb-mir/Downloads/IGI/igi.exe \\",
        f"  --probe {run_dir.relative_to(ROOT)}/probes/probe_main_prefix.js \\",
        "  --target 0x0048F530 --name main --level 1 --observe 30 \\",
        f"  --out {run_dir.relative_to(ROOT)}/reports/retail-main.json \\",
        f"  --expect-exe-sha256 {RETAIL_SHA}",
        "```",
        "",
        "Candidate capture is NOT authorized yet; when the Lead permits it:",
        "```",
        "python3 scripts/tester/frida_probe_driver.py \\",
        "  --exe /Users/haseeb-mir/Downloads/IGI/igi1.exe \\",
        f"  --probe {run_dir.relative_to(ROOT)}/probes/probe_main_prefix.js \\",
        "  --target 0x0048F530 --name main-candidate --level 1 --observe 30 \\",
        f"  --out {run_dir.relative_to(ROOT)}/reports/candidate-main.json \\",
        f"  --expect-exe-sha256 {args.candidate_sha}",
        "```",
        "",
        "The probe records only calls whose return address lies in the prefix. It never dereferences an "
        "unproven pointer: C strings are read only at the 14 proven literal addresses, and the WNDCLASSA "
        "snapshot is bounded to 0x28 bytes after a range check. IAT slots are resolved at hook time.",
        "",
        "## Verdict rule",
        "",
        "A typed main-prefix comparator is the next step after the first capture. Verdicts must be "
        "`PREFIX_MATCH` / `MISMATCH` / `UNOBSERVED` / `BLOCKED`; post-boundary rows are always "
        "UNOBSERVED-until-reachable and cannot be scored. No manifest status is edited by this tooling.",
    ]
    (run_dir / "README.md").write_text("\n".join(readme) + "\n")

    inventory = dict(kind="igi-tester-main-prefix-gate", schema_version=1,
                     retail=dict(path="/Users/haseeb-mir/Downloads/IGI/igi.exe", sha256=RETAIL_SHA),
                     candidate=dict(path="/Users/haseeb-mir/Downloads/IGI/igi1.exe", sha256=args.candidate_sha,
                                    source_commit=CANDIDATE_SOURCE),
                     scope=dict(start=START, end=HARD_STOP, prefix_only=True),
                     probe=dict(path=str(probe_path.relative_to(run_dir)), sha256=probe_sha),
                     contract=dict(path=str(contract_path.relative_to(run_dir)),
                                   sha256=sha256(contract_path)),
                     static=dict(json=contract["static_evidence"]["bn_function"],
                                 disassembly=contract["static_evidence"]["disassembly"],
                                 disassembly_sha256=contract["static_evidence"]["disassembly_sha256"]),
                     calls=len(CALLS), sequence_rows=len(SEQUENCE), proven_strings=len(PROVEN_STRINGS),
                     globals=GLOBALS)
    (run_dir / "inventory.json").write_text(json.dumps(inventory, indent=2) + "\n")
    print(f"wrote main prefix probe {probe_path} sha256 {probe_sha}")
    print(f"wrote contract {contract_path} with {len(SEQUENCE)} ordered rows")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
