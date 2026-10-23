#!/usr/bin/env python3
"""Compute the pull list from a parallel transcription branch.

The MVP register on this branch only tracks evidence-backed acceptance: a native
can be unverified here while a parallel branch already holds a full
transcription of it. Pulling code that already exists is cheaper than
transcribing it again, but a whole-file checkout is only safe when the file does
not also implement natives that are verified here - evidence is bound to the
file's SHA-256, so swapping the file silently invalidates those records.

For every unverified MVP native this report states:

  evidence-only   the implementation file is byte-identical on both branches, so
                  there is no code to pull - only runtime evidence work remains
  adopt-file      the file is new, or it holds no native verified here, so the
                  parallel version can be adopted and re-gated
  hunk-merge      the file also holds verified natives; only the body of this
                  function may be pulled, and the overlapping records must be
                  re-gated afterwards
  no-code-to-pull the parallel branch has no implementation for the native

Usage:
  python3 scripts/codex_sync_report.py --codex-ref codex-smart-workflow --write
"""

from __future__ import annotations

import argparse
import collections
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
REGISTER = "docs/fidelity/mvp-verification-blockers.json"
MANIFEST = "docs/fidelity/native-verification.json"
DEFAULT_OUT = "docs/fidelity/codex-sync.json"


def git(*args: str, root: Path = ROOT) -> str:
    result = subprocess.run(["git", *args], cwd=root, capture_output=True, text=True)
    if result.returncode != 0:
        raise SystemExit(f"git {' '.join(args)} failed: {result.stderr.strip()}")
    return result.stdout


def load_json(ref: str | None, path: str) -> dict | None:
    if ref is None:
        candidate = ROOT / path
        return json.loads(candidate.read_text()) if candidate.exists() else None
    result = subprocess.run(["git", "show", f"{ref}:{path}"], cwd=ROOT,
                            capture_output=True, text=True)
    if result.returncode != 0:
        return None
    return json.loads(result.stdout)


def blob(ref: str, path: str) -> str | None:
    result = subprocess.run(["git", "rev-parse", f"{ref}:{path}"], cwd=ROOT,
                            capture_output=True, text=True)
    return result.stdout.strip() if result.returncode == 0 else None


def verified_paths(manifest: dict | None) -> tuple[set[str], collections.Counter[str]]:
    """Verified addresses and how many of them each implementation file holds."""
    addresses: set[str] = set()
    per_file: collections.Counter[str] = collections.Counter()
    for record in (manifest or {}).get("natives", []):
        status = (record.get("verification") or {}).get("status", "")
        if not status.startswith("VERIFIED"):
            continue
        address = record["address"].upper()
        addresses.add(address)
        path = (record.get("implementation") or {}).get("path")
        if path:
            per_file[path] += 1
    return addresses, per_file


def build_report(codex_ref: str, my_ref: str | None) -> dict:
    register = load_json(my_ref, REGISTER)
    if register is None:
        raise SystemExit(f"cannot read {REGISTER} on {my_ref or 'the worktree'}")
    mine = load_json(my_ref, MANIFEST)
    codex = load_json(codex_ref, MANIFEST)
    if codex is None:
        raise SystemExit(f"cannot read {MANIFEST} on {codex_ref}")

    my_verified, per_file = verified_paths(mine)
    codex_records = {r["address"].upper(): r for r in codex.get("natives", [])}

    rows = []
    for record in register["natives"]:
        address = record["address"].upper()
        if address in my_verified:
            continue
        my_path = (record.get("implementation") or {}).get("path")
        if not my_path:
            # The register records the path it was evaluated against; fall back
            # to this branch's manifest when the register omits it.
            my_record = next((r for r in (mine or {}).get("natives", [])
                              if r["address"].upper() == address), None)
            my_path = ((my_record or {}).get("implementation") or {}).get("path")
        codex_record = codex_records.get(address)
        codex_path = ((codex_record or {}).get("implementation") or {}).get("path")
        codex_status = ((codex_record or {}).get("verification") or {}).get("status")

        if not codex_record or not codex_path:
            file_state, action = "absent", "no-code-to-pull"
        elif not my_path:
            file_state, action = "missing-here", "adopt-file"
        else:
            my_blob, codex_blob = blob(my_ref or "HEAD", my_path), blob(codex_ref, codex_path)
            if my_blob is None:
                file_state, action = "missing-here", "adopt-file"
            elif codex_blob is None:
                file_state, action = "missing-on-codex", "no-code-to-pull"
            elif my_path == codex_path and my_blob == codex_blob:
                file_state, action = "identical", "evidence-only"
            else:
                overlap = per_file.get(my_path, 0)
                file_state = "differs"
                action = "hunk-merge" if overlap else "adopt-file"
        rows.append({
            "address": record["address"],
            "name": record.get("name", ""),
            "blocker": record.get("blocker", ""),
            "my_implementation": my_path,
            "codex_status": codex_status,
            "codex_implementation": codex_path,
            "file_state": file_state,
            "verified_natives_in_that_file": per_file.get(my_path or "", 0),
            "action": action,
        })

    summary = collections.Counter(row["action"] for row in rows)
    file_summary = collections.Counter(row["file_state"] for row in rows)
    return {
        "schema_version": 1,
        "codex_ref": codex_ref,
        "my_ref": my_ref or "worktree",
        "my_verified_count": len(my_verified),
        "codex_record_count": len(codex_records),
        "codex_status_counts": dict(collections.Counter(
            ((r.get("verification") or {}).get("status") or "UNKNOWN")
            for r in codex.get("natives", []))),
        "remaining_mvp_count": len(rows),
        "actions": dict(summary),
        "file_states": dict(file_summary),
        "rows": rows,
    }


def render_markdown(report: dict) -> str:
    lines = [
        "# Parallel-branch pull list",
        "",
        "Generated by `scripts/codex_sync_report.py`; do not edit by hand.",
        "",
        f"- parallel branch: `{report['codex_ref']}`",
        f"- verified on this branch: **{report['my_verified_count']}**",
        f"- MVP natives still unverified here: **{report['remaining_mvp_count']}**",
        f"- parallel branch statuses: " + ", ".join(
            f"{name} {count}" for name, count in sorted(report["codex_status_counts"].items())),
        "",
        "| action | count | meaning |",
        "| --- | --- | --- |",
    ]
    meanings = {
        "evidence-only": "implementation file is identical; only runtime evidence remains",
        "adopt-file": "safe to adopt the parallel file, then re-gate",
        "hunk-merge": "file holds verified natives here; pull only this function body",
        "no-code-to-pull": "parallel branch has no implementation for this native",
    }
    for action in ("evidence-only", "adopt-file", "hunk-merge", "no-code-to-pull"):
        count = report["actions"].get(action, 0)
        if count:
            lines.append(f"| {action} | {count} | {meanings[action]} |")
    lines += ["", "## Natives", "", "| address | name | blocker | parallel status | file | action | verified in file |",
              "| --- | --- | --- | --- | --- | --- | --- |"]
    for row in report["rows"]:
        name = row["name"]
        lines.append(
            f"| `{row['address']}` | {name} | {row['blocker']} | {row['codex_status']} | "
            f"`{row['codex_implementation'] or '-'}` | {row['action']} | "
            f"{row['verified_natives_in_that_file']} |")
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--codex-ref", default="codex-smart-workflow",
                        help="branch or commit holding the parallel transcriptions")
    parser.add_argument("--my-ref", default=None,
                        help="compare this ref instead of the worktree files")
    parser.add_argument("--out", default=DEFAULT_OUT, help="JSON report path")
    parser.add_argument("--markdown", default=None, help="optional markdown path")
    parser.add_argument("--write", action="store_true", help="write the report files")
    args = parser.parse_args()

    report = build_report(args.codex_ref, args.my_ref)
    text = json.dumps(report, indent=2) + "\n"
    if args.write:
        (ROOT / args.out).write_text(text)
        if args.markdown:
            (ROOT / args.markdown).write_text(render_markdown(report))
        print(f"wrote {args.out}" + (f" and {args.markdown}" if args.markdown else ""))
    else:
        sys.stdout.write(text)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
