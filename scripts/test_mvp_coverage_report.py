from __future__ import annotations

import hashlib
import json
import os
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

    def test_boolean_behavioral_counts_are_not_valid_integers(self) -> None:
        for field, value in (("tests_total", True), ("tests_passed", True), ("tests_failed", False)):
            with self.subTest(field=field):
                artifact = self._artifact(
                    "behavioral-test-report", executed=True, tests_total=1, tests_passed=1, tests_failed=0
                )
                artifact[field] = value
                self.manifest["natives"][0]["verification"]["behavioral"] = self._add_evidence(
                    f"boolean-{field}.json", artifact
                )
                self._write_manifest()

                with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
                    report = coverage.build_report(self.root, self.candidate_path)

                result = report["natives"][0]["evidence_references"]["behavioral"]
                self.assertEqual(result["state"], "failed")
                self.assertIn("counts", result["reason"])

    def test_manifest_requires_supported_schema_version(self) -> None:
        for label, version in (("missing", None), ("unsupported", 2), ("boolean", True), ("string", "1")):
            with self.subTest(label=label):
                if version is None:
                    self.manifest.pop("schema_version", None)
                else:
                    self.manifest["schema_version"] = version
                self._write_manifest()
                with self.assertRaisesRegex(coverage.ReportError, "manifest schema_version"):
                    coverage._load_inventory(self.root)
                self.manifest["schema_version"] = 1

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

    def test_top_level_binary_hash_is_canonical_and_record_mismatch_fails_closed(self) -> None:
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
        result = row["evidence_references"]["build"]
        self.assertEqual(result["state"], "invalid")
        self.assertIn("per-record", result["reason"])

    def test_absent_top_level_binary_hash_cannot_be_authorised_by_record_hash(self) -> None:
        self.manifest.pop("binary_sha256")
        record = self.manifest["natives"][0]
        record["verification"]["build"] = self._add_evidence(
            "record-only-build.json", self._artifact("build-report", executed=True, command="compile")
        )
        self._write_manifest()

        with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
            report = coverage.build_report(self.root, self.candidate_path)

        row = report["natives"][0]
        # The report still says the retail binding is unknown; a record-level hash
        # alone must not turn that into a passing evidence state.
        self.assertIsNone(report["inputs"]["retail_binary_sha256"])
        self.assertEqual(row["manifest_binary_binding"], "unknown")
        result = row["evidence_references"]["build"]
        self.assertNotEqual(result["state"], "pass")
        self.assertEqual(result["state"], "unknown")
        self.assertEqual(result["reason"], "retail binary binding is missing")

    def test_malformed_top_level_binary_hash_fails_closed(self) -> None:
        record = self.manifest["natives"][0]
        record["verification"]["build"] = self._add_evidence(
            "malformed-top-level-build.json", self._artifact("build-report", executed=True, command="compile")
        )
        cases = (
            ("boolean", True),
            ("non_hex_string", "not-a-hash"),
            ("integer", 123),
            ("short_hex", "a" * 63),
            ("non_hex_64", "z" * 64),
            ("null", None),
        )
        for label, value in cases:
            with self.subTest(label=label):
                self.manifest["binary_sha256"] = value
                self._write_manifest()
                with self.assertRaisesRegex(coverage.ReportError, "binary_sha256"):
                    coverage._load_inventory(self.root)
                with self.assertRaisesRegex(coverage.ReportError, "binary_sha256"):
                    coverage.build_report(self.root, self.candidate_path)
                exit_code = coverage.main(
                    ["--candidate", str(self.candidate_path), "--out", f"build/malformed-{label}.json"],
                    root=self.root,
                )
                self.assertEqual(exit_code, 2)
                self.assertFalse((self.root / "build" / f"malformed-{label}.json").exists())

    def test_validate_evidence_ref_rejects_missing_or_malformed_top_level_hash(self) -> None:
        ref = self._add_evidence(
            "direct-build.json", self._artifact("build-report", executed=True, command="compile")
        )
        cases = (
            ("absent", None, "unknown"),
            ("boolean", True, "invalid"),
            ("non_hex", "not-a-hash", "invalid"),
            ("integer", 123, "invalid"),
        )
        for label, value, expected in cases:
            with self.subTest(label=label):
                result = coverage.validate_evidence_ref(
                    self.root,
                    "build",
                    ref,
                    address=self.address,
                    binary_sha256=value,
                    implementation_sha256=self.source_sha,
                    implementation_is_current=True,
                    record_binary_sha256=self.binary_sha,
                )
                self.assertNotEqual(result["state"], "pass")
                self.assertEqual(result["state"], expected)
                self.assertIn("retail binary binding", result["reason"])

    def test_fully_agreeing_manifest_record_and_artifact_still_pass(self) -> None:
        record = self.manifest["natives"][0]
        verification = record["verification"]
        verification["semantic_review"] = self._add_evidence(
            "agreeing-semantic.json", self._artifact("semantic-review-report", reviewed=True)
        )
        verification["build"] = self._add_evidence(
            "agreeing-build.json", self._artifact("build-report", executed=True, command="compile")
        )
        behavioral = self._artifact(
            "behavioral-test-report", executed=True, tests_total=3, tests_passed=3, tests_failed=0
        )
        verification["behavioral"] = self._add_evidence("agreeing-behavioral.json", behavioral)
        self._write_manifest()

        with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
            report = coverage.build_report(self.root, self.candidate_path)

        row = report["natives"][0]
        self.assertEqual(report["inputs"]["retail_binary_sha256"], self.binary_sha)
        self.assertEqual(row["manifest_binary_binding"], "matches_manifest")
        for field in ("semantic_review", "build", "behavioral"):
            with self.subTest(field=field):
                self.assertEqual(row["evidence_references"][field]["state"], "pass")

    def test_artifact_retail_hash_must_agree_with_manifest_and_record(self) -> None:
        record = self.manifest["natives"][0]
        cases = (
            ("artifact_differs", self.binary_sha, "b" * 64, "stale"),
            ("record_differs_from_top_level", "b" * 64, self.binary_sha, "invalid"),
            ("record_and_artifact_differ", "b" * 64, "b" * 64, "invalid"),
            ("record_is_malformed", "not-a-hash", self.binary_sha, "invalid"),
        )
        for label, record_hash, artifact_hash, expected in cases:
            with self.subTest(label=label):
                record["binary_sha256"] = record_hash
                artifact = self._artifact("build-report", executed=True, command="compile")
                artifact["binary_sha256"] = artifact_hash
                record["verification"]["build"] = self._add_evidence(f"retail-{label}.json", artifact)
                self._write_manifest()

                with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
                    report = coverage.build_report(self.root, self.candidate_path)

                result = report["natives"][0]["evidence_references"]["build"]
                self.assertEqual(result["state"], expected)
                self.assertNotEqual(result["state"], "pass")

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

    def test_output_rejects_git_marker_worktree_and_common_metadata(self) -> None:
        metadata_root = self.root / "git-metadata"
        gitdir = metadata_root / "worktrees" / "reporter"
        gitdir.mkdir(parents=True)
        (gitdir / "commondir").write_text("../..", encoding="utf-8")
        marker = self.root / ".git"
        marker.write_text("gitdir: git-metadata/worktrees/reporter\n", encoding="utf-8")

        outputs = (
            marker / "config",
            gitdir / "index",
            metadata_root / "config",
        )
        for output in outputs:
            with self.subTest(output=output):
                with self.assertRaisesRegex(coverage.ReportError, "Git or worktree metadata"):
                    coverage._safe_output_path(self.root, str(output), self.candidate_path)

    def test_symlinked_output_parent_or_final_path_is_refused(self) -> None:
        redirect = self.root / "redirect"
        redirect.mkdir()
        parent_link = self.root / "linked-parent"
        parent_link.symlink_to(redirect, target_is_directory=True)
        with self.assertRaisesRegex(coverage.ReportError, "symlinked parent"):
            coverage._safe_output_path(self.root, "linked-parent/report.json", self.candidate_path)

        target = self.root / "existing-input.json"
        target.write_text("{}", encoding="utf-8")
        final_link = self.root / "final-link.json"
        final_link.symlink_to(target)
        with self.assertRaisesRegex(coverage.ReportError, "symlink"):
            coverage._safe_output_path(self.root, "final-link.json", self.candidate_path)
        self.assertEqual(target.read_text(encoding="utf-8"), "{}")

    def test_writer_refuses_parent_swapped_after_validation(self) -> None:
        output_dir = self.root / "validated-dir"
        output_dir.mkdir()
        output = coverage._safe_output_path(self.root, "validated-dir/report.json", self.candidate_path)
        original = self.root / "validated-dir-original"
        output_dir.rename(original)
        redirect = self.root / "swapped-target"
        redirect.mkdir()
        output_dir.symlink_to(redirect, target_is_directory=True)

        with self.assertRaisesRegex(coverage.ReportError, "symlink"):
            coverage._write_output(self.root, output, b"{}\n")
        self.assertFalse((redirect / "report.json").exists())

    def test_output_cannot_equal_or_hard_link_to_retail_trace(self) -> None:
        trace = self.root / "build" / "trace.json"
        trace.parent.mkdir(parents=True, exist_ok=True)
        trace.write_text(json.dumps({
            "kind": coverage.DIAGNOSTIC_REPORT_KIND,
            "diagnostic_only": True,
            "binary_sha256": self.binary_sha,
            "observed_order": [self.address],
        }), encoding="utf-8")
        before = trace.read_bytes()

        for value in ("build/trace.json", str(trace)):
            with self.subTest(output=value):
                with self.assertRaisesRegex(coverage.ReportError, "retail trace"):
                    coverage._safe_output_path(
                        self.root, value, self.candidate_path, retail_trace="build/trace.json"
                    )

        hardlink = self.root / "build" / "trace-alias.json"
        os.link(trace, hardlink)
        with self.assertRaisesRegex(coverage.ReportError, "hard-linked"):
            coverage._safe_output_path(
                self.root, str(hardlink), self.candidate_path, retail_trace="build/trace.json"
            )

        with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
            result = coverage.main(
                [
                    "--candidate", str(self.candidate_path),
                    "--retail-trace", "build/trace.json",
                    "--out", "build/trace.json",
                ],
                root=self.root,
            )
        self.assertEqual(result, 2)
        self.assertEqual(before, trace.read_bytes())

    def test_cli_writes_only_requested_output_and_preserves_manifest(self) -> None:
        before_manifest = self.manifest_path.read_bytes()
        output = self.root / "build" / "diagnostic" / "nested-report.json"
        with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
            result = coverage.main(
                ["--candidate", str(self.candidate_path), "--out", str(output)], root=self.root
            )
        self.assertEqual(result, 0)
        self.assertTrue(output.is_file())
        report_bytes = output.read_bytes()
        self.assertEqual(json.loads(report_bytes)["kind"], "diagnostic-mvp-native-coverage")
        self.assertEqual(before_manifest, self.manifest_path.read_bytes())

        with patch.object(coverage, "candidate_symbols", side_effect=self._symbols):
            rerun = coverage.main(
                ["--candidate", str(self.candidate_path), "--out", str(output)], root=self.root
            )
        self.assertEqual(rerun, 0)
        self.assertEqual(json.loads(output.read_bytes())["kind"], "diagnostic-mvp-native-coverage")
        output.unlink()


if __name__ == "__main__":
    unittest.main()
