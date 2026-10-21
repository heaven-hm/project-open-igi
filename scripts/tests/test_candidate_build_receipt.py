import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import candidate_build_receipt as receipt


class ReceiptTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name).resolve()
        self.build = self.root / 'build'
        self.build.mkdir()
        self.compiler = self.root / 'compiler'
        self.compiler.write_bytes(b'compiler fixture')
        (self.build / 'CMakeCache.txt').write_text(f'CMAKE_CXX_COMPILER:FILEPATH={self.compiler}\n')
        (self.root / 'CMakeLists.txt').write_text('# fixture\n')
        self.source = self.root / 'engine/native.cpp'
        self.source.parent.mkdir()
        self.source.write_text('int native() { return 1; }\n')
        object_dir = self.build / 'engine/platform/windows/CMakeFiles/open-igi.dir'
        object_dir.mkdir(parents=True)
        (object_dir / 'link.txt').write_text('compiler -o candidate.exe\n')
        (object_dir / 'native.cpp.obj').write_bytes(b'object fixture')
        (object_dir / 'native.cpp.obj.d').write_text(f'native.cpp.obj: {self.source}\n')
        self.candidate = self.build / 'engine/platform/windows/candidate.exe'
        self.candidate.write_bytes(b'x86 fixture')
        self.receipt = self.root / 'receipt.json'
        self.receipt.write_text(json.dumps(dict(schema_version=1, kind='diagnostic-candidate-build',
            result='built', root=str(self.root), build_directory=str(self.build),
            candidate_sha256=receipt.digest(self.candidate), inputs=receipt.snapshot(self.root, self.build), compilation=receipt.compiled_inputs(self.root, self.build))))

    def validate(self):
        with patch.object(receipt, 'require_x86_pe'):
            return receipt.validate_receipt(self.receipt, self.root, self.candidate, [self.source])

    def test_unchanged_bound_inputs_validate(self):
        self.assertEqual(self.validate()['result'], 'built')

    def test_changed_source_cannot_be_approved_by_a_newer_executable_timestamp(self):
        self.source.write_text('int native() { return 2; }\n')
        self.candidate.touch()
        with self.assertRaisesRegex(ValueError, '(compiler|compilation) inputs differ'):
            self.validate()

    def test_changed_executable_bytes_fail(self):
        self.candidate.write_bytes(b'other executable')
        with self.assertRaisesRegex(ValueError, 'matching diagnostic build receipt'):
            self.validate()

    def test_new_compiled_source_invalidates_receipt(self):
        (self.source.parent / 'new.cpp').write_text('int added();\n')
        with self.assertRaisesRegex(ValueError, '(compiler|compilation) inputs differ'):
            self.validate()

    def test_changed_compiler_invalidates_receipt(self):
        self.compiler.write_bytes(b'other compiler')
        with self.assertRaisesRegex(ValueError, '(compiler|compilation) inputs differ'):
            self.validate()

    def test_arbitrary_pe_cannot_receive_a_new_build_receipt(self):
        unrelated = self.build / 'unrelated.exe'
        unrelated.write_bytes(b'stale PE')
        argv = ['candidate_build_receipt.py', '--root', str(self.root), '--build-dir', str(self.build),
                '--candidate', str(unrelated)]
        with patch.object(sys, 'argv', argv), patch.object(receipt.subprocess, 'run') as build:
            with self.assertRaisesRegex(ValueError, 'configured open-igi target output'):
                receipt.main()
        build.assert_not_called()

    def test_source_inventory_membership_does_not_prove_target_compilation(self):
        uncompiled = self.source.parent / 'uncompiled.cpp'
        uncompiled.write_text('int uncompiled();')
        report = json.loads(self.receipt.read_text())
        report['inputs'] = receipt.snapshot(self.root, self.build)
        self.receipt.write_text(json.dumps(report))
        with patch.object(receipt, 'require_x86_pe'):
            with self.assertRaisesRegex(ValueError, 'required source is absent'):
                receipt.validate_receipt(self.receipt, self.root, self.candidate, [uncompiled])


if __name__ == '__main__':
    unittest.main()
