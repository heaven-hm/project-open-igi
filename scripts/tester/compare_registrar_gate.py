#!/usr/bin/env python3
"""Typed comparison for one registrar-chain gate contract (no status promotion).

Consumes two frida_probe_driver reports (retail first, candidate second) and the
contract written by build_registrar_gate.py:

  * identity: report executable sha256 / probe sha256 / target address,
  * retail baseline vs the static contract (call counts, expected caller callsite,
    ordered registration-API sequence and name bytes, ordered chain (target,
    callsite) pairs, readable side-effect blocks),
  * candidate vs retail on every required observable: entry/return counts, return
    values, handler argument words, side-effect bytes, registration name strings /
    api order / returned ids, chain call tuples.

Pointer-valued arguments are never compared across processes: only packet-proven
C-string contents, non-pointer semantics recorded by the contract, and
range-checked side-effect bytes are compared. A verdict is MATCH only when retail
matches the static contract and the candidate matches retail; zero observations
are UNOBSERVED and never a pass. This tool does not edit the manifest or promote
any status.
"""
from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

RETAIL_SHA = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"


def load(path: Path) -> dict:
    return json.loads(path.read_text())


def addr(value) -> int | None:
    if value is None:
        return None
    if isinstance(value, int):
        return value
    match = re.search(r"0x[0-9a-fA-F]+", str(value))
    return int(match.group(0), 16) if match else None


def norm(value) -> int | None:
    return addr(value)


class Gate:
    def __init__(self, contract: dict, retail: dict, candidate: dict | None,
                 expected_candidate_sha: str | None, address: str, retail_only: bool = False):
        self.contract = contract
        self.retail = retail
        self.candidate = candidate
        self.expected_candidate_sha = expected_candidate_sha
        self.address = address
        self.retail_only = retail_only
        self.checks: list[dict] = []
        self.verdict = "BLOCKED"
        self.partial_chain = False

    def check(self, name: str, ok: bool, detail: str, side: str = "retail") -> None:
        self.checks.append(dict(check=name, ok=bool(ok), side=side, detail=detail))

    # ---- extraction -----------------------------------------------------
    def entries(self, report: dict) -> list[dict]:
        return [e for e in report.get("entries") or [] if norm(e.get("address")) == norm(self.address)]

    def returns(self, report: dict) -> list[dict]:
        return [e for e in report.get("returns") or [] if norm(e.get("address")) == norm(self.address)]

    def events(self, report: dict, kind: str) -> list[dict]:
        return [e for e in report.get("events") or [] if e.get("type") == kind]

    def identity(self, report: dict, side: str) -> None:
        executable = report.get("executable") or {}
        if side == "retail":
            self.check("identity.retail_sha", executable.get("sha256") == RETAIL_SHA,
                       f"retail sha {executable.get('sha256')} == {RETAIL_SHA}")
        else:
            expected = self.expected_candidate_sha
            actual = executable.get("sha256")
            self.check("identity.candidate_sha", bool(expected) and actual == expected,
                       f"candidate sha {actual} == {expected}")
        targets = report.get("targets") or []
        hooks = report.get("hooks") or []
        matching = [t for t in targets if norm(t.get("retailVA")) == norm(self.address)
                    or norm(t.get("address")) == norm(self.address)]
        error = report.get("error")
        # A recorded tooling error (for example a one-byte `ret` stub Frida cannot
        # intercept) still has a valid identity; it is classified UNOBSERVED later
        # rather than silently treated as an installed hook.
        self.check("identity.target_hooked", bool(matching) and (bool(hooks) or bool(error)),
                   f"hooked targets {[(t.get('address'), t.get('name')) for t in targets]}; "
                   f"hooks={len(hooks)}; error={error}", side)
        if matching:
            expected_probe = (self.contract.get("probe") or {}).get("sha256")
            self.check("identity.probe_sha", matching[0].get("probe_sha256") == expected_probe,
                       f"probe sha {matching[0].get('probe_sha256')} == {expected_probe}", side)
            self.check("identity.hook_description", bool(hooks) or bool(error),
                       f"hooks={len(hooks)} error={error}", side)

    # ---- retail vs static contract -------------------------------------
    def retail_vs_contract(self) -> None:
        report = self.retail
        static = self.contract.get("static_expectations") or {}
        entries = self.entries(report)
        returns = self.returns(report)
        expected_entries = static.get("expected_entry_count")
        if expected_entries is not None:
            self.check("retail.entry_count", len(entries) == expected_entries,
                       f"entries {len(entries)} == {expected_entries}")
            self.check("retail.return_count", len(returns) == len(entries),
                       f"returns {len(returns)} == entries {len(entries)}")
        else:
            self.check("retail.entry_count_positive", len(entries) > 0, f"entries {len(entries)} > 0")

        expected_callsite = addr(static.get("expected_callsite"))
        if expected_callsite is not None:
            observed = [norm(e.get("callerReturnAddress")) for e in entries]
            self.check("retail.caller_callsite",
                       bool(observed) and all(value == expected_callsite for value in observed),
                       f"caller return addresses {[hex(v) if v else None for v in observed]} "
                       f"== {hex(expected_callsite)}")

        expected_regs = static.get("expected_registrations") or []
        observed_regs = self.events(report, "registration")
        if expected_regs:
            self.check("retail.registration_count", len(observed_regs) == len(expected_regs),
                       f"registrations {len(observed_regs)} == {len(expected_regs)}")
            for index, expected in enumerate(expected_regs):
                observed = observed_regs[index] if index < len(observed_regs) else {}
                self.check(f"retail.registration[{index}].api",
                           norm(observed.get("address") or observed.get("api")) == addr(expected["api_va"]),
                           f"observed {observed.get('address')} == {expected['api_va']}")
                expected_names = [item.get("text") for item in expected.get("name_strings") or []]
                observed_names = [item.get("text") for item in observed.get("nameStrings") or []]
                # The packet extractor truncates C strings at 31 characters. When
                # that is the only difference, the runtime observation is the full
                # retail string and the static text is a verbatim prefix; record it
                # as a truncation match rather than a divergence.
                def same_name(observed_text, expected_text):
                    if observed_text == expected_text:
                        return True
                    return bool(expected_text and len(expected_text) >= 31
                                and observed_text.startswith(expected_text))
                names_ok = len(observed_names) == len(expected_names) and all(
                    same_name(o, e) for o, e in zip(observed_names, expected_names))
                self.check(f"retail.registration[{index}].names", names_ok,
                           f"observed {observed_names} == static {expected_names}")
        elif self.contract.get("kind") == "igi-tester-registrar-contract":
            self.check("retail.registration_count", True, "no registration-API calls expected")

        expected_chain = static.get("expected_chain_calls") or []
        limitations = static.get("instrumentation_limitations") or {}
        if expected_chain and limitations:
            self.compare_partial_chain(report, expected_chain, limitations)
        elif expected_chain:
            observed_chain = self.events(report, "chain_call")
            self.check("retail.chain_count", len(observed_chain) == len(expected_chain),
                       f"chain calls {len(observed_chain)} == {len(expected_chain)}")
            mismatched = [event for event in observed_chain if event.get("matches") is False]
            self.check("retail.chain_callsites", not mismatched,
                       f"{len(mismatched)} chain calls observed from an unexpected callsite")
            pairs = [(norm(event.get("target")), norm(event.get("expectedCallsite")))
                     for event in observed_chain]
            expected_pairs = [(addr(item["target"]), addr(item["callsite"])) for item in expected_chain]
            self.check("retail.chain_order", pairs == expected_pairs,
                       "observed (target, callsite) order equals static order" if pairs == expected_pairs
                       else f"first divergence at {next((i for i, (a, b) in enumerate(zip(pairs, expected_pairs)) if a != b), 'length')}")

        expected_effects = static.get("expected_side_effects") or []
        if expected_effects:
            labels = {effect["label"] for effect in expected_effects}
            if returns:
                snapshot = {item.get("label"): item for item in returns[-1].get("sideEffects") or []}
                missing = sorted(labels - set(snapshot))
                unreadable = sorted(label for label in labels
                                    if snapshot.get(label, {}).get("status") != "read")
                self.check("retail.side_effects_present", not missing, f"missing {missing}")
                self.check("retail.side_effects_readable", not unreadable, f"unreadable {unreadable}")
            else:
                self.check("retail.side_effects_present", False, "no return observation to snapshot")

    def compare_partial_chain(self, report: dict, expected_chain: list[dict],
                              limitations: dict) -> None:
        """Compare a chain with exactly declared Frida instrumentation limits.

        Only the declared one-byte `ret` stubs (unhookable) and the declared
        first/tail calls (caller return address generated by Frida's prologue
        relocation or absent because of a tail jump) are excused. Every other
        missing, extra, reordered or retargeted row still fails. A pass here is
        RETAIL_CHAIN_PARTIAL, never MATCH.
        """
        self.partial_chain = True
        uninstrumentable = limitations.get("uninstrumentable") or []
        relocated = limitations.get("relocated_caller") or []
        limited: set[int] = set()
        declared_target: dict[int, int] = {}
        for item in uninstrumentable:
            for callsite in item.get("callsites") or []:
                limited.add(norm(callsite))
                declared_target[norm(callsite)] = norm(item.get("target"))
        for item in relocated:
            limited.add(norm(item.get("callsite")))
            declared_target[norm(item.get("callsite"))] = norm(item.get("target"))
        expected_total = int(limitations.get("expected_total_rows", len(expected_chain)))
        expected_observed = limitations.get("expected_observed_rows")
        required = [(norm(row.get("callsite")), norm(row.get("target")))
                    for row in expected_chain if norm(row.get("callsite")) not in limited]
        observed = self.events(report, "chain_call")
        mismatches = self.events(report, "chain_mismatch")
        failures = self.events(report, "chain_install_failure")
        self.check("retail.chain_total_declared", expected_total == len(expected_chain),
                   f"declared total {expected_total} == rows {len(expected_chain)}")
        self.check("retail.chain_expected_observed_declared",
                   expected_observed is not None and int(expected_observed) == len(required),
                   f"declared observed {expected_observed} == required rows {len(required)}")
        self.check("retail.chain_count", len(observed) == len(required),
                   f"observed {len(observed)} == declared {len(required)}")
        observed_pairs = [(norm(event.get("expectedCallsite")), norm(event.get("target")))
                          for event in observed]
        first_difference = next(((a, b) for a, b in zip(observed_pairs, required) if a != b), None)
        self.check("retail.chain_order", observed_pairs == required,
                   f"ordered pairs differ: {first_difference}")
        expected_failures = [(norm(callsite), declared_target[norm(callsite)])
                             for item in uninstrumentable
                             for callsite in item.get("callsites") or []]
        observed_failures = [(norm(event.get("callsite")), norm(event.get("target")))
                             for event in failures]
        self.check("retail.chain_install_failures", observed_failures == expected_failures,
                   f"failures {observed_failures} == declared {expected_failures}")
        expected_relocated = {(norm(item.get("callsite")), norm(item.get("target")))
                              for item in relocated}
        observed_relocated = [(norm(event.get("expectedCallsite")), norm(event.get("target")))
                              for event in mismatches]
        self.check("retail.chain_relocated_callers",
                   set(observed_relocated) == expected_relocated
                   and len(observed_relocated) == len(expected_relocated),
                   f"relocated {observed_relocated} == declared {sorted(expected_relocated)}")
        unexpected = [event for event in observed if event.get("matches") is False]
        self.check("retail.chain_callsites", not unexpected,
                   f"{len(unexpected)} rows reported an unexpected callsite")

    # ---- candidate vs retail -------------------------------------------
    def observe(self, report: dict) -> dict:
        entries = self.entries(report)
        returns = self.returns(report)
        return dict(
            entries=entries,
            returns=returns,
            entry_count=len(entries),
            return_count=len(returns),
            retvals=[norm(item.get("returnValue")) for item in returns],
            arg_words=[[norm(word) for word in item.get("argWords") or []] for item in entries],
            effects={item.get("label"): (item.get("status"), (item.get("hex") or "").lower())
                     for item in (returns[-1].get("sideEffects") if returns else []) or []},
            registrations=[dict(api=norm(event.get("address")),
                               names=[name.get("text") for name in event.get("nameStrings") or []],
                               retval=norm(event.get("returnValue")))
                           for event in self.events(report, "registration")],
            chain=[(norm(event.get("target")), norm(event.get("expectedCallsite")),
                    event.get("matches")) for event in self.events(report, "chain_call")],
        )

    def candidate_vs_retail(self) -> None:
        retail = self.observe(self.retail)
        candidate = self.observe(self.candidate)
        static = self.contract.get("static_expectations") or {}
        handlers = self.contract.get("chain_position") is None

        self.check("candidate.entry_count", candidate["entry_count"] == retail["entry_count"],
                   f"{candidate['entry_count']} == retail {retail['entry_count']}", "candidate")
        if retail["entry_count"] == 0:
            return
        self.check("candidate.return_count", candidate["return_count"] == retail["return_count"],
                   f"{candidate['return_count']} == retail {retail['return_count']}", "candidate")
        self.check("candidate.return_values", candidate["retvals"] == retail["retvals"],
                   f"{candidate['retvals']} == retail {retail['retvals']}", "candidate")
        if handlers:
            self.check("candidate.arg_words", candidate["arg_words"] == retail["arg_words"],
                       "handler argument words equal retail", "candidate")
        if retail["effects"]:
            self.check("candidate.side_effects", candidate["effects"] == retail["effects"],
                       f"side-effect bytes equal retail: labels {sorted(candidate['effects'])}", "candidate")
        if static.get("expected_registrations"):
            self.check("candidate.registrations",
                       candidate["registrations"] == retail["registrations"],
                       f"registration sequence {candidate['registrations']} == retail {retail['registrations']}",
                       "candidate")
        if "expected_chain_calls" in static and static.get("expected_chain_calls"):
            self.check("candidate.chain", candidate["chain"] == retail["chain"],
                       "chain (target, callsite, matches) sequence equals retail", "candidate")

    def run(self) -> dict:
        if self.retail_only:
            try:
                self.identity(self.retail, "retail")
                if not all(item["ok"] for item in self.checks):
                    self.verdict = "BLOCKED"
                else:
                    self.retail_vs_contract()
                    if len(self.entries(self.retail)) == 0:
                        self.verdict = "RETAIL_UNOBSERVED"
                    elif all(item["ok"] for item in self.checks):
                        self.verdict = "RETAIL_CHAIN_PARTIAL" if self.partial_chain else "RETAIL_MATCH"
                    else:
                        self.verdict = "RETAIL_BASELINE_DIVERGES"
            except Exception as error:
                self.check("internal", False, f"{type(error).__name__}: {error}")
                self.verdict = "BLOCKED"
            return dict(kind="igi-tester-registrar-gate-comparison", schema_version=1,
                        address=self.address, verdict=self.verdict, mode="retail-only",
                        retail_report=str(self.retail.get("captured_at")),
                        candidate_report=None, expected_candidate_sha=None,
                        checks=self.checks,
                        note="RETAIL_MATCH means retail matches the static contract only; candidate not "
                             "captured in this mode; it is not a pass and never promotes manifest statuses")
        try:
            self.identity(self.retail, "retail")
            self.identity(self.candidate, "candidate")
            if not all(item["ok"] for item in self.checks):
                self.verdict = "BLOCKED"
            else:
                self.retail_vs_contract()
                self.candidate_vs_retail()
                retail_entries = len(self.entries(self.retail))
                candidate_entries = len(self.entries(self.candidate))
                if retail_entries == 0:
                    self.verdict = "UNOBSERVED"
                elif candidate_entries == 0:
                    self.verdict = "UNOBSERVED"
                elif all(item["ok"] for item in self.checks):
                    self.verdict = "MATCH"
                else:
                    self.verdict = "MISMATCH"
        except Exception as error:  # fail closed
            self.check("internal", False, f"{type(error).__name__}: {error}")
            self.verdict = "BLOCKED"
        return dict(kind="igi-tester-registrar-gate-comparison", schema_version=1,
                    address=self.address, verdict=self.verdict,
                    retail_report=str(self.retail.get("captured_at")),
                    candidate_report=str(self.candidate.get("captured_at")),
                    expected_candidate_sha=self.expected_candidate_sha,
                    checks=self.checks,
                    note="MATCH requires retail==static contract and candidate==retail; UNOBSERVED is "
                         "never a pass; this tool never promotes manifest statuses")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--contract", type=Path, required=True)
    ap.add_argument("--retail", type=Path, required=True)
    ap.add_argument("--candidate", type=Path)
    ap.add_argument("--expected-candidate-sha")
    ap.add_argument("--retail-only", action="store_true",
                    help="compare only the retail capture against the static contract")
    ap.add_argument("--out", type=Path, required=True)
    args = ap.parse_args()
    contract = load(args.contract)
    if args.retail_only:
        if args.candidate:
            raise SystemExit("--retail-only does not take --candidate")
        gate = Gate(contract, load(args.retail), None, None,
                    contract.get("address", "unknown"), retail_only=True)
    else:
        if not args.expected_candidate_sha or not args.candidate:
            raise SystemExit("--expected-candidate-sha and --candidate are required unless --retail-only")
        gate = Gate(contract, load(args.retail), load(args.candidate),
                    args.expected_candidate_sha.lower(), contract.get("address", "unknown"))
    result = gate.run()
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(result, indent=2) + "\n")
    md = [f"# Registrar gate — {result['address']}: **{result['verdict']}**", "",
          f"Retail report captured_at `{result['retail_report']}`, candidate `{result['candidate_report']}`.",
          f"Candidate sha `{result['expected_candidate_sha']}`.", "", "| check | side | ok | detail |",
          "| ----- | ---- | -- | ------ |"]
    for check in result["checks"]:
        md.append(f"| {check['check']} | {check['side']} | {'PASS' if check['ok'] else 'FAIL'} | "
                  f"{str(check['detail']).replace('|', '\\|')[:220]} |")
    md += ["", "This is evidence only; it does not change any native-verification status."]
    args.out.with_suffix(".md").write_text("\n".join(md) + "\n")
    print(f"{result['address']}: {result['verdict']} ({sum(1 for c in result['checks'] if c['ok'])}/"
          f"{len(result['checks'])} checks) -> {args.out}")
    return 0 if result["verdict"] in ("MATCH", "RETAIL_MATCH", "RETAIL_CHAIN_PARTIAL") else 1


if __name__ == "__main__":
    raise SystemExit(main())
