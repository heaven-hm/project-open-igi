#!/usr/bin/env python3
"""Static evidence capture for named retail natives (Tester, read-only).

For each retail VA it captures, independently of the Lead's evidence records:
  - Binary Ninja identity, prototype and basic-block CFG (live MCP)
  - Radare2 CFG at the exact entry (live)
  - angr CFGFast exact-entry recovery (live)
  - the repository evidence artifacts referenced by the manifest, with sha256
  - candidate PE symbol presence + disassembly for the same native, and caller
    sites in both binaries

It never promotes a status and never treats a CFG graph alone as behavioral
parity. Output is raw evidence JSON per native; verdicts are written separately
by the tester after review.
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
MANIFEST = ROOT / "docs/fidelity/native-verification.json"
EVIDENCE = ROOT / "docs/fidelity/evidence"
RETAIL = Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")
ANGR_PYTHON = ROOT / ".venv-angr/bin/python"


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(command: list[str], timeout: int = 600) -> dict:
    try:
        done = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, timeout=timeout)
        return dict(command=command, exit_code=done.returncode, stdout=done.stdout, stderr=done.stderr)
    except (OSError, subprocess.SubprocessError) as error:
        return dict(command=command, exit_code=1, error=f"{type(error).__name__}: {error}")


def bn(tool: str, arguments: dict) -> dict:
    result = run([sys.executable, str(ROOT / "scripts/bn_mcp_call.py"), tool, json.dumps(arguments)])
    result["tool"] = tool
    return result


def norm(address) -> str:
    """Normalize a BN hex-string or an r2 JSON integer address to 0x%08x."""
    if isinstance(address, int):
        return f"0x{address:08x}"
    text = str(address).strip().lower()
    if text.startswith("0x"):
        return f"0x{int(text, 16):08x}"
    return f"0x{int(text, 10):08x}"


def parse_bn_blocks(text: str) -> dict:
    """Parse bn_function_basic_blocks table rows into a normalized CFG."""
    nodes, edges = set(), set()
    for line in text.splitlines():
        parts = [part.strip() for part in line.split("|")]
        if len(parts) < 7 or not parts[1].isdigit():
            continue
        source = parts[2].split("-")[0].strip().upper()
        if not source.startswith("0X"):
            continue
        source = norm(source)
        nodes.add(source)
        for kind, target in re.findall(
                r"(TrueBranch|FalseBranch|UnconditionalBranch)\s*#\d+\s*(0x[0-9a-fA-F]+)-", parts[5]):
            edges.add((source, norm(target),
                       "conditional" if kind != "UnconditionalBranch" else "unconditional"))
    return dict(nodes=sorted(nodes), edges=sorted(list(edge) for edge in edges))


def parse_r2_graph(text: str, address: str) -> dict:
    graphs = json.loads(text)
    match = [g for g in graphs if int(g["addr"]) == int(address, 16)]
    if len(match) != 1:
        raise ValueError(f"radare2 recovered {len(match)} graphs at {address}")
    nodes, edges = set(), set()
    for block in match[0]["blocks"]:
        source = norm(block["addr"])
        nodes.add(source)
        for label in ("jump", "fail"):
            target = block.get(label)
            if target not in (None, -1, 0xFFFFFFFFFFFFFFFF):
                edges.add((source, norm(target), "conditional" if "fail" in block else "unconditional"))
    return dict(nodes=sorted(nodes), edges=sorted(list(edge) for edge in edges))


def evidence_cfg(address: str) -> dict:
    out = {}
    compact = address[2:].lower()
    for path in sorted(EVIDENCE.glob(f"*{compact}*.json")):
        name = path.name.lower()
        if "-binaryninja" in name or "-binary_ninja" in name:
            kind = "binary_ninja"
        elif "-radare2" in name:
            kind = "radare2"
        elif "-ghidra" in name:
            kind = "ghidra"
        else:
            continue
        try:
            doc = json.loads(path.read_text())
        except ValueError:
            continue
        cfg = doc.get("cfg") or {}
        nodes = sorted({norm(n.get("address", "0")) for n in cfg.get("nodes", [])})
        edges = sorted({(norm(e.get("source", "0")), norm(e.get("target", "0")), str(e.get("kind")))
                         for e in cfg.get("edges", [])})
        out[kind] = dict(path=str(path.relative_to(ROOT)), sha256=sha256(path),
                         function_entry=doc.get("function_entry"),
                         binary_sha256=doc.get("binary_sha256"),
                         nodes=nodes, edges=edges, edge_count=len(edges))
    return out


def graph_similarity(left: dict, right: dict) -> dict:
    ln, rn = set(left["nodes"]), set(right["nodes"])
    le = {tuple(x) for x in left["edges"]}
    re_ = {tuple(x) for x in right["edges"]}
    def jaccard(a, b):
        return 1.0 if not a and not b else len(a & b) / len(a | b)
    return dict(node_jaccard=jaccard(ln, rn), edge_jaccard=jaccard(le, re_),
                left_nodes=len(ln), right_nodes=len(rn), left_edges=len(le), right_edges=len(re_))


def candidate_facts(candidate: Path, symbol_names: list[str]) -> dict:
    nm = "i686-w64-mingw32-nm" if shutil.which("i686-w64-mingw32-nm") else "nm"
    raw = run([nm, str(candidate)])
    demangled = run([nm, "-C", str(candidate)])
    facts = dict(candidate=str(candidate), sha256=sha256(candidate), symbols={})
    for name in symbol_names:
        hits = []
        for line in raw.get("stdout", "").splitlines():
            parts = line.split(None, 2)
            if len(parts) != 3:
                continue
            addr, typ, rawname = parts
            base = rawname.split("@")[0].lstrip("_")
            if typ.upper() == "U":
                continue
            if base == name or rawname.lstrip("_") == name:
                hits.append(dict(symbol=rawname, address=addr, type=typ, demangled=None))
        for line in demangled.get("stdout", "").splitlines():
            parts = line.split(None, 2)
            if len(parts) != 3:
                continue
            addr, typ, text = parts
            if text.split("(")[0].split("::")[-1] == name and typ.upper() != "U":
                for hit in hits:
                    if hit["address"] == addr:
                        hit["demangled"] = text
        facts["symbols"][name] = hits
    # Caller sites in the candidate image (call rel32/disp to the symbol address).
    objdump = shutil.which("i686-w64-mingw32-objdump") or shutil.which("objdump")
    disasm = run([objdump, "-d", "-M", "intel", str(candidate)], timeout=900) if objdump else {"stdout": ""}
    facts["objdump_available"] = bool(objdump)
    for name, hits in facts["symbols"].items():
        for hit in hits:
            needle = f"{int(hit['address'], 16):x}"
            callers = [line.strip() for line in disasm.get("stdout", "").splitlines()
                       if re.search(r"\b(call|jmp)\b", line) and needle in line.lower()]
            hit["candidate_caller_lines"] = callers[:10]
    return facts


def candidate_disassembly(candidate: Path, address: int, length: int = 96) -> dict:
    objdump = shutil.which("i686-w64-mingw32-objdump") or shutil.which("objdump")
    if not objdump:
        return dict(error="objdump unavailable")
    start = address
    return run([objdump, "-d", "-M", "intel", f"--start-address=0x{start:x}",
                f"--stop-address=0x{start + length:x}", str(candidate)])


def parse_handoff_table(path: Path) -> dict:
    """Parse the Tester handoff markdown table into {VA: expected bytes hex}."""
    expected = {}
    pattern = re.compile(r"^\|\s*(\d+)\s*\|\s*(0x[0-9A-Fa-f]{8})\s*\|\s*`([0-9a-fA-F ]{2,})`")
    for line in path.read_text().splitlines():
        match = pattern.match(line)
        if not match:
            continue
        expected[f"0x{int(match.group(2), 16):08X}"] = match.group(3).replace(" ", "").lower()
    return expected


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--address", action="append", required=True)
    ap.add_argument("--run-dir", type=Path, required=True)
    ap.add_argument("--candidate", type=Path, default=ROOT / "build-win32-no-verify-gate/igi1.exe")
    ap.add_argument("--retail", type=Path, default=RETAIL)
    ap.add_argument("--no-angr", action="store_true")
    ap.add_argument("--handoff", type=Path,
                    help="VERIFICATION-HANDOFF.md whose byte column is compared to the retail image")
    ap.add_argument("--expect-bytes", action="append", default=[], metavar="VA:HEX",
                    help="expected retail bytes at VA, compared against a BN memory read")
    ap.add_argument("--crosscheck-length", action="append", default=[], metavar="VA:LENGTH",
                    help="no external expected bytes: compare BN memory read against r2 pD bytes")
    args = ap.parse_args()

    if sha256(args.retail) != "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01":
        raise SystemExit("retail image hash mismatch")
    manifest = {n["address"].lower(): n for n in json.loads(MANIFEST.read_text())["natives"]}
    handoff_expected = parse_handoff_table(args.handoff) if args.handoff else {}
    for item in args.expect_bytes:
        raw_va, _, raw_hex = item.partition(":")
        handoff_expected[f"0x{int(raw_va, 0):08X}"] = raw_hex.replace(" ", "").lower()
    crosscheck_lengths = {}
    for item in args.crosscheck_length:
        raw_va, _, raw_length = item.partition(":")
        crosscheck_lengths[f"0x{int(raw_va, 0):08X}"] = int(raw_length, 0)
    out_dir = args.run_dir / "static"
    out_dir.mkdir(parents=True, exist_ok=True)

    for raw_address in args.address:
        address = f"0x{int(raw_address, 0):08X}"
        record = dict(schema_version=1, kind="igi-tester-static-evidence",
                      captured_at=datetime.now(timezone.utc).isoformat(),
                      address=address, retail=dict(path=str(args.retail), sha256=sha256(args.retail)),
                      candidate=dict(path=str(args.candidate), sha256=sha256(args.candidate)))
        reg = manifest.get(address.lower())
        record["manifest"] = None
        if reg:
            impl = reg.get("implementation") or {}
            impl_path = ROOT / impl.get("path", "")
            record["manifest"] = dict(
                name=reg.get("name"), status=(reg.get("verification") or {}).get("status"),
                implementation=impl.get("path"),
                implementation_sha256_now=sha256(impl_path) if impl_path.is_file() else None,
                implementation_sha256_recorded=impl.get("sha256"),
                implementation_current=(impl_path.is_file() and sha256(impl_path) == impl.get("sha256")),
                game_natives=(reg.get("game_natives") or {}).get("path"))

        expected_bytes = handoff_expected.get(address)
        if expected_bytes:
            read = bn("bn_memory_read", {"address": address, "length": len(expected_bytes) // 2})
            try:
                payload = json.loads(read.get("stdout", ""))
                actual = (payload.get("hex") or "").lower()
            except ValueError:
                actual = ""
            record["expected_bytes"] = dict(hex=expected_bytes, length=len(expected_bytes) // 2,
                                            actual=actual, match=actual == expected_bytes,
                                            tool=read.get("tool"))

        if address in crosscheck_lengths:
            length = crosscheck_lengths[address]
            read = bn("bn_memory_read", {"address": address, "length": length})
            try:
                bn_hex = (json.loads(read.get("stdout", "")).get("hex") or "").lower()
            except ValueError:
                bn_hex = ""
            p8 = subprocess.run(["r2", "-q", "-2", "-c", f"p8 {length} @ {address}; q", str(args.retail)],
                                capture_output=True, text=True)
            r2_hex = (p8.stdout or "").strip().lower()
            record["bytes_crosscheck"] = dict(method="bn_memory_read vs r2 p8", length=length,
                                              bn_sha256=hashlib.sha256(bn_hex.encode()).hexdigest(),
                                              r2_sha256=hashlib.sha256(r2_hex.encode()).hexdigest(),
                                              bn_hex=bn_hex, r2_hex=r2_hex,
                                              match=bool(bn_hex) and bn_hex == r2_hex)

        # Live BN evidence.
        info = bn("bn_function_info", {"function": address})
        proto = bn("bn_function_prototype_get", {"function": address})
        blocks = bn("bn_function_basic_blocks", {"function": address})
        decompile = bn("bn_function_decompile", {"function": address})
        callers = bn("bn_function_callers", {"function": address})
        record["binary_ninja"] = dict(
            info=info, prototype=proto,
            blocks_text=blocks.get("stdout", ""), blocks_exit=blocks.get("exit_code"),
            decompile_text=decompile.get("stdout", ""),
            decompile_exit=decompile.get("exit_code"),
            callers_text=callers.get("stdout", ""))
        try:
            record["binary_ninja"]["cfg"] = parse_bn_blocks(blocks.get("stdout", ""))
        except ValueError as error:
            record["binary_ninja"]["cfg_error"] = str(error)

        # Live radare2 CFG at the exact entry.
        r2_command = ["r2", "-q", "-2", "-c",
                      f"af @ {address}; agfj @ {address}; q", str(args.retail)]
        r2 = run(r2_command, timeout=180)
        if r2.get("exit_code") == 0:
            try:
                parsed = parse_r2_graph(r2.get("stdout", ""), address)
                if not all(0x00400000 <= int(node, 16) <= 0x00600000 for node in parsed["nodes"]):
                    # A stale/short-read r2 run can report shifted block addresses; retry once.
                    r2 = run(r2_command, timeout=180)
            except (ValueError, KeyError, TypeError):
                pass
        r2_stdout = r2.get("stdout", "")
        record["radare2"] = dict(command=r2["command"], exit_code=r2.get("exit_code"),
                                 stderr=r2.get("stderr", ""),
                                 stdout_sha256=hashlib.sha256(r2_stdout.encode()).hexdigest(),
                                 stdout_prefix=r2_stdout[:2000])
        try:
            record["radare2"]["cfg"] = parse_r2_graph(r2.get("stdout", ""), address)
        except (ValueError, KeyError, TypeError) as error:
            record["radare2"]["cfg_error"] = str(error)

        if record.get("binary_ninja", {}).get("cfg") and record.get("radare2", {}).get("cfg"):
            record["cfg_cross_check"] = graph_similarity(
                record["binary_ninja"]["cfg"], record["radare2"]["cfg"])

        # angr exact-entry recovery.
        if not args.no_angr:
            angr = run([str(ANGR_PYTHON), str(ROOT / "scripts/angr_analyze.py"),
                        "--binary", str(args.retail), "--address", address], timeout=600)
            record["angr"] = dict(exit_code=angr.get("exit_code"), stderr=angr.get("stderr", ""))
            try:
                parsed = json.loads(angr.get("stdout", ""))
                fn = (parsed.get("functions") or {}).get(address, {})
                record["angr"].update(dict(
                    binary_sha256=parsed.get("binary_sha256"), mapped_base=parsed.get("mapped_base"),
                    bits=parsed.get("bits"), result=parsed.get("result"),
                    function_entry=fn.get("function_entry"), recovery_mode=fn.get("recovery_mode"),
                    has_unresolved_jumps=fn.get("has_unresolved_jumps")))
            except ValueError:
                record["angr"]["parse_error"] = angr.get("stdout", "")[:500]

        # Repository evidence artifacts and their current hashes.
        record["evidence"] = {}
        compact = address[2:].lower()
        for path in sorted(EVIDENCE.glob(f"*{compact}*.json")):
            record["evidence"].setdefault(path.name, []).append(
                dict(path=str(path.relative_to(ROOT)), sha256=sha256(path)))
        record["evidence_cfg"] = evidence_cfg(address)
        if record["evidence_cfg"].get("binary_ninja") and record.get("binary_ninja", {}).get("cfg"):
            record["evidence_vs_live_bn"] = graph_similarity(
                dict(nodes=record["evidence_cfg"]["binary_ninja"]["nodes"],
                     edges=record["evidence_cfg"]["binary_ninja"].get("edges", [])),
                record["binary_ninja"]["cfg"])
        if record["evidence_cfg"].get("radare2") and record.get("radare2", {}).get("cfg"):
            record["evidence_vs_live_r2"] = graph_similarity(
                dict(nodes=record["evidence_cfg"]["radare2"]["nodes"],
                     edges=record["evidence_cfg"]["radare2"].get("edges", [])),
                record["radare2"]["cfg"])

        # Candidate facts: symbol presence, callers and disassembly.
        names = []
        if record["manifest"] and record["manifest"].get("name"):
            names.append(record["manifest"]["name"])
        names.append(address)
        record["candidate_facts"] = candidate_facts(args.candidate, [n for n in names if not n.startswith("0x")])
        symbols = record["candidate_facts"]["symbols"]
        for name, hits in symbols.items():
            for hit in hits:
                hit["disassembly"] = candidate_disassembly(args.candidate, int(hit["address"], 16))

        out = out_dir / f"{address}-static.json"
        out.write_text(json.dumps(record, indent=2) + "\n")
        bn_blocks = len(record.get("binary_ninja", {}).get("cfg", {}).get("nodes", []))
        r2_blocks = len(record.get("radare2", {}).get("cfg", {}).get("nodes", []))
        angr_ok = (record.get("angr", {}).get("function_entry") == address
                   and record.get("angr", {}).get("has_unresolved_jumps") is False)
        symbol_ok = any(hits for hits in symbols.values())
        print(f"{address}: bn_blocks={bn_blocks} r2_blocks={r2_blocks} angr_exact={angr_ok} "
              f"candidate_symbol={symbol_ok} -> {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
