#!/usr/bin/env python3
"""Run angr CFGFast recovery over the whole GameNatives address set.

angr is the third independent method required by AGENTS.md 10.2 (Binary Ninja
decompilation + Radare2 CFG + angr CFGFast). CFGFast is approximate structural
evidence only; it does not prove semantic or behavioural equivalence.

Runs in a single angr process so the PE is loaded once for all addresses.
"""

import json
import pathlib
import sys
import time

ROOT = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))

IGI = pathlib.Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")
RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"


def main() -> None:
    rows = json.load(open(ROOT / "assets" / "gamenatives_bn_survey.json"))
    addrs = [r["address"] for r in rows]

    import angr
    from angr import options as so

    proj = angr.Project(str(IGI), auto_load_libs=False,
                         main_opts={"base_addr": 0x400000})
    cfg = proj.analyses.CFGFast(normalize=True, data_references=True,
                                 regions=[(0x400000, 0xC35058)])

    out = {}
    t0 = time.time()
    kb = cfg.kb
    for i, a in enumerate(addrs, 1):
        key = f"0x{int(a, 16):08X}"
        addr = int(a, 16)
        func = kb.functions.get(addr)
        if func is None:
            out[key] = {"result": "no_function", "block_count": 0,
                        "is_function_entry": False}
        else:
            blocks = [n for n in func.graph.nodes if getattr(n, "size", 0) > 0]
            out[key] = {
                "result": "pass",
                "function_entry": f"0x{func.addr:08X}",
                "block_count": len(blocks),
                "is_function_entry": func.addr == addr,
            }
        if i % 500 == 0:
            print(f"  {i}/{len(addrs)} {time.time()-t0:.0f}s", flush=True)

    dest = ROOT / "assets" / "gamenatives_angr.json"
    dest.write_text(json.dumps({
        "tool": "angr", "analysis": "CFGFast",
        "angr_version": angr.__version__,
        "binary": str(IGI), "binary_sha256": RETAIL_SHA256,
        "limitations": ["CFGFast is approximate static analysis",
                         "does not prove semantic or behavioural equivalence"],
        "addresses": out,
    }, indent=2))

    entries = sum(1 for v in out.values() if v.get("result") == "pass")
    starts = sum(1 for v in out.values() if v.get("is_function_entry"))
    print(f"\nanalysed={len(out)} recovered={entries} entry_match={starts} "
          f"elapsed={time.time()-t0:.0f}s")
    print(f"written {dest}")


if __name__ == "__main__":
    main()
