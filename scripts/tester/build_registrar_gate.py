#!/usr/bin/env python3
"""Build the registrar-chain runtime gate scaffolding (analysis only, no launch).

Inputs are the static registrar packets from the QTask registration-chain step
(``build/tester/<run>/registrars/*.json``) and the per-callback static captures.
For every registrar / delegated body / nested callback this writes:

  probes/probe_<addr>.js          typed Frida probe (frida_registrar_chain_probe.js
                                  with a JSON config substituted; range-checked
                                  side effects, packet-proven string addresses)
  contracts/<addr>.json           runtime contract: what retail must show, what
                                  the candidate must reproduce, reachability
  contracts/CONTRACTS.md          one-page contract table
  BASELINE.md                     static retail baseline + prior runtime evidence
  README.md                       sequential retail/candidate capture commands
  inventory.json                  probe + contract + static evidence inventory

No game is launched and no manifest/status is edited here.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TEMPLATE = ROOT / "scripts/tester/frida_registrar_chain_probe.js"
RETAIL_SHA = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
REGISTRARS = {
    "0x00402930": dict(kind="chain", position=0, label="QTask_RegisterClasses registration chain"),
    "0x00508F80": dict(kind="registrar", position=1, label="AI graph registrar (wrapper)"),
    "0x00489560": dict(kind="registrar", position=2, label="AI detection registrar"),
    "0x00407D60": dict(kind="registrar", position=3, label="material defines registrar"),
    "0x00488BB0": dict(kind="registrar", position=4, label="level flow registrar"),
    "0x00402FF0": dict(kind="registrar", position=5, label="game options registrar"),
}
DELEGATES = {
    "0x00508F80": ["0x005183D0", "0x00508FA0"],
}
HANDLERS = {
    "0x00407DE0": dict(label="SetActiveMaterial option handler", profile="option"),
    "0x00407E30": dict(label="DefineQMaterial option handler", profile="option"),
    "0x00407F50": dict(label="DefineGameMaterial option handler", profile="option"),
    "0x00488ED0": dict(label="LevelFlow state/check callback (slot 0)", profile="levelflow"),
    "0x00489200": dict(label="LevelFlow event-2 callback", profile="levelflow"),
    "0x00489290": dict(label="LevelFlow load-parameters event-1 callback", profile="levelflow"),
    "0x004D3710": dict(label="AIGraph LoadParameters callback (pure return)", profile="aigraph"),
    "0x00509610": dict(label="AIGraph Setup callback", profile="aigraph"),
    "0x005093B0": dict(label="AIGraph cleanup/tail-call callback", profile="aigraph"),
    "0x00509570": dict(label="AIGraph load-parameters callback", profile="aigraph"),
    "0x00509520": dict(label="AIGraph process callback", profile="aigraph"),
}
# Side effects proven by the static packets/BN decompiles. Values are compared
# retail-vs-candidate at runtime; no static expected value is invented.
EXTRA_SIDE_EFFECTS = {
    "0x00407D60": [("material_array_clear_0x569474", "0x00569474", 0x4D00),
                   ("al_0x538518", "0x00538518", 1), ("al_0x538519", "0x00538519", 1),
                   ("al_0x53851a", "0x0053851A", 1)],
    "0x00489560": [("qtasklist_global_0x5c8ac0", "0x005C8AC0", 4),
                   ("qtask_globals_0x5c8b0c", "0x005C8B0C", 12)],
    "0x00488BB0": [("levelflow_id_0x5c8ab8", "0x005C8AB8", 2)],
    "0x00508FA0": [("aigraph_type_id_0x54e83c", "0x0054E83C", 2)],
}
HANDLER_SIDE_EFFECTS = {
    "0x00407DE0": [("active_material_index_0x53851c", "0x0053851C", 4)],
    "0x00407E30": [("active_material_index_0x53851c", "0x0053851C", 4)],
    "0x00407F50": [("active_material_index_0x53851c", "0x0053851C", 4)],
    "0x00488ED0": [("levelflow_current_0xbc2360", "0x00BC2360", 4)],
    "0x00489200": [("levelflow_current_0xbc2360", "0x00BC2360", 4)],
    "0x00489290": [("levelflow_list_count_0xafa7e0", "0x00AFA7E0", 4)],
}
REG_API_HINTS = ("TaskType_", "Game_DefineOptions", "QTask_Link", "QTask_Remove",
                 "QTaskList_", "QTask_Unable")
HANDLER_SCENARIO = {
    "option": "retail `window level1`, observe <=60 s; option callbacks fire only if the level/config script evaluates the registered options",
    "levelflow": "retail `window level1`, observe <=60 s; LevelFlow callbacks fire only when the LevelFlow task is created and its events run",
    "aigraph": "AI graph callbacks are outside the current MVP scope; expect UNOBSERVED and never treat that as a pass",
}


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def load_json(path: Path) -> dict:
    return json.loads(path.read_text())


def addr_only(value: str | None) -> str | None:
    """Normalize BN addresses such as '0x401900 (x86)' to '0x401900'."""
    if not value:
        return None
    match = re.search(r"0x[0-9a-fA-F]+", value)
    return match.group(0).lower() if match else None


def bn_name(info_field: dict | None) -> str | None:
    if not info_field:
        return None
    match = re.search(r'name:\s*"([^"]+)"', info_field.get("stdout", ""))
    return match.group(1) if match else None


def registrations_of(packet: dict) -> list[dict]:
    # Registration records in the packet may omit the API target address; the
    # ordered calls list carries it per callsite, so join the two before use.
    callsite_target = {addr_only(call.get("callsite")): addr_only(call.get("target_address"))
                       for call in packet.get("calls") or []}
    rows = []
    for row in list(packet.get("registrations") or []):
        row = dict(row)
        if not row.get("target_address"):
            row["target_address"] = callsite_target.get(addr_only(row.get("callsite")))
        rows.append(row)
    for delegate in packet.get("delegates") or []:
        delegate_calls = {addr_only(call.get("callsite")): addr_only(call.get("target_address"))
                          for call in delegate.get("calls") or []}
        for row in delegate.get("registrations") or []:
            row = dict(row)
            if not row.get("target_address"):
                row["target_address"] = delegate_calls.get(addr_only(row.get("callsite")))
            rows.append(row)
    return rows


def calls_by_callsite(packet: dict) -> dict[str, dict]:
    return {call["callsite"].lower(): call for call in packet.get("calls") or []}


def is_registration_api(name: str) -> bool:
    return any(hint in name for hint in REG_API_HINTS)


def width_from_return_use(return_use: list[str], default: int = 4) -> int:
    for line in return_use:
        m = re.search(r"\bmov\s+(byte|word|dword)\s+\[", line)
        if m:
            return {"byte": 1, "word": 2, "dword": 4}[m.group(1)]
    return default


def chunk_effects(rows: list[tuple[str, str, int]]) -> list[dict]:
    effects = []
    for label, va, length in rows:
        base = int(va, 16)
        offset = 0
        while offset < length:
            piece = min(4096, length - offset)
            effects.append(dict(label=label if length <= 4096 else f"{label}_part{offset // 4096}",
                                va=f"0x{base + offset:08X}", length=piece))
            offset += piece
    return effects


def derive_side_effects(packet: dict, address: str) -> list[dict]:
    rows: list[tuple[str, str, int]] = []
    for rec in registrations_of(packet):
        va = rec.get("id_store")
        if not va:
            continue
        width = width_from_return_use(rec.get("return_use") or [])
        rows.append((f"id_store_{va}", va, width))
    rows.extend(EXTRA_SIDE_EFFECTS.get(address, []))
    rows.extend(HANDLER_SIDE_EFFECTS.get(address, []))
    seen: dict[int, tuple[str, int]] = {}
    for label, va, width in rows:
        value = int(va, 16)
        # One block per address; the widest static observation wins so word and
        # dword writes to the same id slot are both covered.
        if value not in seen or width > seen[value][1]:
            seen[value] = (f"0x{value:08X}", width)
    merged = [(f"global_{va}", va, width) for (va, width) in seen.values()]
    return chunk_effects(merged)


def derive_api_hooks(packet: dict) -> tuple[list[dict], list[str]]:
    grouped: dict[str, dict] = {}
    proven: set[str] = set()
    for rec in registrations_of(packet):
        name = rec.get("target") or ""
        if not is_registration_api(name):
            continue
        va = addr_only(rec.get("target_address"))
        if not va:
            continue
        entry = grouped.setdefault(va, dict(va=va, name=name, argWords=4, expectedNames=[]))
        for string in rec.get("name_strings") or []:
            entry["expectedNames"].append(string["address"])
            proven.add(string["address"])
        call_rows = packet.get("calls") or []
        for call in call_rows:
            if call.get("callsite") == rec.get("callsite"):
                entry["argWords"] = max(entry["argWords"], len(call.get("args") or []) + 1)
    return sorted(grouped.values(), key=lambda item: item["va"]), sorted(proven)


def expected_registrations(packet: dict) -> list[dict]:
    ordered = []
    for rec in registrations_of(packet):
        if not is_registration_api(rec.get("target") or ""):
            continue
        if not rec.get("target_address"):
            continue
        ordered.append(dict(
            api_va=addr_only(rec["target_address"]),
            api_name=rec["target"],
            name_strings=[dict(address=s["address"], text=s["text"]) for s in rec.get("name_strings") or []],
            id_store=rec.get("id_store"),
            other_args=rec.get("other_args") or [],
            callsite=rec.get("callsite"),
        ))
    return ordered


def chain_entries(packet: dict) -> list[dict]:
    entries = []
    callees = {c["callsite"]: c for c in packet.get("callees") or []}
    for index, call in enumerate(packet.get("calls") or [], 1):
        callee = callees.get(call["callsite"]) or {}
        target = addr_only(call.get("target_address") or callee.get("callee"))
        if not target:
            continue
        tail = bool(call.get("tail_jump"))
        callsite = call["callsite"]
        entries.append(dict(order=index, callsite=callsite, target=target,
                            label=callee.get("name") or call.get("target"),
                            tail_jump=tail,
                            return_address=None if tail else f"0x{int(callsite, 16) + 5:08x}"))
    return entries


def build():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--packets-dir", type=Path, required=True,
                    help="build/tester/<run>/registrars from the static packet step")
    ap.add_argument("--static-dir", type=Path, required=True,
                    help="handler static_probe_capture output directory")
    ap.add_argument("--run-dir", type=Path, required=True,
                    help="build/tester/<run>/registrar-gates target directory")
    ap.add_argument("--observe-seconds", type=int, default=30)
    ap.add_argument("--candidate-sha", default="6d39aa53216f2d0db4edb50afac1e217fa7dd8b5881f3f073404ba4621973e2e",
                    help="Lead-frozen candidate sha the probes/reports must bind to")
    args = ap.parse_args()

    args.packets_dir = args.packets_dir.resolve()
    args.static_dir = args.static_dir.resolve()
    run_dir = args.run_dir.resolve()
    limitations_path = run_dir / "chain-limitations.json"
    limitations = json.loads(limitations_path.read_text()) if limitations_path.exists() else {}
    probes_dir = run_dir / "probes"
    contracts_dir = run_dir / "contracts"
    reports_dir = run_dir / "reports"
    binary_static_dir = run_dir / "static"
    for path in (probes_dir, contracts_dir, reports_dir, binary_static_dir):
        path.mkdir(parents=True, exist_ok=True)

    packets = {}
    for address in REGISTRARS:
        path = args.packets_dir / f"{address.lower()}.json"
        packets[address] = load_json(path)

    # All 156 registration positions (also used for per-registrar expected caller).
    chain = chain_entries(packets["0x00402930"])
    chain_by_target = {entry["target"].lower(): entry for entry in chain}
    delegate_by_address = {}
    for parent in DELEGATES:
        for delegate in packets[parent].get("delegates") or []:
            delegate_by_address[delegate["address"].lower()] = (parent, delegate)

    static_handlers = {}
    for address in HANDLERS:
        path = args.static_dir / f"{address.lower()}-static.json"
        static_handlers[address] = load_json(path)

    inventory = dict(kind="igi-tester-registrar-gate", schema_version=1,
                     retail=dict(path="/Users/haseeb-mir/Downloads/IGI/igi.exe", sha256=RETAIL_SHA,
                                 preferred_base="0x00400000"),
                     image_contract="retail VAs are 0x400000-based; probes resolve them against the "
                                    "observed main module base, so the same probe is used for both sides",
                     generated_from=dict(packets=str(args.packets_dir.resolve()),
                                         handler_static=str(args.static_dir.resolve())),
                     probes=[], contracts=[], static_evidence=[])

    contracts_md = ["# Registrar-chain runtime contracts", "",
                    f"Retail image sha256 `{RETAIL_SHA}` (base 0x400000). No runtime capture has been "
                    "performed for these contracts yet: expected retail *values* are to be captured first, "
                    "then compared; only static call/registration/order expectations are pre-filled below.", "",
                    "| address | kind | chain pos | observe | required | scenario | reachability |",
                    "| ------- | ---- | --------- | ------- | -------- | -------- | ------------ |"]

    def write_probe(address: str, config: dict, contract: dict, static_paths: list[str]) -> None:
        stem = address.lower()
        probe_path = probes_dir / f"probe_{stem}.js"
        template = TEMPLATE.read_text()
        probe_path.write_text(template.replace(
            "const CONFIG = __PROBE_CONFIG_JSON__;",
            "const CONFIG = " + json.dumps(config, indent=2) + ";"))
        contract_path = contracts_dir / f"{stem}.json"
        contract["probe"] = dict(path=str(probe_path.relative_to(run_dir)), sha256=sha256(probe_path))
        contract_path.write_text(json.dumps(contract, indent=2) + "\n")
        inventory["probes"].append(dict(address=address, probe=str(probe_path.relative_to(run_dir)),
                                        probe_sha256=sha256(probe_path), config=config))
        inventory["contracts"].append(dict(address=address,
                                            contract=str(contract_path.relative_to(run_dir)),
                                            contract_sha256=sha256(contract_path)))
        inventory["static_evidence"].extend(static_paths)

    # Registrar / delegated-body probes.
    for address, meta in REGISTRARS.items():
        packet = packets[address]
        if address == "0x00402930":
            continue
        expected = expected_registrations(packet)
        api_hooks, proven = derive_api_hooks(packet)
        effects = derive_side_effects(packet, address)
        expected_callsite = None
        if address.lower() in chain_by_target:
            expected_callsite = chain_by_target[address.lower()]["callsite"]
        registration_allowlist = [dict(callsite=row["callsite"].lower(), api=row.get("api_va"),
                                        names=[name["address"] for name in row.get("name_strings") or []])
                                   for row in expected]
        config = dict(target=dict(va=address, name=meta["label"]),
                      argWords=0, maxEvents=8192,
                      sideEffects=effects, apiHooks=api_hooks, provenStrings=proven,
                      chainCalls=[], registrationAllowlist=registration_allowlist)
        contract = dict(
            kind="igi-tester-registrar-contract", schema_version=1, address=address,
            label=meta["label"], chain_position=meta["position"],
            static_expectations=dict(
                bn_name=packet["decompiler"]["info"].get("Name"),
                bn_blocks=packet["decompiler"]["complexity"].get("Blocks"),
                r2=packet["static"].get("radare2"),
                angr=packet["static"].get("angr"),
                direct_calls=packet["call_count"],
                expected_callsite=expected_callsite,
                expected_registrations=expected,
                expected_side_effects=effects,
                expected_entry_count=1,
                **({"instrumentation_limitations": limitations[address]}
                   if address in limitations else {}),
            ),
            runtime_contract=dict(
                scenario=f"retail `window level1`, observe <= {args.observe_seconds} s; candidate same "
                         "scenario, sequential after retail",
                required_observables=dict(
                    entry_count="exactly retail observations for this registrar invocation",
                    return_count="equal to entry count for a non-aborting native",
                    arg_words="registrars take no arguments from 0x402930; observed words are informational",
                    return_value="must equal retail EAX for every matched call",
                    caller_return_address=f"must resolve to static callsite {expected_callsite}",
                    side_effects="every side effect label must be readable and byte-equal retail vs candidate",
                    registrations="ordered (api_va, name strings, argument words) must equal retail exactly",
                ),
                candidate_rule="PASS only when retail matches these static expectations and the candidate "
                                "matches retail on every required observable; unobserved or partially "
                                "observed runs are UNOBSERVED, never PASS",
                fail_closed=["missing entry/return", "different registration count/order/name bytes",
                             "unreadable or unequal side effect", "different return value",
                             "caller return address mismatch"],
            ),
            reachability=dict(
                position=meta["position"],
                chain="0x00402930 registration chain",
                requires_transcribed_prefix=[a for a, m in REGISTRARS.items()
                                             if 0 < m["position"] < meta["position"]],
                current_candidate="candidate dee2974c stub throws at position 1 (0x00508F80); nothing in "
                                  "the chain after that call is observable until the corresponding prefix "
                                  "is transcribed and rebuilt",
                observation_rule="observable only when the candidate reaches this call without throwing; "
                                  "otherwise report UNOBSERVED with the earliest divergence address",
            ),
        )
        static_paths = [str(p.relative_to(ROOT)) for p in
                        [args.packets_dir / f"{address.lower()}.json"]]
        write_probe(address, config, contract, static_paths)
        contracts_md.append(
            f"| {address} | registrar | {meta['position']} | {args.observe_seconds}s | entry/return, "
            f"{len(expected)} registrations, {len(effects)} side effects | retail window level1 | "
            f"needs chain prefix {', '.join(contract['reachability']['requires_transcribed_prefix']) or 'none'} |")

    # 0x5183D0 and 0x508FA0 delegated bodies.
    for delegate_address, (parent, delegate) in sorted(delegate_by_address.items()):
        probe_address = f"0x{int(delegate_address, 16):08X}"
        # Represent the delegated body with the same packet shape the helpers use.
        pseudo = dict(registrations=delegate.get("registrations") or [],
                      calls=delegate.get("calls") or [], callees=[])
        expected = expected_registrations(pseudo)
        api_hooks, proven = derive_api_hooks(pseudo)
        effects = derive_side_effects(pseudo, probe_address)
        expected_callsite = None
        for call in packets[parent].get("calls") or []:
            if addr_only(call.get("target_address")) == delegate_address:
                expected_callsite = call["callsite"]
                break
        chain_calls = []
        if probe_address == "0x005183D0":
            chain_calls = []
        for call in delegate.get("calls") or []:
            target = addr_only(call.get("target_address"))
            if not target:
                continue
            tail = bool(call.get("tail_jump"))
            chain_calls.append(dict(target=target, callsite=call["callsite"],
                                    label=call.get("target"), tailJump=tail,
                                    returnAddress=None if tail else f"0x{int(call['callsite'], 16) + 5:08x}"))
        registration_allowlist = [dict(callsite=row["callsite"].lower(), api=row.get("api_va"),
                                        names=[name["address"] for name in row.get("name_strings") or []])
                                   for row in expected]
        config = dict(target=dict(va=probe_address, name=delegate["info"].get("Name") or probe_address),
                      argWords=0, maxEvents=8192,
                      sideEffects=effects, apiHooks=api_hooks, provenStrings=proven,
                      chainCalls=chain_calls, registrationAllowlist=registration_allowlist)
        contract = dict(
            kind="igi-tester-registrar-contract", schema_version=1, address=probe_address,
            label=f"delegated body of {parent}", chain_position=REGISTRARS[parent]["position"],
            static_expectations=dict(
                bn_name=delegate["info"].get("Name"),
                bn_blocks=delegate["complexity"].get("Blocks"),
                r2=delegate["static"].get("radare2"),
                angr=delegate["static"].get("angr"),
                direct_calls=delegate["call_count"],
                expected_callsite=expected_callsite,
                expected_registrations=expected,
                expected_side_effects=effects,
                expected_entry_count=1,
                expected_chain_calls=chain_calls,
                **({"instrumentation_limitations": limitations[probe_address]}
                   if probe_address in limitations else {}),
            ),
            runtime_contract=dict(
                scenario=f"retail `window level1`, observe <= {args.observe_seconds} s",
                required_observables=dict(
                    entry_count="exactly one invocation from its parent registrar",
                    return_count="equal to entry count",
                    caller_return_address=(f"must resolve to static callsite {expected_callsite} (+instruction "
                                           f"length); for 0x5183D0 the caller is its 0x508F80 wrapper, whose "
                                           f"hook entry records the runtime value")
                    if not chain_calls else
                    "entry caller recorded; the declared chain limits below control edge evidence",
                    registrations="ordered registrations must equal retail exactly",
                    side_effects="byte-equal retail vs candidate",
                    chain_calls=("0x5183D0: all non-limited expected targets called in static order; "
                                 "six 0x4C1780 one-byte ret stubs and the first/last prologue-return "
                                 "anomalies are declared instrumentation limits (66/74 observable; "
                                 "RETAIL_CHAIN_PARTIAL, never MATCH)")
                    if chain_calls else "none",
                ),
                candidate_rule="PASS only when retail matches the static expectations and the candidate "
                                "matches retail; unobserved is UNOBSERVED",
                fail_closed=["missing invocation", "different registration/order", "unequal side effect"],
            ),
            reachability=dict(position=REGISTRARS[parent]["position"],
                              requires_transcribed_prefix=[a for a, m in REGISTRARS.items()
                                                           if 0 < m["position"] < REGISTRARS[parent]["position"]],
                              observation_rule="observable only when the parent registrar runs on the candidate"),
        )
        write_probe(probe_address, config, contract,
                    [str((args.packets_dir / f"{parent.lower()}.json").relative_to(ROOT))])
        contracts_md.append(
            f"| {probe_address} | delegated body | {REGISTRARS[parent]['position']} | "
            f"{args.observe_seconds}s | {len(expected)} registrations, {len(chain_calls)} chain calls | "
            f"retail window level1 | needs parent {parent} |")

    # 0x402930 chain-order probe.
    chain_targets = [dict(target=entry["target"], callsite=entry["callsite"], label=entry["label"],
                          returnAddress=entry["return_address"], tailJump=entry["tail_jump"])
                     for entry in chain]
    config = dict(target=dict(va="0x00402930", name="QTask_RegisterClasses registration chain"),
                  argWords=0, maxEvents=16384, sideEffects=[], apiHooks=[], provenStrings=[],
                  chainCalls=chain_targets)
    contract = dict(
        kind="igi-tester-registrar-contract", schema_version=1, address="0x00402930",
        label="QTask_RegisterClasses registration chain", chain_position=0,
        static_expectations=dict(
            bn_name=packets["0x00402930"]["decompiler"]["info"].get("Name"),
            bn_blocks=packets["0x00402930"]["decompiler"]["complexity"].get("Blocks"),
            r2=packets["0x00402930"]["static"].get("radare2"),
            angr=packets["0x00402930"]["static"].get("angr"),
            direct_calls=packets["0x00402930"]["call_count"],
            expected_chain_calls=chain,
            expected_entry_count=1,
            **({"instrumentation_limitations": limitations["0x00402930"]}
               if "0x00402930" in limitations else {}),
        ),
        runtime_contract=dict(
            scenario=f"retail `window level1`, observe <= {args.observe_seconds} s",
            required_observables=dict(
                entry_count="exactly one invocation (from QTask_CreateTask 0x00401000 at retail callsite "
                            "0x0048F7AA per prior evidence; contract records the observed caller)",
                return_value="retail returns EAX 7 per the prior matrix capture (documented); verify on "
                             "the first new retail run before accepting",
                chain_calls="all non-limited expected (target, callsite) pairs observed in static order "
                            "with caller_return_address matching the static callsite+instruction length; "
                            "declared instrumentation limitations (one-byte ret stubs at 0x4C1780 and the "
                            "first/tail prologue-return anomalies) are excused only at their declared "
                            "callsites and counts; result is RETAIL_CHAIN_PARTIAL, never MATCH",
            ),
            candidate_rule="with the current stub the candidate throws before any chain call: expect "
                            "UNOBSERVED, never PASS; after transcriptions, candidate must show the same "
                            "156 ordered (callsite, target) pairs and return value",
            fail_closed=["missing/extra chain target", "caller callsite mismatch outside declared limits",
                         "order inversion", "limited-count differs from the declared count"],
        ),
        reachability=dict(position=0, chain="0x00402930 registration chain",
                          requires_transcribed_prefix=[], observation_rule="retail observable; candidate "
                          "observable once the chain is transcribed"),
    )
    write_probe("0x00402930", config, contract,
                [str((args.packets_dir / "0x00402930.json").relative_to(ROOT))])
    contracts_md.append(
        f"| 0x00402930 | chain order | 0 | {args.observe_seconds}s | entry/return + 156 ordered "
        f"(callsite,target) pairs (152 observable; 4 declared instrumentation-limited) | "
        f"retail window level1 | none (candidate needs full chain) |")

    # Nested callbacks.
    for address, meta in HANDLERS.items():
        static = static_handlers[address]
        bn = static["binary_ninja"]
        effects = chunk_effects(HANDLER_SIDE_EFFECTS.get(address, []))
        config = dict(target=dict(va=address, name=meta["label"]), argWords=4, maxEvents=4096,
                      sideEffects=effects, apiHooks=[], provenStrings=[], chainCalls=[])
        contract = dict(
            kind="igi-tester-registrar-contract", schema_version=1, address=address,
            label=meta["label"], chain_position=None,
            static_expectations=dict(
                bn_name=bn_name(bn.get("info")),
                blocks=(static.get("cfg_cross_check") or {}).get("binary_ninja_blocks"),
                r2=dict(blocks=len(((static.get("radare2") or {}).get("cfg") or {}).get("nodes") or []),
                        edges=len(((static.get("radare2") or {}).get("cfg") or {}).get("edges") or [])),
                angr=static.get("angr"),
                expected_side_effects=effects,
            ),
            runtime_contract=dict(
                scenario=HANDLER_SCENARIO[meta["profile"]],
                required_observables=dict(
                    entry_count="observed count only (callback frequency is engine/scenario driven)",
                    return_count="equal to entry count for non-aborting callbacks",
                    arg_words="observed stack words must equal retail for every matched invocation",
                    return_value="must equal retail for every matched invocation",
                    side_effects="where labels are listed: readable and byte-equal retail vs candidate",
                ),
                candidate_rule="PASS only when at least one invocation exists on both sides and every "
                                "matched observable is equal; zero invocations is UNOBSERVED, never PASS",
                fail_closed=["candidate entry with no return (abort/throw)", "argument/return mismatch",
                             "unequal side effect"],
            ),
            reachability=dict(position=None,
                              observation_rule="independent of the QTask chain, but only fires when its "
                              "engine/script scenario executes; AI-graph callbacks are outside MVP scope"),
        )
        write_probe(address, config, contract,
                    [str((args.static_dir / f"{address.lower()}-static.json").relative_to(ROOT))])
        contracts_md.append(
            f"| {address} | handler | - | {args.observe_seconds}s | entry/return, arg words, retval"
            + (f", {len(effects)} side effects" if effects else "")
            + f" | {HANDLER_SCENARIO[meta['profile']].split(';')[0]} | scenario-driven / may be UNOBSERVED |")

    (run_dir / "contracts" / "CONTRACTS.md").write_text("\n".join(contracts_md) + "\n")

    baseline = [
        "# Registrar-chain retail baseline (static + prior runtime evidence)", "",
        f"Retail image: `igi.exe` sha256 `{RETAIL_SHA}` (base 0x400000), `/Users/haseeb-mir/Downloads/IGI/igi.exe`. "
        "No new launch was performed for this scaffolding.", "",
        "## Static retail expectations (from `registrars/` packets)", "",
        "- `0x00402930` is one basic block of 156 ordered targets (155 `call` + tail `jmp` at 0x402c37), "
        "no stack arguments, all return values ignored. r2 splits the tail jump into a second block; "
        "BN callee list and disassembly agree on the 156 targets.",
        "- Registration order at runtime is proven by each target's caller return address (the static "
        "callsite), not by BN names: `probe_0x00402930.js` hooks all 156 targets and keeps only calls whose "
        "return address equals the expected callsite.",
        "- Registrar expectations (address, call count, registration count, side effects):",
    ]
    for address, meta in REGISTRARS.items():
        if address == "0x00402930":
            continue
        packet = packets[address]
        baseline.append(
            f"  - `{address}` ({meta['label']}): {packet['call_count']} direct calls, "
            f"{len(expected_registrations(packet))} registration-API calls, "
            f"{len(derive_side_effects(packet, address))} side-effect blocks.")
    baseline += ["", "## Prior runtime evidence (existing captures, not re-launched)", "",
                 "- `build/tester/20261024T002148Z/runtime/retail-qtask.json`: `QTask_CreateTask` 0x00401000 "
                 "entry->return, `QTask_RegisterClasses` 0x00402930 entry->return observed once. "
                 "`build/tester/20261024T002148Z/MATRIX.md` documents retail EAX 1 for 0x00401000 and "
                 "EAX 7 for 0x00402930; those return values must be re-verified on the first new retail run "
                 "because the old JSON does not retain the EAX field.",
                 "- Frozen candidate for the first registrar gate: `6d39aa53216f2d0db4edb50afac1e217fa7dd8b5881f3f073404ba4621973e2e` (source `42490c2e`, commit `6f019e12`) "
                 "(Lead, built from commit 6849c7a5); the earlier 89c087ee/6a64... predecessor capture sets are superseded.",
                 "- `build/tester/20261024T002148Z/runtime/candidate-qtask.json`: candidate dee2974c (historical) shows "
                 "both entries and **no returns**; the chain aborts inside 0x00402930 before the first "
                 "registrar call.",
                 "- `build/tester/20261024T002148Z/runtime/candidate-direct-wine-exit.json`: uncaught "
                 "`std::logic_error` (`QTask_RegisterClasses ... AI graph registrar ... not transcribed`), "
                 "Wine exit code 3, 1.35 s.", "",
                 "## Runtime values still to capture (retail first, with Lead-frozen sha + lease)", "",
                 "1. Every registrar's EAX and entry/return counts.",
                 "2. TaskType ids stored at the packet-proven globals (0x54E83C, 0x5C8AB8, 0x5C8AB4, ...).",
                 "3. The 0x569474..0x56E174 clear range bytes after 0x00407D60 returns.",
                 "4. The ordered registration-API sequence with name-string bytes and returned ids.",
                 "5. Handler invocation counts/arguments/returns (scenario dependent).", "",
                 "Baselines are deliberately not pre-filled with invented values: retail capture fills them, "
                 "and the comparator fails closed if retail diverges from the static expectations above."]
    (run_dir / "BASELINE.md").write_text("\n".join(baseline) + "\n")

    readme = [
        "# Registrar-chain gate scaffolding (analysis only, nothing launched)", "",
        f"Run dir: `{run_dir.relative_to(ROOT)}`. Probes are generated from "
        "`scripts/tester/frida_registrar_chain_probe.js`; contracts are in `contracts/`.", "",
        f"Frozen candidate for the first gate: `{args.candidate_sha}` (deployed `igi1.exe`, Lead-frozen). "
        "The earlier `89c087ee...` capture set is superseded and must not be reused. Every candidate "
        "capture is sha-bound with `--expect-exe-sha256` and the comparator re-checks the report sha.", "",
        "## Sequential workflow (retail first, one game process at a time)", "",
        "1. Lead freezes the candidate sha and hands the game-run lease; never overlap the two games.",
        "2. Retail capture (one process):",
        "```",
        "python3 scripts/tester/frida_probe_driver.py \\",
        "  --exe /Users/haseeb-mir/Downloads/IGI/igi.exe \\",
        "  --probe build/tester/<run>/registrar-gates/probes/probe_<addr>.js \\",
        "  --target <retail-va> --name <label> \\",
        "  --level 1 --observe 30 \\",
        "  --out build/tester/<run>/registrar-gates/reports/retail-<addr>.json \\",
        f"  --expect-exe-sha256 {RETAIL_SHA}",
        "```",
        "3. Confirm the retail process is gone, then candidate capture with the same probe, "
        "`--exe /Users/haseeb-mir/Downloads/IGI/igi1.exe`, `--expect-exe-sha256 <frozen-candidate-sha>`, "
        "and `--hook probes/probe_<addr>.js:<retail-va>:<label>:<candidate-va>` when the candidate VA differs.",
        "4. Typed comparison:",
        "```",
        "python3 scripts/tester/compare_registrar_gate.py \\",
        "  --contract build/tester/<run>/registrar-gates/contracts/<addr>.json \\",
        "  --retail build/tester/<run>/registrar-gates/reports/retail-<addr>.json \\",
        "  --candidate build/tester/<run>/registrar-gates/reports/candidate-<addr>.json \\",
        "  --expected-candidate-sha <frozen-candidate-sha> \\",
        "  --out build/tester/<run>/registrar-gates/reports/comparison-<addr>.json",
        "```",
        "5. Verdicts: `MATCH` only when retail matches the static contract and the candidate matches retail; "
        "`UNOBSERVED` for zero invocations; `BLOCKED` for missing identity/hook failures/stale sha; "
        "`MISMATCH` otherwise. Never promote a status from this tool.", "",
        "## Probe inventory", "",
    ]
    for entry in inventory["probes"]:
        readme.append(f"- `{entry['address']}` -> `{entry['probe']}` (side effects "
                      f"{len(entry['config'].get('sideEffects') or [])}, api hooks "
                      f"{len(entry['config'].get('apiHooks') or [])}, chain hooks "
                      f"{len(entry['config'].get('chainCalls') or [])})")
    (run_dir / "README.md").write_text("\n".join(readme) + "\n")
    inventory["contracts_md"] = str((contracts_dir / "CONTRACTS.md").relative_to(run_dir))
    inventory["baseline_md"] = str((run_dir / "BASELINE.md").relative_to(run_dir))
    inventory["readme_md"] = str((run_dir / "README.md").relative_to(run_dir))
    (run_dir / "inventory.json").write_text(json.dumps(inventory, indent=2) + "\n")
    (run_dir / "contracts" / "CONTRACTS.md").write_text("\n".join(contracts_md) + "\n")
    print(f"wrote {len(inventory['probes'])} probes and contracts under {run_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(build())
