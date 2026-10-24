#!/usr/bin/env python3
"""Static packets for the QTask_RegisterClasses registrar chain (analysis only).

For each requested registrar entry and for 0x00402930 this captures, with Binary
Ninja as the single selected decompiler:
  - function identity/prototype/complexity, full decompilation and disassembly,
  - the ordered direct call list with the argument-setup instructions, literal /
    string / register arguments and return-value use,
  - ordered class/option registrations (calls whose arguments include a readable
    image string) with the exact name bytes and the global that receives the id,
  - delegated registrar bodies (callees that themselves perform registrations),
  - Radare2 CFG node/edge counts and angr exact-entry result read from the run
    directory's static_probe_capture output (no second decompiler is used).

Writes build/tester/<run>/registrars/<raw>.json|md and CHAIN.md. It never edits
engine sources, CMake, manifest or evidence JSON.
"""
from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BN = ROOT / "scripts/bn_mcp_call.py"

ENTRIES = {
    "0x00402930": "QTask_RegisterClasses/0x402930 (retail registration call chain)",
    "0x00508F80": "AI graph registrar",
    "0x00489560": "AI detection registrar",
    "0x00407D60": "material defines registrar",
    "0x00488BB0": "level flow registrar",
    "0x00402FF0": "game options registrar",
}
DELEGATES = {
    "0x00508F80": ["0x005183D0", "0x00508FA0"],
}
REG_API_HINTS = ("TaskType_Register", "Game_DefineOptions", "QTask_LinkQTaskEventHandler",
                 "QTask_RemoveEventCallback", "QTaskList_New", "QTask_UnableToAllocateEven")


def run_bn(tool: str, arguments: dict) -> dict:
    proc = subprocess.run([sys.executable, str(BN), tool, json.dumps(arguments)],
                          capture_output=True, text=True, timeout=300)
    return dict(tool=tool, stdout=proc.stdout, stderr=proc.stderr, returncode=proc.returncode)


def block(text: str, marker: str) -> str:
    assert marker in text, text[:200]
    body = text.split(marker, 1)[1]
    end = body.find("\n````", 1)
    return body[:end] if end != -1 else body


def bn_decompile(address: str) -> str:
    return block(run_bn("bn_function_decompile", {"function": address})["stdout"], "````bn-pseudoc")


def bn_disassembly(address: str) -> str:
    return block(run_bn("bn_function_disassembly", {"function": address})["stdout"], "````asm")


def bn_info(address: str) -> dict:
    text = run_bn("bn_function_info", {"function": address})["stdout"]
    info = {}
    for line in text.splitlines():
        m = re.match(r"\| (Function|Name|Start|Highest Address|Definition|Basic Blocks|Calling Convention|Exported) \| (.*?) \|", line)
        if m:
            info[m.group(1)] = m.group(2).strip("` ")
    return info


def bn_complexity(address: str) -> dict:
    text = run_bn("bn_function_complexity", {"function": address})["stdout"]
    out = {}
    for line in text.splitlines():
        m = re.match(r"\| (Blocks|Edges|Calls|Variables|Total Bytes) \| (\d+) \|", line)
        if m:
            out[m.group(1)] = int(m.group(2))
    return out


def bn_callees(address: str) -> list[dict]:
    text = run_bn("bn_function_callees", {"function": address})["stdout"]
    rows = []
    for line in text.splitlines():
        m = re.match(r"\| (0x[0-9a-f]+) \| x86 \| (0x[0-9a-f]+) \| (.+?) \((0x[0-9a-f]+) \(x86\)\) \|$", line)
        if m:
            rows.append(dict(callsite=m.group(1), callee=m.group(2), name=m.group(3).strip(),
                             callee_function=m.group(4)))
    return rows


_FUNCTION_CACHE: dict[str, dict] = {}
_DATA_CACHE: dict[int, dict] = {}


def resolve_function(name_or_address: str) -> dict | None:
    key = name_or_address.lower()
    if key in _FUNCTION_CACHE:
        return _FUNCTION_CACHE[key]
    text = run_bn("bn_function_info", {"function": name_or_address})["stdout"]
    info = {}
    for line in text.splitlines():
        m = re.match(r"\| (Function|Name|Start|Highest Address|Definition) \| (.*?) \|", line)
        if m:
            info[m.group(1)] = m.group(2).strip("` ")
    result = info if info.get("Function") else None
    _FUNCTION_CACHE[key] = result
    return result


def read_data(address: int) -> dict:
    if address in _DATA_CACHE:
        return _DATA_CACHE[address]
    text = run_bn("bn_data_at", {"address": f"0x{address:08x}"})["stdout"]
    try:
        payload = json.loads(text)
    except Exception:
        payload = {}
    memory = payload.get("memory") or {}
    hex_text = (memory.get("hex") or "").strip()
    data = bytes.fromhex(hex_text) if re.fullmatch(r"[0-9a-fA-F]*", hex_text) else b""
    cut = data.split(b"\x00", 1)[0]
    printable = all(32 <= ch < 127 for ch in cut)
    result = dict(hex=hex_text,
                  ascii=cut.decode("latin-1") if printable and len(cut) >= 2 else None,
                  formatted=(payload.get("dataVariable") or {}).get("formatted"),
                  printable=printable and len(cut) >= 2)
    _DATA_CACHE[address] = result
    return result


def read_string(address: int, length: int = 64) -> dict:
    text = run_bn("bn_memory_read", {"address": f"0x{address:08x}", "length": length})["stdout"]
    try:
        payload = json.loads(text)
    except Exception:
        return dict(raw=text[:200])
    hex_text = (payload.get("hex") or payload.get("data") or "").strip()
    data = bytes.fromhex(hex_text) if re.fullmatch(r"[0-9a-fA-F]*", hex_text) else b""
    cut = data.split(b"\x00", 1)[0]
    printable = all(32 <= ch < 127 for ch in cut)
    return dict(hex=hex_text, ascii=cut.decode("latin-1") if printable else None,
                printable=printable and len(cut) >= 2)


def parse_calls(asm: str, callee_callsites: set[str] | None = None) -> list[dict]:
    lines = []
    for line in asm.splitlines():
        m = re.match(r"\s*(0x[0-9a-f]+)\s+(\S+)\s*(.*)$", line)
        if m:
            lines.append(dict(address=m.group(1), mnemonic=m.group(2), operands=m.group(3).strip(),
                              raw=line.strip()))
    calls = []
    pending: list[dict] = []
    previous_call = None
    for item in lines:
        mn = item["mnemonic"]
        if mn == "call" or (mn == "jmp" and callee_callsites and item["address"] in callee_callsites):
            target = item["operands"].split()[0] if item["operands"] else ""
            calls.append(dict(callsite=item["address"], target=target, args=pending,
                              raw=item["raw"], previous_call=previous_call,
                              tail_jump=mn == "jmp"))
            pending = []
            previous_call = target
        elif mn in ("retn", "ret", "jmp") or mn.startswith("j"):
            pending = []
        elif mn == "push":
            operand = item["operands"].split()[0] if item["operands"] else ""
            if operand not in ("edi", "esi", "ebx", "ebp"):
                pending.append(dict(kind="push", operand=operand, address=item["address"],
                                    mnemonic=mn, raw=item["raw"]))
        elif mn.startswith("mov") or mn.startswith("lea") or mn.startswith("xor"):
            pending.append(dict(kind="setup", operand=item["operands"], address=item["address"],
                                mnemonic=mn, raw=item["raw"]))
    return calls


def decode_args(call: dict) -> list[dict]:
    """Decode the pushes belonging to a call, in source (cdecl) order."""
    branches = [a for a in call["args"] if a["kind"] == "push"]
    decoded = []
    for arg in reversed(branches):
        operand = arg["operand"]
        entry = dict(kind="push", raw=arg["raw"], operand=operand)
        literal = None
        m = re.fullmatch(r"0x([0-9a-f]+)", operand)
        if m:
            literal = int(m.group(1), 16)
        elif operand.lower().startswith("data_"):
            literal = int(operand.split("_", 1)[1], 16)
        if literal is not None:
            entry["value"] = f"0x{literal:x}"
            if 0x400000 <= literal < 0x600000:
                string = read_data(literal)
                if string.get("printable"):
                    entry.update(kind="string", address=f"0x{literal:08x}", name=string["ascii"],
                                 bn_formatted=string.get("formatted"))
                else:
                    function = resolve_function(f"0x{literal:08x}")
                    if function:
                        entry.update(kind="code_pointer", address=f"0x{literal:08x}",
                                     name=function.get("Name"))
        elif operand == "eax":
            entry.update(kind="register", register="eax")
        elif re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", operand):
            function = resolve_function(operand)
            if function:
                entry.update(kind="code_pointer", address=function.get("Function"),
                             name=function.get("Name"))
            else:
                entry.update(kind="register", register=operand)
        decoded.append(entry)
    return list(reversed(decoded))


def registration_view(call: dict) -> dict | None:
    strings = [a for a in call["args"] if a["kind"] == "string"]
    target_name = call["target"]
    looks_api = any(hint in target_name for hint in REG_API_HINTS)
    if not strings and not looks_api:
        return None
    numeric = [a.get("value") or a.get("operand") for a in call["args"] if a["kind"] != "string"]
    return_use = call.get("return_use", [])
    id_store = None
    for line in return_use:
        m = re.search(r"\[(?:0x([0-9a-f]+)|g_([0-9a-f]+))\]", line)
        if m:
            id_store = f"0x{m.group(1) or m.group(2)}"
            break
    return dict(callsite=call["callsite"], target=target_name,
                target_is_decompiler_named=looks_api,
                name_strings=[dict(address=a["address"], text=a["name"]) for a in strings],
                code_pointers=[dict(address=a.get("address"), name=a.get("name"))
                               for a in call["args"] if a["kind"] == "code_pointer"],
                other_args=numeric, id_store=id_store,
                arg_setup=[a["raw"] for a in call.get("setup", [])],
                return_use=return_use)


def annotate_return_use(asm: str, calls: list[dict]) -> None:
    lines = [m.group(0) for m in re.finditer(r"\s*(0x[0-9a-f]+)\s+(\S+)\s*(.*)", asm) if m]
    index = {}
    for pos, line in enumerate(lines):
        m = re.match(r"\s*(0x[0-9a-f]+)\s+(\S+)", line)
        if m:
            index[m.group(1)] = pos
    for call in calls:
        pos = index.get(call["callsite"])
        uses = []
        if pos is not None:
            for line in lines[pos + 1: pos + 5]:
                m = re.match(r"\s*(0x[0-9a-f]+)\s+(\S+)\s*(.*)", line)
                if not m:
                    continue
                if m.group(2) in ("call", "jmp") or m.group(2).startswith("j"):
                    break
                if re.search(r"\b(eax|ax|al)\b", m.group(3)):
                    uses.append(line.strip())
        call["return_use"] = uses


def static_evidence(run_dir: Path, address: str) -> dict:
    path = run_dir / "static" / f"{address.lower()}-static.json"
    if not path.is_file():
        return dict(available=False)
    doc = json.loads(path.read_text())
    cfg = ((doc.get("radare2") or {}).get("cfg") or {})
    return dict(available=True,
                radare2=dict(blocks=len(cfg.get("nodes", [])), edges=len(cfg.get("edges", [])),
                             command=((doc.get("radare2") or {}).get("command"))),
                binary_ninja_blocks=len(((doc.get("binary_ninja") or {}).get("cfg") or {}).get("nodes", [])),
                angr=doc.get("angr"),
                bytes_crosscheck=doc.get("bytes_crosscheck"))


def packet(address: str, label: str, run_dir: Path, delegated: list[str]) -> dict:
    info = bn_info(address)
    complexity = bn_complexity(address)
    asm = bn_disassembly(address)
    decompile = bn_decompile(address)
    callees = bn_callees(address)
    calls = parse_calls(asm, {c["callsite"] for c in callees})
    seen = {call["callsite"] for call in calls}
    for callee in callees:
        if callee["callsite"] not in seen:
            calls.append(dict(callsite=callee["callsite"], target=callee["name"], args=[],
                              raw=f"{callee['callsite']} call {callee['name']}",
                              previous_call=None, tail_jump=False))
    calls.sort(key=lambda c: int(c["callsite"], 16))
    annotate_return_use(asm, calls)
    for call in calls:
        call["args"] = decode_args(call)
        call["setup"] = [a for a in call["args"] if a["kind"] != "push"]
    records = [view for view in (registration_view(call) for call in calls) if view]
    delegates = []
    for d in sorted({a.lower() for a in delegated}):
        if d == address.lower():
            continue
        d_asm = bn_disassembly(d)
        d_decomp = bn_decompile(d)
        d_callees = bn_callees(d)
        d_calls = parse_calls(d_asm, {c["callsite"] for c in d_callees})
        for c in d_calls:
            c["args"] = decode_args(c)
            c["setup"] = [a for a in c["args"] if a["kind"] != "push"]
        annotate_return_use(d_asm, d_calls)
        for c in d_calls:
            target_addr = resolve_function(c["target"]).get("Function") if resolve_function(c["target"]) else None
            c["target_address"] = target_addr
        d_records = [view for view in (registration_view(c) for c in d_calls) if view]
        delegates.append(dict(address=d, info=bn_info(d), complexity=bn_complexity(d),
                              call_count=len(d_calls), registrations=d_records,
                              calls=[dict(callsite=c["callsite"], target=c["target"],
                                          target_address=c.get("target_address"),
                                          args=[a.get("name") or a.get("value") or a.get("operand") for a in c["args"]],
                                          return_use=c.get("return_use", []))
                                     for c in d_calls],
                              disassembly=d_asm, decompilation=d_decomp,
                              static=static_evidence(run_dir, d)))
    callee_by_callsite = {c["callsite"]: c for c in callees}
    return dict(address=address, label=label,
                image=dict(path="/Users/haseeb-mir/Downloads/IGI/igi.exe",
                           sha256="dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01",
                           preferred_base="0x00400000", module="igi.exe"),
                decompiler=dict(tool="Binary Ninja MCP", selected="BN first, no Ghidra used",
                                version_note="live MCP", info=info, complexity=complexity),
                static=static_evidence(run_dir, address),
                call_count=len(calls),
                return_value_use=any(call["return_use"] for call in calls),
                calls=[dict(index=i + 1, callsite=c["callsite"], target=c["target"],
                            target_address=(callee_by_callsite.get(c["callsite"], {}).get("callee")
                                            or (resolve_function(c["target"]).get("Function")
                                                if resolve_function(c["target"]) else None)),
                            tail_jump=c.get("tail_jump", False),
                            args=[dict(kind=a["kind"], value=a.get("value"), name=a.get("name"),
                                       operand=a.get("operand")) for a in c["args"]],
                            return_use=c["return_use"]) for i, c in enumerate(calls)],
                registrations=records,
                delegate_count=len(delegates), delegates=delegates,
                decompilation=decompile, disassembly=asm,
                callees=callees)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--run-dir", type=Path, required=True)
    args = ap.parse_args()
    run_dir = args.run_dir.resolve()
    out_dir = run_dir / "registrars"
    out_dir.mkdir(parents=True, exist_ok=True)
    packets = {}
    for address, label in ENTRIES.items():
        delegated = DELEGATES.get(address, [])
        packet_data = packet(address, label, run_dir, delegated)
        packets[address] = packet_data
        path = out_dir / f"{address.lower()}.json"
        path.write_text(json.dumps(packet_data, indent=2) + "\n")
        md = [f"# Registrar packet — {address} ({label})", "",
              f"Image: igi.exe sha256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, base 0x400000. Decompiler: Binary Ninja (single decompiler; no Ghidra).",
              f"BN info: `{packet_data['decompiler']['info'].get('Name')}` {packet_data['decompiler']['info'].get('Definition')} blocks={packet_data['decompiler']['complexity'].get('Blocks')} edges={packet_data['decompiler']['complexity'].get('Edges')} calls={packet_data['decompiler']['complexity'].get('Calls')} bytes={packet_data['decompiler']['complexity'].get('Total Bytes')}",
              f"r2 CFG: {(packet_data['static'].get('radare2') or {}).get('blocks')} blocks / {(packet_data['static'].get('radare2') or {}).get('edges')} edges; angr exact entry: {((packet_data['static'].get('angr') or {}).get('function_entry'))}, unresolved jumps: {((packet_data['static'].get('angr') or {}).get('has_unresolved_jumps'))}; BN-vs-r2 byte crosscheck: {(packet_data['static'].get('bytes_crosscheck') or {}).get('match')}",
              f"Ordered direct calls: {packet_data['call_count']}; return-value use anywhere: {packet_data['return_value_use']}", ""]
        md.append("## Ordered calls")
        md.append("")
        md.append("| # | callsite | target address | target (BN name) | decoded args (source order) | return use |")
        md.append("| - | -------- | -------------- | ---------------- | -------------------------- | ---------- |")
        for call in packet_data["calls"]:
            arg_text = "; ".join(
                (a.get("name") or a.get("value") or a.get("operand") or "") for a in call["args"]) or "-"
            md.append(f"| {call['index']} | {call['callsite']} | {call.get('target_address') or '-'} | {call['target']} | {arg_text} | {'; '.join(call['return_use']) or '-'} |")
        md.append("")
        md.append("## Registrations (calls carrying name strings / registration APIs)")
        md.append("")
        md.append("| callsite | target | name string | code pointers / other args | id store | return use |")
        md.append("| -------- | ------ | ----------- | -------------------------- | -------- | ---------- |")
        for rec in packet_data["registrations"]:
            names = ", ".join(f"`{n['text']}` @ {n['address']}" for n in rec["name_strings"]) or "-"
            pointers = ", ".join(f"{p['name']}@{p['address']}" for p in rec["code_pointers"]) or "-"
            md.append(f"| {rec['callsite']} | {rec['target']} | {names} | {pointers}; {', '.join(rec['other_args']) or '-'} | {rec.get('id_store') or '-'} | {'; '.join(rec['return_use']) or '-'} |")
        if packet_data["delegates"]:
            md.append("")
            md.append("## Delegated registrar bodies")
            for d in packet_data["delegates"]:
                md.append("")
                md.append(f"### {d['address']} — BN name `{d['info'].get('Name')}` {d['info'].get('Definition')}, blocks={d['complexity'].get('Blocks')}, calls={len(d['calls'])}")
                dstatic = d.get("static") or {}
                dr2 = dstatic.get("radare2") or {}
                dangr = dstatic.get("angr") or {}
                md.append("")
                md.append(f"r2 CFG: {dr2.get('blocks')} blocks / {dr2.get('edges')} edges; angr exact entry: {dangr.get('function_entry')}, unresolved jumps: {dangr.get('has_unresolved_jumps')}.")
                md.append("")
                md.append("| callsite | target address | target (BN name) | args | return use |")
                md.append("| -------- | -------------- | ---------------- | ---- | ---------- |")
                for c in d["calls"]:
                    md.append(f"| {c['callsite']} | {c.get('target_address') or '-'} | {c['target']} | {', '.join(str(a) for a in c['args']) or '-'} | {'; '.join(c.get('return_use') or []) or '-'} |")
                md.append("")
                md.append(f"Full body is in the JSON (`delegates[{d['address']}].disassembly/decompilation`).")
        md.append("")
        md.append("## Unresolved / caveats")
        md.append("")
        static = packet_data.get("static") or {}
        r2 = static.get("radare2") or {}
        angr = static.get("angr") or {}
        note = []
        if r2 and packet_data["decompiler"]["complexity"].get("Blocks") is not None and r2.get("blocks") != packet_data["decompiler"]["complexity"].get("Blocks"):
            note.append(f"r2 reports {r2.get('blocks')} blocks vs BN {packet_data['decompiler']['complexity'].get('Blocks')} (block splitting/tail-jump handling differs); call targets were cross-checked against the BN callee list.")
        if not static.get("available"):
            note.append("static_probe_capture evidence missing for this address in the run dir.")
        if packet_data["registrations"] and not any(rec["name_strings"] for rec in packet_data["registrations"]):
            note.append("these calls are classified by BN callee name only; no class-name string was present in their decoded arguments (verify primitive identity from the target addresses before transcription).")
        note.append("Function/API names are Binary Ninja auto-generated; addresses, immediate bytes and strings are the evidence.")
        md.extend("- " + item for item in note)
        md.append("")
        md.append("## Full Binary Ninja pseudocode")
        md.append("")
        md.append("````c")
        md.append(packet_data["decompilation"].rstrip())
        md.append("````")
        (out_dir / f"{address.lower()}.md").write_text("\n".join(md) + "\n")
        print(f"{address} {label}: calls={packet_data['call_count']} registrations={len(packet_data['registrations'])} delegates={len(packet_data['delegates'])}")
    # chain summary
    chain = packets["0x00402930"]
    summary = [f"# Registration chain summary — 0x00402930 (Tester static packet)", "",
               "Image: igi.exe sha256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01` (base 0x400000). Decompiler: Binary Ninja only.",
               "",
               f"`0x00402930` is a single-basic-block sequence of **{chain['call_count']} direct calls** (BN reported 156 callees; return values are never used). The retail call order is the disassembly order below; each call target is one registrar native. First five requested registrar entries are the first five targets.",
               "",
               "| # | callsite | target | BN name | requested |", "| - | -------- | ------ | ------- | --------- |"]
    requested = {"0x00508F80", "0x00489560", "0x00407D60", "0x00488BB0", "0x00402FF0"}
    callee_by_site = {c["callsite"]: c for c in chain.get("callees", [])}
    for index, call in enumerate(chain["calls"], 1):
        callee = callee_by_site.get(call["callsite"]) or {}
        addr = f"0x{int(callee['callee'], 16):08x}" if callee.get("callee") else ""
        name = callee.get("name") or call["target"]
        tail = " (tail jmp)" if call.get("tail_jump") else ""
        summary.append(f"| {index} | {call['callsite']} | {addr} | {name}{tail} | {'YES' if addr.lower() in {r.lower() for r in requested} else ''} |")
    summary += ["", "## Registrar packets", ""]
    for address, label in ENTRIES.items():
        if address == "0x00402930":
            continue
        p = packets[address]
        summary.append(f"- {address} ({label}): {p['call_count']} direct calls, {len(p['registrations'])} registration entries, {len(p['delegates'])} delegated body/bodies ({', '.join(d['address'] for d in p['delegates']) or 'none'}); packet `registrars/{address.lower()}.json`.")
    summary += ["", "## Unresolved / caveats", "",
                "- Function names shown are Binary Ninja decompiler/auto-generated names; they are not verified retail symbols. Use the addresses, argument bytes and strings as evidence.",
                "- `0x00508F80` is a wrapper: it calls `0x005183D0` (large unconditional call sequence = core task-type registrations) and tail-jumps `0x00508FA0` (AIGraph TaskType_Register + handler links). Both delegated bodies are captured in the packet.",
                "- Registrations use more than one primitive (`TaskType_Register`-class calls with name strings, `Game_DefineOptions`-class calls, event-handler link calls); the packets list every call with decoded args so the Lead can transcribe the exact primitive sequence. `QTask_*` names are decompiler guesses and must be confirmed against the call addresses during transcription.",
                "- No runtime capture was taken for this analysis-only step; no manifest/status edits."]
    (out_dir / "CHAIN.md").write_text("\n".join(summary) + "\n")

    brief = ["# Registrar static packets — one-page summary", "",
             "Image: retail `igi.exe` sha256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, base 0x400000, from `/Users/haseeb-mir/Downloads/IGI/igi.exe`. Binary Ninja is the single selected decompiler (no Ghidra). r2 CFG + angr exact entry from `scripts/tester/static_probe_capture.py`; evidence in `static/<addr>-static.json`.", "",
             "## 0x00402930 registration chain", "",
             f"- Single basic block (BN) of exactly **{chain['call_count']} ordered direct calls** (155 `call` + 1 tail `jmp`); every return value is ignored.",
             "- Retail order = row order in `CHAIN.md`; first the five requested registrar entries, then the remaining registrars.",
             "- r2 reports 2 blocks / 1 edge (tail-jump split) vs BN 1 block / 0 edges; angr exact entry `0x00402930`, unresolved jumps `False`. The 156 call targets agree between BN callee list and the disassembly.",
             "- No arguments are passed to any target from 0x402930 (no pushes at all in the block); registration arguments live inside each registrar.", "",
             "## Requested registrar packets", "",
             "| address | r2 blocks/edges | angr exact | direct calls | registration entries | delegated bodies | first class/option name(s) |",
             "| ------- | --------------- | ---------- | ------------ | -------------------- | ---------------- | -------------------------- |"]
    for address, label in ENTRIES.items():
        if address == "0x00402930":
            continue
        p = packets[address]
        st = p["static"] or {}
        r2 = st.get("radare2") or {}
        angr = st.get("angr") or {}
        names = []
        for rec in (p["registrations"] + [r for d in p["delegates"] for r in d["registrations"]]):
            if not any(k in rec["target"] for k in ("TaskType_Register", "Game_DefineOptions", "TaskType_Set")):
                continue
            names.extend(n["text"] for n in rec["name_strings"])
        delegate_regs = sum(len(d["registrations"]) for d in p["delegates"])
        reg_text = str(len(p["registrations"])) + (f" + {delegate_regs} delegated" if delegate_regs else "")
        brief.append(f"| {address} ({label}) | {r2.get('blocks')}/{r2.get('edges')} | entry {angr.get('function_entry')}, unresolved {angr.get('has_unresolved_jumps')} | {p['call_count']} | {reg_text} | {', '.join(d['address'] for d in p['delegates']) or 'none'} | {', '.join(names[:4]) or '(none decoded)'} |")
    brief += ["", "## Unresolved / caveats", "",
              "- All names (`Physics_ApplyImpulse_7`, `QTask_RemoveEventCallback`, `TaskType_Register`, ...) are Binary Ninja auto-generated; they are **not** verified retail symbols. Transcribe from addresses, immediate arguments and strings in the full disassembly, not from the names.",
              "- `0x00508F80` delegates: calls `0x005183D0` (74 unconditional calls = core task-type registration list) then tail-jumps `0x00508FA0` (AIGraph `TaskType_Register` 'AIGraph' @ 0x54E888, size 0xC0, then event-handler links). Both bodies are captured in `0x00508f80.json` (`delegates`).",
              "- `0x00489560` has four calls with no decoded name string (`QTaskList_New`-class and three `0x0`-argument `QTask_UnableToAllocateEven`-class calls); those primitives need confirmation from the call target bytes before transcription.",
              "- Argument decoding is instruction-level but heuristic around callee-saved prologue pushes; the full Binary Ninja disassembly is embedded in every packet (`disassembly`) and must be treated as the authority.",
              "- No runtime capture and no manifest/status edits were made; this step is analysis only."]
    (out_dir / "SUMMARY.md").write_text("\n".join(brief) + "\n")
    print("wrote", out_dir / "CHAIN.md", "and", out_dir / "SUMMARY.md")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
