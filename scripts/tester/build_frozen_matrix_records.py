#!/usr/bin/env python3
"""Build the 29 linked per-native records for a frozen matrix run dir.

Reads build/tester/<run>/static + runtime and writes build/tester/<run>/individual.
The 30th native (0x00410360) is manual because it is not linked in the candidate.
"""
import json, subprocess, sys
from pathlib import Path
ROOT=Path('/Users/haseeb-mir/Documents/Code/project-open-igi')
import os
run=Path(os.environ.get('TESTER_RUN_DIR') or open('/tmp/tester-run-dir').read().strip())
run_abs=(ROOT/run).resolve()
groups = {
 'globals': (["0x0048F120","0x0048F130","0x0048F200","0x0048F210","0x0048F280","0x0048F290","0x0048F2A0","0x0048F2B0","0x0048F300"], None),
 'methods': (["0x0048F0C0","0x0048F0D0","0x0048F0F0","0x0048F100","0x0048F140","0x0048F1A0","0x0048F1C0","0x0048F1E0","0x0048F240","0x0048F260"], None),
 'parser': (["0x0048F360","0x004A5A0C"], None),
 'window': (["0x0048F350"], None),
 'wininput': (["0x0048F920"], None),
 'germany': (["0x0048F930"], None),
 'usa': (["0x0048F950"], None),
 'hardcoregerman': (["0x0048F960"], None),
 'trio': (["0x004C1800","0x004C1790","0x004C17C0"], {"0x004C1800":"new","0x004C1790":"register","0x004C17C0":"remove"}),
}
NAMES={
 "0x0048F120":"Display_SetScreenMode_48F120","0x0048F130":"Display_GetScreenMode_48F130",
 "0x0048F200":"AppMain_Set5C8C04_48F200","0x0048F210":"Game_SetStartupMode_48F210",
 "0x0048F280":"Input_SetUseWin32Input_48F280","0x0048F290":"AppMain_Set5C8C0C_48F290",
 "0x0048F2A0":"AppMain_Set5C8C10_48F2A0","0x0048F2B0":"AppContext_SetGermanVersion_48F2B0",
 "0x0048F300":"AppContext_SetLanguage_48F300",
 "0x0048F0C0":"StoreCommandLine","0x0048F0D0":"StoreInstanceHandle","0x0048F0F0":"SetConfigFileName",
 "0x0048F100":"SetAppTitle_48F100","0x0048F140":"AppContext_SetIsActive",
 "0x0048F1A0":"App_ContextSetDebugged","0x0048F1C0":"SetDebugCompileState_48F1C0",
 "0x0048F1E0":"App_ContextSetDebugtextState","0x0048F240":"App_ContextSetLightmapsUsed",
 "0x0048F260":"AppContext_SetTerrainLightmapsUsed",
 "0x0048F360":"AppMain_ParseCmdLineArgs_48F360","0x004A5A0C":"AppMain_TokenizeDelimited_4A5A0C",
 "0x0048F350":"AppMain_WindowCommand_48F350","0x0048F920":"AppMain_WinInputCommand_48F920",
 "0x0048F930":"AppMain_GermanyCommand_48F930","0x0048F950":"AppMain_USACommand_48F950",
 "0x0048F960":"AppMain_HardcoreGermanCommand_48F960",
 "0x004C1800":"QTaskList_New","0x004C1790":"QTaskList_Register","0x004C17C0":"QTaskList_Remove",
}
LIVE_STATIC_SCOPE = {
    "0x0048F130","0x0048F200","0x0048F210","0x0048F280","0x0048F290","0x0048F2A0",
    "0x0048F2B0","0x0048F300","0x0048F350","0x0048F360","0x0048F920","0x0048F930",
    "0x0048F950","0x0048F960","0x004A5A0C",
}
written=[]
for group,(addresses,invariants) in groups.items():
    for address in addresses:
        invariant=(invariants or {}).get(address,"none")
        cmd=[sys.executable,str(ROOT/'scripts/tester/build_individual_record.py'),
             '--address',address,'--name',NAMES[address],'--invariant',invariant,
             '--static',str(run_abs/'static'/f'{address}-static.json'),
             '--retail',str(run_abs/'runtime'/f'retail-{group}.json'),
             '--candidate',str(run_abs/'runtime'/f'candidate-{group}.json'),
             '--comparison',str(run_abs/'runtime'/f'comparison-{group}.json'),
             '--out',str(run_abs/'individual'/f'{address}.json')]
        if address in LIVE_STATIC_SCOPE:
            cmd.append('--live-static-scope')
        proc=subprocess.run(cmd,capture_output=True,text=True)
        if proc.returncode:
            print('FAILED',address,proc.stderr[-500:]); continue
        written.append(address)
        print(proc.stdout.strip().splitlines()[0] if proc.stdout else address)
print('written',len(written))
