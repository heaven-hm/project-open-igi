#!/usr/bin/env python3
"""Apply the corrected native names everywhere.

Each entry was decided from the retail body in igi.exe and the authoritative
name sources: the IGIPatch list, the native name evidence file and the
confirmed catalogs.

The name is applied to:
  * the Binary Ninja database, through the MCP server,
  * the scripts/GameNatives filenames, the declaration inside them and their
    provenance comments,
  * the engine sources: the function name, its declaration, its call sites and
    the comments that mention it,
  * the native catalogs, and the manifest record that pins each file.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
from bn_client import BN  # noqa: E402

GN = ROOT / "scripts" / "GameNatives"
ENGINE = ROOT / "engine"
CATALOGS = (ROOT / "assets" / "IGINatives_v4.json",
            ROOT / "scripts" / "IGINatives_v4.json",
            ROOT / "assets" / "IGINatives_All_GameNatives.json")
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
FILENAME = re.compile(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$")

# address -> (old name, new name)
RENAMES = {
    "0x004A53B3": ("Game_DataSymbolLoad", "sprintf"),
    "0x004A7720": ("Camera_ApplyWideScreen_11", "_output"),
    "0x004A760B": ("Camera_ApplyHandiCam_11", "_flsbuf"),
    "0x0041F2D0": ("Menu_DrawWidget_9", "ListBox_AddItem"),
    "0x0048F360": ("FX_FragmentSimulate_27", "AppMain_ParseCmdLineArgs"),
    "0x00490230": ("Display_CloseDevice", "Keyboard_Update"),
    "0x004AF7B0": ("Debug_Error", "LDebug_Error"),
    "0x004D3210": ("Render_ClipMesh_5", "AnimController_UpdateChannels"),
    "0x004E6030": ("Timer_SetLimit", "SoundSys_SetFrequency"),
    "0x004E7B80": ("SoundSys_InitSoundSys_11", "DebugText_SetCursor"),
    "0x004E7BB0": ("SoundSys_PlaySoundSample_11", "DebugText_Clear"),
    "0x00509CF0": ("Physics_InterpolateWaypoint_10", "Joypad_Update"),
}


def replace_in_tree(root: Path, old: str, new: str, suffixes) -> list[str]:
    """Replace a whole-word identifier in the matching files under a tree."""
    changed = []
    pattern = re.compile(rf"\b{re.escape(old)}\b")
    for path in root.rglob("*"):
        if not path.is_file() or path.suffix not in suffixes:
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        if not pattern.search(text):
            continue
        path.write_text(pattern.sub(new, text), encoding="utf-8")
        changed.append(str(path.relative_to(ROOT)))
    return changed


def main() -> None:
    apply = "--apply" in sys.argv
    bn = BN()

    report = []
    for address, (old, new) in RENAMES.items():
        entry = {"address": address, "from": old, "to": new}

        # 1. Binary Ninja
        current = bn.header(bn.call("bn_function_info", {"function": address}), "name")
        entry["bnCurrent"] = current
        if current and current != new:
            ok, msg = bn.rename(address, new)
            entry["bnRenamed"] = ok
            if not ok:
                entry["bnError"] = msg
        else:
            entry["bnRenamed"] = True

        # 2. GameNatives: the files, the identifiers and the comments
        key = f"{int(address, 16):08X}"
        gn_files = []
        for path in GN.rglob(f"{key}-*"):
            m = FILENAME.match(path.name)
            if not m:
                continue
            target = path.with_name(f"{key}-{new}{m.group(3) or ''}{m.group(4)}")
            if target != path and target.exists():
                entry.setdefault("gnConflicts", []).append(target.name)
            gn_files.append(str(path.relative_to(ROOT)))
        entry["gameNatives"] = gn_files

        # 3. the identifiers, in the GameNatives corpus and in the engine.
        #    Each old name identifies exactly one retail function, so the call
        #    sites of that function are renamed with it.
        entry["gameNativesText"] = replace_in_tree(GN, old, new, {".cpp", ".hpp", ".h"})
        entry["engine"] = replace_in_tree(ENGINE, old, new, {".cpp", ".hpp", ".h", ".cc", ".cxx"})

        report.append(entry)
        print(f"{address}  {old} -> {new}  bn={entry.get('bnRenamed')} "
              f"gnFiles={len(gn_files)} gnText={len(entry['gameNativesText'])} "
              f"engine={len(entry['engine'])}")

    if apply:
        bn.call("bn_analysis_update_and_wait", {})

        # 2. the GameNatives filenames move to the corrected name
        for entry in report:
            key = f"{int(entry['address'], 16):08X}"
            new = entry["to"]
            for path in list(GN.rglob(f"{key}-*")):
                m = FILENAME.match(path.name)
                if not m:
                    continue
                target = path.with_name(f"{key}-{new}{m.group(3) or ''}{m.group(4)}")
                if target != path and not target.exists():
                    path.rename(target)

        bn.call("bn_open_item_save", {"openItem": "item_1"})

        # 4. the catalogs and the manifest
        for path in CATALOGS:
            if not path.exists():
                continue
            doc = json.loads(path.read_text())
            for item in doc.get("Natives", []):
                key = f"0x{int(str(item['address']), 16):08X}"
                rename = RENAMES.get(key)
                if not rename:
                    continue
                old, new = rename
                if item.get("name") == old:
                    item["name"] = new
                if item.get("bnName") == old:
                    item["bnName"] = new
                if "signature" in item and old in item["signature"]:
                    item["signature"] = re.sub(rf"\b{re.escape(old)}\b", new, item["signature"])
            path.write_text(json.dumps(doc, indent=2) + "\n")

        manifest = json.loads(MANIFEST.read_text())
        for item in manifest["natives"]:
            key = f"0x{int(str(item['address']), 16):08X}"
            rename = RENAMES.get(key)
            if not rename:
                continue
            old, new = rename
            if item.get("name") == old:
                item["name"] = new
            for holder in ("implementation", "game_natives"):
                reference = item.get(holder) or {}
                if isinstance(reference, dict) and old in reference.get("path", ""):
                    reference["path"] = reference["path"].replace(old, new)
        MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n")
        print(f"\nwritten {len(CATALOGS)} catalogs and the manifest")

    dest = ROOT / "assets" / "native_rename_report.json"
    dest.write_text(json.dumps(report, indent=2) + "\n")
    print(f"written {dest}")


if __name__ == "__main__":
    main()
