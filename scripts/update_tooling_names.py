#!/usr/bin/env python3
"""Update the tooling that hardcodes a native name.

The differential runners, the Frida probe and the generated manifest all refer to
a native by name. When a native is renamed, those references have to follow it.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SCRIPTS = ROOT / "scripts"

RENAMES = {
    "Game_DataSymbolLoad": "sprintf",
    "Camera_ApplyWideScreen_11": "_output",
    "Camera_ApplyHandiCam_11": "_flsbuf",
    "Menu_DrawWidget_9": "ListBox_AddItem",
    "FX_FragmentSimulate_27": "AppMain_ParseCmdLineArgs",
    "Display_CloseDevice": "Keyboard_Update",
    "Debug_Error": "LDebug_Error",
    "Render_ClipMesh_5": "AnimController_UpdateChannels",
    "Timer_SetLimit": "SoundSys_SetFrequency",
    "SoundSys_InitSoundSys_11": "DebugText_SetCursor",
    "SoundSys_PlaySoundSample_11": "DebugText_Clear",
    "Physics_InterpolateWaypoint_10": "Joypad_Update",
}
TEXT_SUFFIXES = {".py", ".js", ".md", ".json", ".txt"}
# the rename table itself and the record of what was renamed
SKIP = {"apply_native_renames.py", "native_rename_report.json", "update_tooling_names.py"}


def main() -> None:
    apply = "--apply" in sys.argv
    changed = []
    for path in sorted(SCRIPTS.rglob("*")):
        if not path.is_file() or path.suffix not in TEXT_SUFFIXES:
            continue
        if path.name in SKIP or "GameNatives/" in path.as_posix():
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        new_text = text
        hits = []
        for old, new in RENAMES.items():
            pattern = re.compile(rf"\b{re.escape(old)}\b")
            if pattern.search(new_text):
                new_text = pattern.sub(new, new_text)
                hits.append(f"{old}->{new}")
        if new_text != text:
            path.write_text(new_text, encoding="utf-8")
            changed.append({"path": str(path.relative_to(ROOT)), "renames": hits})
            print(f"  {path.relative_to(ROOT)}: {', '.join(hits)}")
    print(f"files updated: {len(changed)}")
    if not apply:
        print("(dry run)")


if __name__ == "__main__":
    main()
