#!/usr/bin/env python3
"""Build docs/fidelity/native-database.md from the verification manifest.

The manifest (docs/fidelity/native-verification.json) is the single source of truth for
native status. This script renders it as one Markdown table per status so the database
cannot drift from the records: regenerate it whenever records change.

Columns: native name | signature | retail address | CFG | verification comment.

The comment column says what evidence actually exists for that row - decoder agreement,
runtime observation counts, or the explicit reason the row is not promoted - because a
status word alone is not proof.

Usage: python3 scripts/build_native_database.py
"""
from __future__ import annotations

import json
import pathlib

ROOT = pathlib.Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs/fidelity/native-verification.json"
OUTPUT = ROOT / "docs/fidelity/native-database.md"

# Strongest first. The order also defines how the document reads.
STATUS_ORDER = [
    "VERIFIED",
    "VERIFIED_WITH_DOCUMENTED_SCOPE",
    "RUNTIME_TESTED",
    "BUILD_VERIFIED",
    "STRUCTURALLY_VERIFIED",
    "TRANSCRIBED",
    "ANALYZED",
    "INCONCLUSIVE",
]

SESSION_EXPLANATION = (
    "Records written in the Level 1 MVP session (2026-10-25). They carry static structure "
    "matching retail at CFG 1.000 and retail-side runtime observations, but no candidate runtime "
    "comparison, because the candidate exits at 0x00402930 before executing game natives. They are "
    "therefore not claimed as verified."
)


def cfg_of(row: dict) -> str:
    verification = row.get("verification") or {}
    value = verification.get("cfg_similarity")
    return f"{value:.3f}" if isinstance(value, (int, float)) else "-"


def comment_of(row: dict) -> str:
    verification = row.get("verification") or {}
    bits: list[str] = []
    status = verification.get("status", "?")
    if isinstance(verification.get("cfg_similarity"), (int, float)):
        bits.append(f"CFG {cfg_of(row)}")
    if verification.get("binary_ninja") and verification.get("radare2"):
        bits.append("bn+r2 agree")
    elif verification.get("binary_ninja"):
        bits.append("bn evidence")
    elif verification.get("radare2"):
        bits.append("r2 evidence")
    review = verification.get("semantic_review")
    if isinstance(review, dict) and review.get("evidence_path"):
        bits.append(f"review `{pathlib.Path(review['evidence_path']).name}`")
    if verification.get("runtime_contract"):
        bits.append("runtime contract")
    if verification.get("behavioral_runner"):
        bits.append("paired behavioral runner")
    notes = verification.get("notes")
    if isinstance(notes, str) and notes.strip():
        bits.append(notes.strip().replace("|", "/"))
    if not bits:
        bits.append("no verification fields recorded")
    if status in {"TRANSCRIBED", "ANALYZED"} and "review" not in " ".join(bits):
        bits.append("static only")
    return "; ".join(bits).replace("\n", " ")


def signature_of(row: dict) -> str:
    implementation = row.get("implementation") or {}
    signature = implementation.get("signature")
    if isinstance(signature, str) and signature.strip() and signature != "unknown":
        return signature.replace("|", "/")
    natives = row.get("game_natives") or {}
    other = natives.get("signature")
    if isinstance(other, str) and other.strip() and other != "unknown":
        return other.replace("|", "/")
    return "unestablished"


def has_evidence(row: dict) -> bool:
    """True only when the record carries checkable evidence, not just a status word."""
    verification = row.get("verification") or {}
    decoder = bool(verification.get("binary_ninja")) or bool(verification.get("radare2"))
    review = isinstance(verification.get("semantic_review"), dict)
    behavioural = bool(verification.get("behavioral_runner")) or isinstance(verification.get("runtime_contract"), dict)
    return decoder and review and behavioural


def main() -> int:
    manifest = json.loads(MANIFEST.read_text())
    rows = [row for row in manifest.get("natives", []) if row.get("address")]
    rows.sort(key=lambda row: int(row["address"], 16))

    buckets: dict[str, list[dict]] = {status: [] for status in STATUS_ORDER}
    for row in rows:
        status = (row.get("verification") or {}).get("status", "UNKNOWN")
        buckets.setdefault(status, []).append(row)

    lines = [
        "# Game native database",
        "",
        "Generated from `docs/fidelity/native-verification.json` by `scripts/build_native_database.py`.",
        "The manifest is the source of truth; this table is a rendering of it and must be regenerated, never edited by hand.",
        "",
        f"Retail artifact: `igi.exe` SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, image base `0x00400000`.",
        "",
        "Status meanings follow `AGENTS.md` section 10.5. **Only rows under VERIFIED and VERIFIED_WITH_DOCUMENTED_SCOPE",
        "carry a verification claim**; everything below those two sections is static work in progress, and saying otherwise",
        "would misrepresent the tree.",
        "",
    ]

    unbacked = [row for status in ("VERIFIED", "VERIFIED_WITH_DOCUMENTED_SCOPE")
                for row in (buckets.get(status) or []) if not has_evidence(row)]
    unbacked_addresses = {row["address"].lower() for row in unbacked}
    for status in STATUS_ORDER:
        if status in ("VERIFIED", "VERIFIED_WITH_DOCUMENTED_SCOPE"):
            buckets[status] = [row for row in buckets.get(status) or [] if row["address"].lower() not in unbacked_addresses]

    for status in STATUS_ORDER + sorted(set(buckets) - set(STATUS_ORDER)):
        rows_for_status = buckets.get(status) or []
        if status == "VERIFIED" and rows_for_status:
            lines.append("Rows below carry decoder evidence, a semantic review and a behavioral/runtime contract, and were re-checked")
            lines.append("against this branch's records. Legacy rows whose VERIFIED status has no such record are listed separately at")
            lines.append("the end of this document instead of being counted here.")
            lines.append("")
        if not rows_for_status:
            continue
        lines.append(f"## {status} ({len(rows_for_status)})")
        lines.append("")
        if status == "TRANSCRIBED":
            lines.append(SESSION_EXPLANATION)
            lines.append("")
        lines.append("| native name | signature | retail address | CFG | verification comment |")
        lines.append("| --- | --- | --- | --- | --- |")
        for row in rows_for_status:
            name = (row.get("name") or "unnamed").replace("|", "/")
            lines.append(
                f"| `{name}` | `{signature_of(row)}` | `{row['address']}` | {cfg_of(row)} | {comment_of(row)} |"
            )
        lines.append("")

    if unbacked:
        lines.append(f"## STATUS CLAIM WITHOUT RECORDED EVIDENCE ({len(unbacked)})")
        lines.append("")
        lines.append("These rows claim VERIFIED or VERIFIED_WITH_DOCUMENTED_SCOPE in the manifest but their record carries no decoder")
        lines.append("evidence, no semantic review, or no behavioral/runtime contract. They are historical statuses from earlier branch")
        lines.append("work and were **not** re-validated in this session, so they are not counted as verified here. Reproducing their")
        lines.append("evidence, or downgrading the status, is required before they can be relied on.")
        lines.append("")
        lines.append("| native name | signature | retail address | CFG | verification comment |")
        lines.append("| --- | --- | --- | --- | --- |")
        for row in sorted(unbacked, key=lambda item: int(item["address"], 16)):
            name = (row.get("name") or "unnamed").replace("|", "/")
            lines.append(f"| `{name}` | `{signature_of(row)}` | `{row['address']}` | {cfg_of(row)} | claim without recorded evidence |")
        lines.append("")

    OUTPUT.write_text("\n".join(lines))
    print(f"wrote {OUTPUT.relative_to(ROOT)} with {len(rows)} rows")
    for status in STATUS_ORDER:
        count = len(buckets.get(status) or [])
        if count:
            print(f"  {status}: {count}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
