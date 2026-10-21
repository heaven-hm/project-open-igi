#!/usr/bin/env python3
"""Align the manifest with the confirmed catalog and repair every reference.

A rename moves the GameNatives file and changes the native's name, so the
manifest can be left pointing at a path that no longer exists and carrying a
name the catalog no longer uses. This:

  * takes the name from the confirmed catalog for every address it covers,
  * resolves the GameNatives file for each address from what is on disk,
  * refreshes the recorded hashes of the GameNatives file, the implementation
    file and the evidence files, and
  * reports anything it could not resolve.
"""

import hashlib
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
CATALOG = ROOT / "scripts" / "IGINatives_v4.json"
GN = ROOT / "scripts" / "GameNatives"
FILENAME = re.compile(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$")


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def build_index():
    """Every GameNatives file, keyed by its address."""
    index: dict[str, list[tuple[str, Path]]] = {}
    for path in GN.rglob("*"):
        if not path.is_file():
            continue
        m = FILENAME.match(path.name)
        if m:
            index.setdefault(m.group(1).upper(), []).append((m.group(2), path))
    return index


def pick(index, key: str, want: str, old_name: str, old_ext: str, old_disamb: str):
    """Choose the file for an address: the canonical name first, then the
    variant that matches the one the manifest recorded."""
    candidates = index.get(key, [])
    if not candidates:
        return None
    fallback_cpp = None
    fallback_header = None
    # the candidate source is the .cpp, so it is preferred over the header
    for extension in (old_ext, ".cpp", ".hpp"):
        for name, path in candidates:
            if (FILENAME.match(path.name).group(4) or "") != extension:
                continue
            if name.lower() != want.lower():
                continue
            # the base file is preferred over the " 2" duplicate: the duplicate
            # includes a header that does not exist
            disamb = FILENAME.match(path.name).group(3) or ""
            if extension == ".cpp" and not disamb.strip():
                return path
            if extension == ".cpp":
                fallback_cpp = path
            else:
                fallback_header = path
    for name, path in candidates:
        m = FILENAME.match(path.name)
        if m and (m.group(4) or "") == old_ext and (m.group(3) or "") == old_disamb:
            return path
    for name, path in candidates:
        if name.lower() == old_name.lower():
            return path
    if fallback_cpp:
        return fallback_cpp
    if fallback_header:
        return fallback_header
    return sorted(candidates)[0][1]


def main() -> None:
    apply = "--apply" in sys.argv
    manifest = json.loads(MANIFEST.read_text())
    catalog = {e["address"].upper(): e for e in json.loads(CATALOG.read_text())["Natives"]}
    index = build_index()

    renamed, repathed, rehashed, unresolved = [], [], [], []
    for record in manifest["natives"]:
        address = record["address"].upper()
        key = f"{int(address, 16):08X}"
        entry = catalog.get(address)

        old_name = record.get("name", "")
        if entry and entry["name"] != old_name:
            renamed.append({"address": address, "from": old_name, "to": entry["name"]})
            record["name"] = entry["name"]

        want = record["name"]
        reference = record.get("game_natives") or {}
        old_rel = reference.get("path", "")
        old_base = Path(old_rel).name
        om = FILENAME.match(old_base)
        old_ext = (om.group(4) or "") if om else ".cpp"
        old_disamb = (om.group(3) or "") if om else ""

        path = pick(index, key, want, old_name, old_ext, old_disamb)
        if path is None:
            unresolved.append({"address": address, "name": want, "path": old_rel})
            continue
        rel = path.relative_to(ROOT).as_posix()
        if rel != old_rel:
            repathed.append({"address": address, "from": old_rel, "to": rel})
        reference["path"] = rel
        digest = sha256(path)
        if reference.get("sha256") != digest:
            reference["sha256"] = digest
            rehashed.append({"address": address, "path": rel})
        record["game_natives"] = reference

        # a candidate record uses the GameNatives file as its implementation
        implementation = record.get("implementation") or {}
        if implementation.get("path", "").startswith("scripts/GameNatives/"):
            implementation["path"] = rel
            implementation["sha256"] = digest
        elif implementation.get("path"):
            # the implementation file is the one the gate hashes and compares
            implementation_path = ROOT / implementation["path"]
            if implementation_path.exists():
                current = sha256(implementation_path)
                if implementation.get("sha256") != current:
                    implementation["sha256"] = current
                    rehashed.append({"address": address,
                                     "path": implementation["path"],
                                     "kind": "implementation"})
        record["implementation"] = implementation

    # the evidence files are regenerated when the source they bind to changed
    for record in manifest["natives"]:
        verification = record.get("verification") or {}
        for key_name in ("binary_ninja", "radare2", "semantic_review", "build"):
            reference = verification.get(key_name)
            if not isinstance(reference, dict):
                continue
            evidence = ROOT / reference.get("evidence_path", "")
            if evidence.exists():
                digest = sha256(evidence)
                if reference.get("evidence_sha256") != digest:
                    reference["evidence_sha256"] = digest
                    rehashed.append({"address": record["address"],
                                     "path": reference["evidence_path"]})

    print(f"names aligned to the catalog : {len(renamed)}")
    print(f"game_natives paths repaired  : {len(repathed)}")
    print(f"hashes refreshed             : {len(rehashed)}")
    print(f"unresolved                   : {len(unresolved)}")
    for r in unresolved[:10]:
        print(f"  UNRESOLVED {r['address']} {r['name']}: {r['path']}")
    for r in renamed[:10]:
        print(f"  renamed {r['address']}: {r['from']} -> {r['to']}")

    if apply:
        MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n")
        print(f"written {MANIFEST}")


if __name__ == "__main__":
    main()
