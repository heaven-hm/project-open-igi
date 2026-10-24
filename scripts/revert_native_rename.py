#!/usr/bin/env python3
"""Revert a single native rename.

The owner decided that 0x004AF7B0 keeps the name Debug_Error, so the rename to
LDebug_Error is undone in the Binary Ninja database, in the GameNatives tree and
in the catalogs.
"""

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
from bn_client import BN  # noqa: E402

ADDRESS = "0x004AF7B0"
OLD = "LDebug_Error"
NEW = "Debug_Error"
GN = ROOT / "scripts" / "GameNatives"
CATALOGS = (ROOT / "assets" / "IGINatives_v4.json",
            ROOT / "scripts" / "IGINatives_v4.json",
            ROOT / "assets" / "IGINatives_All_GameNatives.json")
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
FILENAME = re.compile(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$")


def main() -> None:
    apply = "--apply" in sys.argv
    bn = BN()
    key = f"{int(ADDRESS, 16):08X}"

    if apply:
        ok, msg = bn.rename(ADDRESS, NEW)
        print(f"Binary Ninja: {ADDRESS} -> {NEW} ok={ok} {msg}")
        bn.call("bn_analysis_update_and_wait", {})
        bn.call("bn_open_item_save", {"openItem": "item_1"})

        for path in list(GN.rglob(f"{key}-*")):
            m = FILENAME.match(path.name)
            if not m:
                continue
            text = path.read_text(encoding="utf-8", errors="replace")
            text = re.sub(rf"\b{re.escape(OLD)}\b", NEW, text)
            path.write_text(text, encoding="utf-8")
            target = path.with_name(f"{key}-{NEW}{m.group(3) or ''}{m.group(4)}")
            if target != path and not target.exists():
                path.rename(target)
                print(f"  {path.name} -> {target.name}")

        for path in CATALOGS:
            if not path.exists():
                continue
            doc = json.loads(path.read_text())
            for item in doc.get("Natives", []):
                if item.get("name") == OLD:
                    item["name"] = NEW
                if item.get("bnName") == OLD:
                    item["bnName"] = NEW
                if "signature" in item and OLD in item["signature"]:
                    item["signature"] = item["signature"].replace(OLD, NEW)
            path.write_text(json.dumps(doc, indent=2) + "\n")

        manifest = json.loads(MANIFEST.read_text())
        for item in manifest["natives"]:
            if int(str(item["address"]), 16) != int(ADDRESS, 16):
                continue
            if item.get("name") == OLD:
                item["name"] = NEW
            for holder in ("implementation", "game_natives"):
                reference = item.get(holder) or {}
                if isinstance(reference, dict) and OLD in reference.get("path", ""):
                    reference["path"] = reference["path"].replace(OLD, NEW)
        MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n")
        print("catalogs and manifest updated")
    else:
        print(f"would rename {ADDRESS} {OLD} -> {NEW}")


if __name__ == "__main__":
    main()
