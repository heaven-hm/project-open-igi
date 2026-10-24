#!/usr/bin/env python3
"""EU-native coverage audit for the 55 addresses in GameNatives/EU_NATIVES_BN_COPY.md.

Read-only. Reads the EU table, the native manifest, evidence artifacts, engine
sources, the candidate PE symbol table and Binary Ninja MCP; writes only the
report paths given on the command line.

For every address it records, with evidence rather than inference:
  - current Binary Ninja identity/prototype (cached per run)
  - engine implementation record (manifest path, else an explicit
    `retail VA 0x...` provenance marker in the source) and whether that source
    is compiled by the candidate build
  - GameNatives decompiled copies on disk
  - static evidence artifacts and whether their hashes still match the manifest
  - whether the native is linked into the candidate PE (exact retail name,
    decorated/normalized variant, or C++-mangled variant) - three distinct facts
  - concrete gap/blocker list

No status is promoted anywhere: this produces coverage facts only.
"""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
EU_DOC = ROOT / "scripts/GameNatives/EU_NATIVES_BN_COPY.md"
MANIFEST = ROOT / "docs/fidelity/native-verification.json"
EVIDENCE = ROOT / "docs/fidelity/evidence"
PROVENANCE_RE = re.compile(r"retail\s+va\s*[: ]\s*0x([0-9a-f]{8})", re.IGNORECASE)


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def parse_eu_table(text: str) -> list[dict]:
    rows = []
    in_table = False
    for line in text.splitlines():
        if line.startswith("| Retail VA |"):
            in_table = True
            continue
        if in_table:
            if not line.startswith("|"):
                break
            cells = [c.strip() for c in line.strip().strip("|").split("|")]
            if len(cells) != 4 or set(cells[0]) <= set("-: "):
                continue
            m = re.match(r"`?(0x[0-9A-Fa-f]{8})`?", cells[0])
            if not m:
                continue
            rows.append(dict(
                address=m.group(1).lower(),
                eu_bn_name=cells[1],
                eu_status=cells[2],
                eu_file=cells[3] if cells[3] != "-" else None,
            ))
    return rows


def parse_manifest() -> dict[str, dict]:
    data = json.loads(MANIFEST.read_text())
    out: dict[str, dict] = {}
    duplicates: dict[str, list] = {}
    for record in data.get("natives", []):
        addr = str(record.get("address", "")).lower()
        if addr in out:
            duplicates.setdefault(addr, [out[addr].get("name")]).append(record.get("name"))
        out[addr] = record
    if duplicates:
        print(f"WARNING: duplicate manifest addresses: {duplicates}", file=sys.stderr)
    return out


def run_nm(exe: Path, demangle: bool) -> str:
    nm = "i686-w64-mingw32-nm" if shutil.which("i686-w64-mingw32-nm") else "nm"
    cmd = [nm] + (["-C"] if demangle else []) + [str(exe)]
    proc = subprocess.run(cmd, capture_output=True, text=True, timeout=600)
    if proc.returncode != 0:
        raise SystemExit(f"nm failed on {exe}: {proc.stderr[:400]}")
    return proc.stdout


def normalize(name: str) -> str:
    """Name form used only to detect a decorated/C++ variant, never an exact match."""
    return re.sub(r"[^a-z0-9]", "", name.lower())


def parse_symbols(text: str, demangled: bool) -> dict[str, dict]:
    """Map lookup name -> symbol record.

    Exact names are stored under their undecorated spelling. Demangled names are
    stored with their C++ signature stripped and under the final `::` component.
    """
    symbols: dict[str, dict] = {}
    for line in text.splitlines():
        parts = line.split(None, 2)
        if len(parts) != 3:
            continue
        addr, typ, raw = parts
        if typ.upper() == "U":
            continue
        record = dict(raw=raw, type=typ, address=addr)
        if demangled:
            base = re.sub(r"\(.*$", "", raw)
            short = base.split("::")[-1]
            symbols.setdefault(short, record)
            symbols.setdefault(base, record)
        else:
            name = raw.split("@")[0]
            if name.startswith("_"):
                name = name[1:]
            symbols[name] = record
            m = re.match(r"_?([A-Za-z_][A-Za-z0-9_]*?)@\d+$", raw)
            if m:
                symbols.setdefault(m.group(1), record)
    return symbols


def candidate_symbols(exe: Path) -> dict:
    raw_text = run_nm(exe, demangle=False)
    demangled_text = run_nm(exe, demangle=True)
    exact = parse_symbols(raw_text, demangled=False)
    demangled = parse_symbols(demangled_text, demangled=True)
    normalized = {}
    for name, record in list(demangled.items()) + list(exact.items()):
        normalized.setdefault(normalize(name), record)
    return dict(exact=exact, demangled=demangled, normalized=normalized,
                raw_count=len(raw_text.splitlines()))


def find_link(names: list[str], syms: dict) -> dict:
    """Return the strongest linkage fact for a set of candidate retail names."""
    for name in names:
        if not name or name == "-":
            continue
        if name in syms["exact"]:
            return dict(status="exact", matched_name=name, symbol=syms["exact"][name])
        if name in syms["demangled"]:
            return dict(status="cpp_demangled", matched_name=name, symbol=syms["demangled"][name])
        key = normalize(name)
        if key in syms["normalized"]:
            return dict(status="name_normalized", matched_name=name, symbol=syms["normalized"][key])
    return dict(status="none", matched_name=None, symbol=None)


def compiled_sources(build_dir: Path) -> set[str]:
    cc = build_dir / "compile_commands.json"
    if not cc.is_file():
        return set()
    sources = set()
    for entry in json.loads(cc.read_text()):
        f = entry.get("file", "")
        try:
            sources.add(str(Path(f).resolve().relative_to(ROOT.resolve())))
        except (ValueError, OSError):
            sources.add(f)
    return sources


def evidence_for(address: str) -> list[dict]:
    compact = address[2:].lower()
    found = []
    if not EVIDENCE.is_dir():
        return found
    for path in sorted(EVIDENCE.iterdir()):
        name = path.name.lower()
        if path.is_file() and (name.startswith(address) or name.startswith(compact)):
            found.append(dict(path=str(path.relative_to(ROOT)), sha256=sha256(path)))
    return found


def provenance_implementations(address: str, engine_files: list[Path]) -> list[str]:
    """Engine sources whose provenance comment explicitly binds this retail VA."""
    hits = []
    needle = address.lower()
    for path in engine_files:
        try:
            text = path.read_text(errors="ignore")
        except OSError:
            continue
        for m in PROVENANCE_RE.finditer(text):
            if "0x" + m.group(1).lower() == needle:
                hits.append(str(path.relative_to(ROOT)))
                break
    return sorted(hits)


def bn_lookup(address: str) -> dict:
    sys.path.insert(0, str(ROOT / "scripts"))
    import bn_mcp  # noqa: E402

    result: dict = dict(queried=True, found=False)
    try:
        info = bn_mcp.call_tool("bn_function_info", {"function": address})
        text = info if isinstance(info, str) else json.dumps(info)
        if "errorCode" in text:
            result["error"] = text[:300]
        m = re.search(r'^name:\s*"([^"]+)"', text, re.MULTILINE)
        if m:
            result["found"] = True
            result["name"] = m.group(1)
        try:
            proto = bn_mcp.call_tool("bn_function_prototype_get", {"function": address})
            ptext = proto if isinstance(proto, str) else json.dumps(proto)
            m = re.search(r'^definition:\s*"([^"]*)"', ptext, re.MULTILINE)
            if m:
                result["definition"] = m.group(1)
        except Exception as exc:
            result["prototype_error"] = f"{type(exc).__name__}: {exc}"
    except Exception as exc:
        result["error"] = f"{type(exc).__name__}: {exc}"
    return result


def classify(row: dict, manifest: dict[str, dict], syms: dict, compiled: set[str],
             engine_files: list[Path]) -> dict:
    address = row["address"]
    out = dict(row)
    record = manifest.get(address)
    out["manifest_record"] = bool(record)
    verification = (record or {}).get("verification") or {}
    out["manifest_name"] = record.get("name") if record else None
    out["manifest_status"] = verification.get("status")

    # --- Implementation identity ---
    impl = (record.get("implementation") or {}).get("path") if record else None
    impl_source = "manifest" if impl else None
    out["engine_implementation"] = impl
    out["engine_implementation_present"] = False
    if impl:
        impl_path = ROOT / impl
        out["engine_implementation_present"] = impl_path.is_file()
        if impl_path.is_file():
            out["engine_implementation_sha256"] = sha256(impl_path)
            out["engine_implementation_sha256_matches_manifest"] = (
                out["engine_implementation_sha256"] ==
                (record.get("implementation") or {}).get("sha256"))
    else:
        prov = provenance_implementations(address, engine_files)
        out["engine_provenance_sources"] = prov
        if prov:
            impl = prov[0]
            impl_source = "provenance-comment (not in manifest)"
            out["engine_implementation"] = impl
            out["engine_implementation_present"] = (ROOT / impl).is_file()
    out["engine_implementation_source"] = impl_source
    out["engine_source_compiled"] = bool(impl and impl in compiled)

    # --- GameNatives decompiled copies ---
    gn_files = []
    for path in sorted((ROOT / "scripts/GameNatives").rglob(f"*{address[2:]}*")):
        if path.is_file():
            gn_files.append(dict(path=str(path.relative_to(ROOT)), sha256=sha256(path)))
    out["game_natives_files"] = gn_files
    out["game_natives_present"] = bool(gn_files)

    # --- Candidate linkage ---
    names = []
    for candidate in (row["eu_bn_name"], out["manifest_name"]):
        if candidate and candidate != "-" and candidate not in names:
            names.append(candidate)
    signature = ((record or {}).get("game_natives") or {}).get("signature", "")
    m = re.search(r"([A-Za-z_][A-Za-z0-9_]*)\s*\(", signature)
    if m and m.group(1) not in names:
        names.append(m.group(1))
    link = find_link(names, syms)
    out["linkage"] = link
    out["linked_in_candidate"] = link["status"] != "none"
    out["linked_exact_in_candidate"] = link["status"] == "exact"

    # --- Static evidence artifacts + manifest bindings ---
    ev = evidence_for(address)
    out["evidence_artifacts"] = ev
    kinds = set()
    for item in ev:
        kinds.add(Path(item["path"]).name.split("-", 1)[-1].rsplit(".", 1)[0])
    out["evidence_kinds"] = sorted(kinds)
    stale = []
    for key in ("binary_ninja", "radare2", "semantic_review", "behavioral", "build"):
        ref = verification.get(key)
        if not isinstance(ref, dict):
            continue
        path = ref.get("evidence_path")
        if not path:
            continue
        expected = ref.get("evidence_sha256")
        p = ROOT / path
        if not p.is_file():
            stale.append(dict(kind=key, path=path, problem="missing"))
        elif expected and sha256(p) != expected:
            stale.append(dict(kind=key, path=path, problem="sha256 mismatch"))
    out["evidence_stale_or_missing"] = stale

    # --- Binary Ninja re-check ---
    bn = bn_lookup(address)
    out["binary_ninja"] = bn
    recheck = []
    if not bn.get("found"):
        recheck.append("BN has no function at this VA" + (" (query error)" if bn.get("error") else ""))
    else:
        if row["eu_bn_name"] not in ("-", None) and bn.get("name") != row["eu_bn_name"]:
            recheck.append(
                f"BN name drift: EU copy {row['eu_bn_name']!r} vs BN {bn.get('name')!r}")
        if out["manifest_name"] and bn.get("name") != out["manifest_name"]:
            recheck.append(
                f"manifest name drift: manifest {out['manifest_name']!r} vs BN {bn.get('name')!r}")
    out["needs_bn_recheck"] = recheck

    # --- Gaps (facts only) ---
    gaps = []
    if not out["engine_implementation_present"]:
        gaps.append("no engine implementation in this checkout")
    elif not out["engine_source_compiled"]:
        gaps.append("engine source not in candidate compile_commands.json")
    if link["status"] == "none":
        gaps.append("native not linked in candidate PE (no symbol for retail name or variant)")
    elif link["status"] != "exact":
        gaps.append(f"linked only as {link['status']} variant "
                    f"({(link.get('symbol') or {}).get('raw')}), not the exact retail symbol name")
    if not out["manifest_record"]:
        gaps.append("no manifest record (gate cannot select evidence/status)")
    if out["evidence_stale_or_missing"]:
        gaps.append("manifest evidence missing/stale: " + ", ".join(
            f"{x['kind']}:{x['problem']}" for x in out["evidence_stale_or_missing"]))
    if not out["evidence_kinds"]:
        gaps.append("no docs/fidelity/evidence artifacts")
    if recheck:
        gaps.append("BN re-check needed: " + "; ".join(recheck))
    out["gaps"] = gaps
    return out


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--run-dir", type=Path, required=True)
    ap.add_argument("--candidate", type=Path,
                    default=ROOT / "build-win32-no-verify-gate/igi1.exe")
    ap.add_argument("--build-dir", type=Path,
                    default=ROOT / "build-win32-no-verify-gate")
    ap.add_argument("--bn-cache", type=Path, default=None,
                    help="JSON cache of BN results (default: <run-dir>/bn-cache.json)")
    ap.add_argument("--skip-bn", action="store_true")
    args = ap.parse_args()

    args.run_dir.mkdir(parents=True, exist_ok=True)
    cache_path = args.bn_cache or args.run_dir / "bn-cache.json"
    bn_cache: dict[str, dict] = {}
    if cache_path.is_file():
        try:
            bn_cache = json.loads(cache_path.read_text())
        except ValueError:
            bn_cache = {}

    rows = parse_eu_table(EU_DOC.read_text())
    if len(rows) != 55:
        print(f"WARNING: expected 55 EU rows, parsed {len(rows)}", file=sys.stderr)
    manifest = parse_manifest()
    if not args.candidate.is_file():
        raise SystemExit(f"candidate missing: {args.candidate}")
    syms = candidate_symbols(args.candidate)
    compiled = compiled_sources(args.build_dir)
    engine_files = [p for p in (ROOT / "engine").rglob("*")
                    if p.suffix.lower() in (".c", ".cc", ".cpp", ".h", ".hpp")]

    global bn_lookup  # noqa: PLW0603
    if args.skip_bn:
        bn_lookup = lambda addr: dict(queried=False, found=None)  # noqa: E731
    elif bn_cache:
        cached = bn_cache
        real_lookup = bn_lookup

        def cached_lookup(addr: str, _cache=cached, _real=real_lookup) -> dict:
            if addr in _cache:
                return dict(_cache[addr], cached=True)
            return _real(addr)

        bn_lookup = cached_lookup  # noqa: PLW0603

    records = []
    try:
        for i, row in enumerate(rows, 1):
            print(f"[{i}/{len(rows)}] {row['address']} {row['eu_bn_name']}", file=sys.stderr)
            records.append(classify(row, manifest, syms, compiled, engine_files))
    finally:
        for r in records:
            bn = r.get("binary_ninja")
            if isinstance(bn, dict) and bn.get("queried"):
                bn_cache[r["address"]] = {k: v for k, v in bn.items() if k != "cached"}
        cache_path.write_text(json.dumps(bn_cache, indent=1) + "\n")

    counts = {
        "eu_total": len(records),
        "engine_implementation_present": sum(r["engine_implementation_present"] for r in records),
        "manifest_record": sum(r["manifest_record"] for r in records),
        "linked_in_candidate": sum(r["linked_in_candidate"] for r in records),
        "linked_exact_in_candidate": sum(r["linked_exact_in_candidate"] for r in records),
        "game_natives_present": sum(r["game_natives_present"] for r in records),
        "has_evidence_artifacts": sum(bool(r["evidence_kinds"]) for r in records),
        "bn_found": sum(bool((r.get("binary_ninja") or {}).get("found")) for r in records),
        "needs_bn_recheck": sum(bool(r["needs_bn_recheck"]) for r in records),
        "with_gaps": sum(bool(r["gaps"]) for r in records),
    }
    report = dict(
        schema_version=1,
        kind="igi-eu-natives-coverage",
        generated_at=datetime.now(timezone.utc).isoformat(),
        inputs=dict(
            eu_doc=str(EU_DOC.relative_to(ROOT)),
            eu_doc_sha256=sha256(EU_DOC),
            manifest=str(MANIFEST.relative_to(ROOT)),
            candidate=str(args.candidate),
            candidate_sha256=sha256(args.candidate),
            candidate_symbols=len(syms["exact"]),
            compiled_sources=len(compiled),
        ),
        counts=counts,
        notes=[
            "Facts only. Present/linked/evidence presence are not verification and promote no status.",
            "linked_exact_in_candidate: a defined candidate symbol carries the exact retail/BN name.",
            "linked_in_candidate additionally accepts decorated/name-normalized or C++-demangled variants; "
            "see per-native linkage.status. Variants are symbol-binding gaps for typed probes.",
            "needs_bn_recheck lists BN identity disagreements with the EU copy note or the manifest.",
        ],
        natives=records,
    )
    out_json = args.run_dir / "eu-natives-coverage.json"
    out_json.write_text(json.dumps(report, indent=2) + "\n")

    lines = [
        "# EU-native coverage (55 addresses)",
        "",
        f"Generated: {report['generated_at']}",
        f"Candidate: `{args.candidate}` sha256 `{report['inputs']['candidate_sha256']}`",
        f"EU doc: `{EU_DOC.relative_to(ROOT)}` sha256 `{report['inputs']['eu_doc_sha256']}`",
        "",
        "| Metric | Count |",
        "|---|---|",
    ]
    for k, v in counts.items():
        lines.append(f"| {k} | {v} |")
    lines += [
        "",
        "| VA | BN name | Manifest status | Engine impl | Linked | Evidence | BN re-check |",
        "|---|---|---|---|---|---|---|",
    ]
    for r in records:
        lines.append("| {addr} | {name} | {man} | {impl} | {linked} | {ev} | {bn} |".format(
            addr=r["address"],
            name=r.get("eu_bn_name") or "-",
            man=r.get("manifest_status") or ("no record" if not r.get("manifest_record") else "-"),
            impl="yes" if r["engine_implementation_present"] else "no",
            linked=(r["linkage"]["status"] if r["linked_in_candidate"] else "no"),
            ev=",".join(r["evidence_kinds"]) or "-",
            bn="; ".join(r["needs_bn_recheck"]) or "-",
        ))
    out_md = args.run_dir / "eu-natives-coverage.md"
    out_md.write_text("\n".join(lines) + "\n")
    print(json.dumps(counts, indent=1))
    print(f"wrote {out_json}")
    print(f"wrote {out_md}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
