from __future__ import annotations

import hashlib
import json
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

import mvp_coverage_report as coverage


class CoverageReportTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        self.targets_path = self.root / coverage.TARGETS_REL
        self.manifest_path = self.root / coverage.MANIFEST_REL
        self.source_path = self.root / "engine" / "native.cpp"
        self.candidate_path = self.root / "build" / "candidate.exe"
        self.evidence_dir = self.root / coverage.EVIDENCE_REL
        self.targets_path.parent.mkdir(parents=True)
        self.source_path.parent.mkdir(parents=True)
        self.candidate_path.parent.mkdir(parents=True)
        self.evidence_dir.mkdir(parents=True)
        self.source_path.write_bytes(b"candidate source\n")
        self.candidate_path.write_bytes(b"candidate executable fixture\n")
        self.source_sha = hashlib.sha256(self.source_path.read_bytes()).hexdigest()
        self.binary_sha = "a" * 64
        self.address = "0x0048F530"
        self.target = {"address": self.address, "name": "Retail_Main", "category": "Flow"}
        self.targets_path.write_text(json.dumps([self.target]), encoding="utf-8")
        self.manifest = {
            "schema_version": 1,
            "binary_sha256": self.binary_sha,
            "natives": [{
                "address": self.address,
                "name": "Retail_Main",
                "binary_sha256": self.binary_sha,
                "implementation": {"path": "engine/native.cpp", "sha256": self.source_sha},
                "verification": {"status": "STRUCTURALLY_VERIFIED"},
            }],
        }
        self._write_manifest()

    def tearDown(self) -> None:
        self.temp.cleanup()

    def _write_manifest(self) -> None:
        self.manifest_path.parent.mkdir(parents=True, exist_ok=True)
        self.manifest_path.write_text(json.dumps(self.manifest), encoding="utf-8")

    def _add_evidence(self, name: str, artifact: dict, *, ref_status: str | None = None, corrupt_hash: bool = False) -> dict:
        path = self.evidence_dir / name
        path.write_text(json.dumps(artifact, sort_keys=True), encoding="utf-8")
        digest = hashlib.sha256(path.read_bytes()).hexdigest()
        if corrupt_hash:
            digest = "0" * 64
        ref = {"evidence_path": path.relative_to(self.root).as_posix(), "evidence_sha256": digest}
        if ref_status is not None:
            ref["status"] = ref_status
        return ref

    def _artifact(self, kind: str, **fields: object) -> dict:
        return {
            "schema_version": 1,
            "kind": kind,
            "address": self.address,
            "binary_sha256": self.binary_sha,
            "implementation_sha256": self.source_sha,
            "result": "passed",
            **fields,
        }

    def _symbols(self, _executable: Path, _nm: str = "unused", _run=None) -> list[str]:
        return ["00000001 T igi::retail::Main(igi::Config const*)"]

    def test_report_is_diagnostic_and_checks_bound_evidence(self) -> None:
        verification = self.manifest["natives"][0]["verification"]
        verification["semantic_review"] = self._add_evidence(
            "semantic.json", self._artifact("semantic-review-report", reviewed=True)
        )
        verification["build"] = self._add_evidence(
            "build.json", self._artifact("build-report", executed=True, command="compile")
        )
        behavioral = self._artifact(
            "behavioral-test-report", executed=True, tests_total=2, tests_passed=2, tests_failed=0
        )
        verification["behavioral"] = self._add_evidence("behavioral.json", behavioral)
        verification["runtime_tests"] = self._add_evidence(
            "behavioral.json", behavioral, ref_status="PASS"
        )
        self._write_manifest()
        trace = self.root / "build" / "retail-trace.json"
        trace.write_text(json.dumps({"binary_sha256": self.binary_sha, "observed_order": [self.address]}), encoding="utf-8")
        before_manifest = self.manifest_path.read_bytes()

        with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
            report = coverage.build_report(
                self.root, self.candidate_path, retail_trace="build/retail-trace.json"
            )
            second = coverage.build_report(
                self.root, self.candidate_path, retail_trace="build/retail-trace.json"
            )

        row = report["natives"][0]
        self.assertEqual(report, second)
        self.assertTrue(report["diagnostic_only"])
        self.assertFalse(report["acceptance_claim"])
        self.assertFalse(report["status_mutation"])
        self.assertEqual(row["candidate_symbol_name_match"]["state"], "unique_name_match")
        self.assertEqual(row["retail_trace_observation"], "observed")
        self.assertEqual(row["evidence_references"]["semantic_review"]["state"], "pass")
        self.assertEqual(row["evidence_references"]["build"]["state"], "pass")
        self.assertEqual(row["evidence_references"]["behavioral"]["state"], "pass")
        self.assertEqual(row["evidence_references"]["runtime_tests"]["state"], "pass")
        self.assertEqual(report["inputs"]["retail_binary_sha256"], self.binary_sha)
        self.assertEqual(row["manifest_binary_binding"], "matches_manifest")
        self.assertNotIn("fully_verified_end_to_end", report["summary"])
        self.assertEqual(before_manifest, self.manifest_path.read_bytes())

    def test_stale_or_missing_evidence_remains_nonpass(self) -> None:
        verification = self.manifest["natives"][0]["verification"]
        verification["build"] = self._add_evidence(
            "stale.json", self._artifact("build-report", executed=True, command="compile"), corrupt_hash=True
        )
        verification["semantic_review"] = {"evidence_path": "docs/fidelity/evidence/missing.json", "evidence_sha256": "f" * 64}
        self._write_manifest()

        with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
            report = coverage.build_report(self.root, self.candidate_path)

        refs = report["natives"][0]["evidence_references"]
        self.assertEqual(refs["build"]["state"], "stale")
        self.assertEqual(refs["semantic_review"]["state"], "invalid")
        self.assertEqual(refs["behavioral"]["state"], "unknown")

    def test_mismatched_evidence_source_hash_is_stale(self) -> None:
        artifact = self._artifact("build-report", executed=True, command="compile")
        artifact["implementation_sha256"] = "b" * 64
        self.manifest["natives"][0]["verification"]["build"] = self._add_evidence("build.json", artifact)
        self._write_manifest()

        with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
            report = coverage.build_report(self.root, self.candidate_path)

        self.assertEqual(report["natives"][0]["evidence_references"]["build"]["state"], "stale")

    def test_catalog_name_mismatch_is_diagnostic_but_address_is_authoritative(self) -> None:
        self.target["address"] = "0x004F82B0"
        self.target["name"] = "TerrainLightMap_Init_2"
        self.targets_path.write_text(json.dumps([self.target]), encoding="utf-8")
        self.manifest["natives"][0]["address"] = "0x004F82B0"
        self.manifest["natives"][0]["name"] = "TerrainLightMap_PushEntry"
        self._write_manifest()
        with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
            report = coverage.build_report(self.root, self.candidate_path)
        row = report["natives"][0]
        self.assertEqual(row["address"], "0x004F82B0")
        self.assertEqual(row["catalog_name"], "TerrainLightMap_PushEntry")
        self.assertFalse(row["catalog_name_matches_target"])

        self.manifest["natives"][0]["address"] = "0x004F82B1"
        self._write_manifest()
        with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
            with self.assertRaisesRegex(coverage.ReportError, "missing manifest addresses"):
                coverage.build_report(self.root, self.candidate_path)

    def test_top_level_binary_hash_is_canonical_and_record_mismatch_is_reported(self) -> None:
        record = self.manifest["natives"][0]
        record["binary_sha256"] = "b" * 64
        artifact = self._artifact("build-report", executed=True, command="compile")
        record["verification"]["build"] = self._add_evidence("canonical-build.json", artifact)
        self._write_manifest()

        with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
            report = coverage.build_report(self.root, self.candidate_path)

        row = report["natives"][0]
        self.assertEqual(report["inputs"]["retail_binary_sha256"], self.binary_sha)
        self.assertEqual(row["manifest_binary_binding"], "mismatch")
        self.assertEqual(row["evidence_references"]["build"]["state"], "pass")

    def test_duplicate_target_or_manifest_addresses_fail_closed(self) -> None:
        self.targets_path.write_text(json.dumps([self.target, self.target]), encoding="utf-8")
        with self.assertRaisesRegex(coverage.ReportError, "duplicate address in target list"):
            coverage._load_inventory(self.root)
        self.targets_path.write_text(json.dumps([self.target]), encoding="utf-8")
        self.manifest["natives"].append(dict(self.manifest["natives"][0]))
        self._write_manifest()
        with self.assertRaisesRegex(coverage.ReportError, "duplicate address in manifest"):
            coverage._load_inventory(self.root)

    def test_evidence_requires_supported_schema_version(self) -> None:
        for label, version in (("missing", None), ("unsupported", 2)):
            with self.subTest(label=label):
                artifact = self._artifact("build-report", executed=True, command="compile")
                if version is None:
                    artifact.pop("schema_version")
                else:
                    artifact["schema_version"] = version
                self.manifest["natives"][0]["verification"]["build"] = self._add_evidence(
                    f"{label}-schema.json", artifact
                )
                self._write_manifest()
                with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
                    report = coverage.build_report(self.root, self.candidate_path)
                evidence = report["natives"][0]["evidence_references"]["build"]
                self.assertEqual(evidence["state"], "invalid")
                self.assertIn("schema_version", evidence["reason"])

    def test_parse_nm_symbols_keeps_defined_text_names(self) -> None:
        output = """\n00000000 T igi::retail::Foo(int, unsigned char)\n00000010 W weak_function()\n00000020 D global_data\n         U unresolved()\n"""
        self.assertEqual(
            coverage.parse_nm_symbols(output),
            ["igi::retail::Foo(int, unsigned char)", "weak_function()"],
        )

    def test_nm_error_and_empty_symbol_table_fail_closed(self) -> None:
        failed = lambda *_args, **_kwargs: SimpleNamespace(returncode=3, stderr="bad executable", stdout="")
        with self.assertRaisesRegex(coverage.ReportError, "nm failed with exit code 3"):
            coverage.candidate_symbols(self.candidate_path, "fake-nm", failed)
        empty = lambda *_args, **_kwargs: SimpleNamespace(returncode=0, stderr="", stdout="no symbols")
        with self.assertRaisesRegex(coverage.ReportError, "no defined text symbols"):
            coverage.candidate_symbols(self.candidate_path, "fake-nm", empty)

    def test_repeated_candidate_and_protected_outputs_are_rejected(self) -> None:
        with self.assertRaises(SystemExit):
            coverage.main(["--candidate", str(self.candidate_path), "--candidate", str(self.candidate_path)], root=self.root)
        for path in (self.manifest_path, self.candidate_path, self.source_path, self.targets_path,
                     Path(coverage.__file__), Path(__file__), self.root / "docs/fidelity/evidence/report.json"):
            with self.subTest(path=path):
                with self.assertRaises(coverage.ReportError):
                    coverage._safe_output_path(self.root, str(path), self.candidate_path)
        existing = self.root / "input.json"
        existing.write_text('{"kind":"other-input"}', encoding="utf-8")
        with self.assertRaisesRegex(coverage.ReportError, "not a previous diagnostic report"):
            coverage._safe_output_path(self.root, str(existing), self.candidate_path)
        (self.root / ".git").mkdir()
        with patch.object(coverage.subprocess, "run", return_value=SimpleNamespace(returncode=0, stdout="", stderr="")):
            with self.assertRaisesRegex(coverage.ReportError, "output path is tracked"):
                coverage._safe_output_path(self.root, "tracked.json", self.candidate_path)

    def test_cli_writes_only_requested_output_and_preserves_manifest(self) -> None:
        before_manifest = self.manifest_path.read_bytes()
        output = self.root / "tmp-report.json"
        with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
            result = coverage.main(
                ["--candidate", str(self.candidate_path), "--out", str(output)], root=self.root
            )
        self.assertEqual(result, 0)
        report_bytes = output.read_bytes()
        self.assertEqual(json.loads(report_bytes)["kind"], "diagnostic-mvp-native-coverage")
        self.assertEqual(before_manifest, self.manifest_path.read_bytes())
        output.unlink()


if __name__ == "__main__":
    unittest.main()
