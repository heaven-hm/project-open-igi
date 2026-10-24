"""REA-style evidence records: observations / inferences / unknowns with
confidence, tool provenance, digests, and the project's verification statuses.

Every conclusion separates observations (directly seen), inferences (derived),
and unknowns (not established). Confidence is recorded honestly; it is never a
substitute for the underlying evidence (AGENTS.md 10.4.3, 10.5).
"""

from __future__ import annotations

import hashlib
import json
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from . import SCHEMA_VERSION

# Verification statuses from AGENTS.md 10.5, in ascending evidence order.
STATUS_ORDER = [
    "ANALYZED",
    "TRANSCRIBED",
    "STRUCTURALLY_VERIFIED",
    "BUILD_VERIFIED",
    "RUNTIME_TESTED",
    "VERIFIED_WITH_DOCUMENTED_SCOPE",
    "VERIFIED",
]

# Confidence labels mapped to a 0..1 weight for aggregation/ledger counts.
CONFIDENCE_WEIGHT = {
    "confirmed": 1.0,     # directly observed and cross-checked
    "strong": 0.8,        # observed, single tool
    "inferred": 0.5,      # derived from evidence, not directly observed
    "weak": 0.25,         # plausible, thin evidence
    "unknown": 0.0,       # not established
}


def utc_now() -> str:
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


@dataclass
class Provenance:
    tool: str
    version: str | None = None
    command: str | None = None
    source: str | None = None

    def as_dict(self) -> dict[str, Any]:
        return {k: v for k, v in {
            "tool": self.tool, "version": self.version,
            "command": self.command, "source": self.source}.items() if v is not None}


@dataclass
class EvidenceBuilder:
    kind: str
    address: str | None = None
    binary_sha256: str | None = None
    result: str = "analyzed"
    status: str = "ANALYZED"
    observations: list[dict[str, Any]] = field(default_factory=list)
    inferences: list[dict[str, Any]] = field(default_factory=list)
    unknowns: list[dict[str, Any]] = field(default_factory=list)
    limits: list[str] = field(default_factory=list)
    provenance: list[Provenance] = field(default_factory=list)
    fields: dict[str, Any] = field(default_factory=dict)

    def observe(self, text: str, *, confidence: str = "confirmed",
                detail: Any = None, source: str | None = None) -> "EvidenceBuilder":
        self.observations.append(_claim(text, confidence, detail, source))
        return self

    def infer(self, text: str, *, confidence: str = "inferred",
              detail: Any = None, source: str | None = None) -> "EvidenceBuilder":
        self.inferences.append(_claim(text, confidence, detail, source))
        return self

    def unknown(self, text: str, *, reason: str = "") -> "EvidenceBuilder":
        entry: dict[str, Any] = {"text": text, "confidence": "unknown"}
        if reason:
            entry["reason"] = reason
        self.unknowns.append(entry)
        return self

    def limit(self, text: str) -> "EvidenceBuilder":
        self.limits.append(text)
        return self

    def add_provenance(self, prov: Provenance) -> "EvidenceBuilder":
        self.provenance.append(prov)
        return self

    def set(self, key: str, value: Any) -> "EvidenceBuilder":
        self.fields[key] = value
        return self

    def build(self) -> dict[str, Any]:
        doc: dict[str, Any] = {
            "schema_version": SCHEMA_VERSION,
            "kind": self.kind,
            "result": self.result,
            "generated_at": utc_now(),
            "generator": "re_harness",
            "status": self.status,
            "confidence_summary": self._confidence_summary(),
        }
        if self.address is not None:
            doc["address"] = self.address
        if self.binary_sha256 is not None:
            doc["binary_sha256"] = self.binary_sha256
        doc["observations"] = self.observations
        doc["inferences"] = self.inferences
        doc["unknowns"] = self.unknowns
        doc["limits"] = self.limits
        doc["provenance"] = [p.as_dict() for p in self.provenance]
        for key, value in self.fields.items():
            doc[key] = value
        return doc

    def _confidence_summary(self) -> dict[str, Any]:
        counts = {"observations": len(self.observations), "inferences": len(self.inferences),
                  "unknowns": len(self.unknowns)}
        weights = [CONFIDENCE_WEIGHT.get(c, 0.0) for c in
                   [o.get("confidence") for o in self.observations] +
                   [i.get("confidence") for i in self.inferences]]
        counts["mean_confidence"] = round(sum(weights) / len(weights), 3) if weights else 0.0
        return counts


def _claim(text: str, confidence: str, detail: Any, source: str | None) -> dict[str, Any]:
    entry: dict[str, Any] = {"text": text, "confidence": confidence}
    if detail is not None:
        entry["detail"] = detail
    if source is not None:
        entry["source"] = source
    return entry


def write_evidence(evidence_dir: Path, filename: str, document: dict[str, Any]) -> dict[str, Any]:
    """Write an evidence document and return its identity (path + sha256)."""
    evidence_dir.mkdir(parents=True, exist_ok=True)
    path = evidence_dir / filename
    text = json.dumps(document, indent=2, ensure_ascii=False) + "\n"
    path.write_text(text, encoding="utf-8")
    digest = hashlib.sha256(text.encode("utf-8")).hexdigest()
    return {"path": str(path), "filename": filename, "evidence_sha256": digest}


def evidence_id(path: Path) -> str:
    return "re-harness:" + path.stem
