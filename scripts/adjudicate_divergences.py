#!/usr/bin/env python3
"""Adjudicate all GameNatives naming divergences against retail igi.exe evidence.

Rules, applied from strongest to weakest evidence:

  string_evidence   a retail string literal inside the function states the name
                    (e.g. Error_Show("Unable to allocate event")).
  block_coherent    BN names form a consistent library block (Vec3_*, Mat33_*)
                    where the GameNatives names are unrelated Ghidra guesses.
  thunk             the function is a single tail-jump thunk; the name must agree
                    with the jump target, so a caller-side alias is wrong.
  body_evidence     the decompiled body matches exactly one of the two names.
  ambiguous         neither is supported; no rename until resolved.
"""

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def nz(s):
    return re.sub(r"[^a-z0-9]", "", str(s).lower())


def strip_suffix(s):
    return re.sub(r"_\d+$", "", nz(s))


def main() -> None:
    ev = json.load(open(ROOT / "assets" / "gamenatives_divergence_evidence.json"))
    allrows = {r["address"].lower(): r for r in
               json.load(open(ROOT / "assets" / "gamenatives_bn_survey.json"))}

    # coherent library prefixes already present in the Binary Ninja naming
    bn_prefixes = {}
    for r in allrows.values():
        n = nz(r["bnName"] or "")
        m = re.match(r"^([a-z]+?)(?:_|$)", n)
        if m:
            bn_prefixes.setdefault(m.group(1), 0)
            bn_prefixes[m.group(1)] += 1

    verdicts = []
    for e in ev:
        gn = e["gnNames"][0]
        bn = e["bnName"]
        body, strings = e["body"], e["strings"]
        target = None
        reason = ""
        confidence = "LOW"

        # 1. thunk: single tailcall, the alias must match the real callee
        tm = re.search(r"return ([A-Za-z_0-9]+)\(\);", body)
        if "tailcall" in body and tm:
            callee = tm.group(1)
            if strip_suffix(callee) == strip_suffix(bn.replace("j_", "")):
                verdict, reason = "bn", f"thunk to {callee}; GameNatives alias '{gn}' is a caller-side guess"
                confidence = "HIGH"
            else:
                verdict, reason = "ambiguous", f"thunk to {callee} matches neither name"
            verdicts.append({**e, "verdict": verdict, "reason": reason,
                              "confidence": confidence})
            continue

        # 2. a retail string names the function outright
        matched = None
        for s in strings:
            for cand, who in ((bn, "bn"), (gn, "gn")):
                base = re.sub(r"_\d+$", "", cand)
                probe = nz(base.split("_", 1)[-1])
                if probe and len(probe) >= 4 and nz(s).find(probe) >= 0:
                    matched = (who, s)
                    break
            if matched:
                break
        if matched:
            who, s = matched
            verdict = who if who == "bn" else "gn"
            other = gn if verdict == "bn" else bn
            reason = f"retail string {s!r} supports '{bn if verdict=='bn' else gn}'"
            confidence = "HIGH"
            verdicts.append({**e, "verdict": verdict, "reason": reason,
                              "confidence": confidence})
            continue

        # 3. block coherence
        bnp, gnp = nz(bn).split("_")[0], nz(gn).split("_")[0]
        b_count = sum(1 for r in allrows.values()
                      if (r["bnName"] or "").startswith(bnp + "_"))
        g_count = sum(1 for r in allrows.values()
                      if (r["gnNames"] and r["gnNames"][0].startswith(gnp + "_")))
        if b_count >= 5 and g_count <= 2:
            verdict, reason = "bn", (f"'{bn}' sits in a {b_count}-function BN library block; "
                                     f"GameNatives prefix '{gnp}' has only {g_count}")
            confidence = "MEDIUM"
            verdicts.append({**e, "verdict": verdict, "reason": reason,
                              "confidence": confidence})
            continue

        # 4. body evidence
        verdict, reason = "ambiguous", "no string, thunk or block evidence resolves this pair"
        if nz(bn) != nz(gn):
            bl = body.lower()
            if bn.lower().split("_")[-1] in bl and gn.lower().split("_")[-1] not in bl:
                verdict, reason, confidence = "bn", "decompiled body references the BN name", "MEDIUM"
            elif gn.lower().split("_")[-1] in bl and bn.lower().split("_")[-1] not in bl:
                verdict, reason, confidence = "gn", "decompiled body references the GameNatives name", "MEDIUM"
        verdicts.append({**e, "verdict": verdict, "reason": reason,
                          "confidence": confidence})

    dest = ROOT / "assets" / "gamenatives_verdicts.json"
    dest.write_text(json.dumps(verdicts, indent=2))
    tally = {}
    for v in verdicts:
        tally[v["verdict"]] = tally.get(v["verdict"], 0) + 1
    print(tally)
    for v in verdicts:
        flag = {"bn": "BN ", "gn": "GN ", "ambiguous": "?? "}[v["verdict"]]
        print(f"{flag} {v['address']}  gn={v['gnNames'][0]:<34} bn={v['bnName']:<34} [{v['confidence']}] {v['reason'][:60]}")


if __name__ == "__main__":
    main()
