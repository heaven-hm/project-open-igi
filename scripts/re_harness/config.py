"""Configuration, profile loading, retail-binary identity and catalog access.

All path resolution is relative to the project root (the parent of scripts/).
The retail executable is the behavior authority: every stage re-checks its
SHA-256 before trusting any address-derived evidence (AGENTS.md 10.3).
"""

from __future__ import annotations

import hashlib
import json
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

try:  # PyYAML is used for the target-agnostic config; degrade gracefully if absent.
    import yaml
except Exception:  # pragma: no cover - optional dependency
    yaml = None  # type: ignore[assignment]

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS = ROOT / "scripts"
PKG = Path(__file__).resolve().parent
PROFILES_DIR = PKG / "profiles"
DEFAULT_CONFIG_PATH = PKG / "re_harness.yaml"

EXPECTED_RETAIL_SHA256 = (
    "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
)


class ConfigError(Exception):
    """Raised when required configuration or identity cannot be established."""


def sha256_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def _load_yaml(path: Path) -> dict[str, Any]:
    if yaml is None:
        raise ConfigError(f"PyYAML unavailable; cannot read {path.name}")
    with path.open("encoding=" "utf-8") as handle:
        data = yaml.safe_load(handle)
    if not isinstance(data, dict):
        raise ConfigError(f"{path.name}: expected a mapping at top level")
    return data


def _load_json(path: Path) -> Any:
    with path.open(encoding="utf-8") as handle:
        return json.load(handle)


@dataclass
class RetailTarget:
    retail_binary: Path
    retail_sha256: str
    image_base: int
    code_range: tuple[int, int]
    registrars: list[int] = field(default_factory=list)
    binary_ok: bool = False
    hash_matches: bool = False
    notes: list[str] = field(default_factory=list)

    def verify(self) -> tuple[bool, list[str]]:
        """Return (identity_ok, messages). Fail closed on any mismatch."""
        messages: list[str] = []
        if not self.retail_binary.is_file():
            messages.append(f"retail binary missing: {self.retail_binary}")
            return False, messages
        actual = sha256_file(self.retail_binary)
        self.binary_ok = True
        if actual != self.retail_sha256:
            messages.append(
                f"retail SHA-256 mismatch: expected {self.retail_sha256}, got {actual}"
            )
            self.hash_matches = False
            return False, messages
        self.hash_matches = True
        messages.append(f"retail binary present and SHA-256 matches ({self.retail_binary})")
        return True, messages


@dataclass
class HarnessConfig:
    profile_name: str
    base: dict[str, Any]
    profile: dict[str, Any]
    retail: RetailTarget
    evidence_dir: Path
    catalog_path: Path | None
    catalog: dict[str, Any] | None
    catalog_index: dict[int, dict[str, Any]] = field(default_factory=dict)
    warnings: list[str] = field(default_factory=list)

    # --- convenience accessors -------------------------------------------------
    def cfg(self, *path: str, default: Any = None) -> Any:
        """Read a nested key from merged (base-over-profile) config."""
        cur: Any = self.base
        for key in path:
            if isinstance(cur, dict) and key in cur:
                cur = cur[key]
            else:
                return default
        return cur

    @property
    def decompiler_order(self) -> list[str]:
        return list(self.cfg("decompiler_order", default=["ghidra_cli"]))

    @property
    def runtime_cfg(self) -> dict[str, Any]:
        return self.cfg("runtime", default={}) or {}

    @property
    def scope_gate(self) -> dict[str, Any]:
        return self.cfg("scope_gate", default={}) or {}

    def in_code_range(self, va: int) -> bool:
        lo, hi = self.retail.code_range
        return lo <= va <= hi

    def catalog_lookup(self, va: int) -> dict[str, Any] | None:
        return self.catalog_index.get(va)

    def scope_gate_result(self) -> tuple[bool, list[str]]:
        """Fail-closed evaluation of the run scope gate."""
        reasons: list[str] = []
        gate = self.scope_gate
        if gate.get("require_retail_binary", True) and not self.retail.retail_binary.is_file():
            reasons.append("scope gate: retail binary missing")
        if gate.get("require_retail_hash_match", True) and not self.retail.hash_matches:
            reasons.append("scope gate: retail SHA-256 did not match (or not verified)")
        if gate.get("require_catalog", True) and self.catalog_path is None:
            reasons.append("scope gate: native catalog missing")
        return (len(reasons) == 0), reasons


def _merge_profile_into_base(base: dict[str, Any], profile: dict[str, Any]) -> dict[str, Any]:
    """Shallow-merge profile values over base defaults (profile wins per key)."""
    merged = dict(base)
    for key, value in profile.items():
        if isinstance(value, dict) and isinstance(merged.get(key), dict):
            merged[key] = {**merged[key], **value}
        else:
            merged[key] = value
    return merged


def load_config(profile_name: str = "igi", *, base_path: Path | None = None) -> HarnessConfig:
    base_path = base_path or DEFAULT_CONFIG_PATH
    base: dict[str, Any] = {}
    if base_path.is_file():
        base = _load_yaml(base_path)
    else:
        base = {"scope_gate": {}, "runtime": {}, "tools": {}, "decompiler_order": ["ghidra_cli"]}

    profile: dict[str, Any] = {}
    profile_path = PROFILES_DIR / f"{profile_name}.json"
    if not profile_path.is_file():
        raise ConfigError(f"profile not found: {profile_path}")
    profile = _load_json(profile_path)

    merged = _merge_profile_into_base(base, profile)
    cfg = HarnessConfig(profile_name=profile_name, base=merged, profile=profile,
                        retail=_init_retail(profile),
                        evidence_dir=_resolve(profile.get("evidence_dir", "docs/fidelity/evidence")),
                        catalog_path=None, catalog=None)

    # Catalog access (best effort; absence is a recorded warning, not a crash).
    catalog_rel = profile.get("natives", {}).get("catalog")
    if catalog_rel:
        catalog_path = (ROOT / catalog_rel).resolve()
        cfg.catalog_path = catalog_path
        if catalog_path.is_file():
            try:
                catalog = _load_json(catalog_path)
                cfg.catalog = catalog
                cfg.catalog_index = _index_catalog(catalog, profile.get("natives", {}))
            except (OSError, ValueError) as error:
                cfg.warnings.append(f"catalog present but unreadable ({catalog_path}): {error}")
        else:
            cfg.warnings.append(f"catalog declared but missing: {catalog_path}")

    identity_ok, messages = cfg.retail.verify()
    if not identity_ok:
        cfg.warnings.extend(messages)
    return cfg


def _init_retail(profile: dict[str, Any]) -> RetailTarget:
    target = profile.get("target", {})
    code = target.get("code_range", ["0x401000", "0x532510"])
    lo = int(str(code[0]), 16)
    hi = int(str(code[1]), 16)
    registrars = [int(str(r), 16) for r in target.get("natives", {}).get("registrars", [])] \
        if "natives" in target else []
    if not registrars:
        registrars = [int(str(r), 16) for r in profile.get("natives", {}).get("registrars", [])]
    return RetailTarget(
        retail_binary=(ROOT / target.get("retail_binary", "")).resolve()
        if target.get("retail_binary") and not target.get("retail_binary").startswith("/")
        else Path(target.get("retail_binary", "/Users/haseeb-mir/Downloads/IGI/igi.exe")).resolve(),
        retail_sha256=target.get("retail_sha256", EXPECTED_RETAIL_SHA256),
        image_base=int(str(target.get("image_base", "0x400000")), 16),
        code_range=(lo, hi),
        registrars=registrars,
    )


def _resolve(rel: str) -> Path:
    p = Path(rel)
    return p if p.is_absolute() else (ROOT / rel).resolve()


def _index_catalog(catalog: dict[str, Any], natives_cfg: dict[str, Any]) -> dict[int, dict[str, Any]]:
    key = natives_cfg.get("catalog_key", "Natives")
    addr_field = natives_cfg.get("address_field", "address")
    entries = catalog.get(key)
    if not isinstance(entries, list):
        return {}
    index: dict[int, dict[str, Any]] = {}
    for entry in entries:
        if not isinstance(entry, dict):
            continue
        raw = entry.get(addr_field)
        va = _coerce_va(raw)
        if va is not None and va not in index:
            index[va] = entry
    return index


def _coerce_va(raw: Any) -> int | None:
    if raw is None:
        return None
    text = str(raw).strip()
    if not text:
        return None
    try:
        return int(text, 16) if text.lower().startswith("0x") else int(text)
    except ValueError:
        return None
