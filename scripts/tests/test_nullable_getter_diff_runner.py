import importlib.util
from pathlib import Path
import unittest
import struct
import tempfile

SPEC = importlib.util.spec_from_file_location('nullable_getter_diff_runner', Path(__file__).resolve().parents[1] / 'nullable_getter_diff_runner.py')
runner = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(runner)


class ProfileTests(unittest.TestCase):
    def test_zero_displacement(self):
        self.assertEqual(runner.recognize_getter(bytes.fromhex('8b44240485c074038b00c333c0c3')), (0, 14))

    def test_signed_displacement(self):
        for encoded, expected in (('10', 16), ('e0', -32), ('20', 32)):
            self.assertEqual(runner.recognize_getter(bytes.fromhex('8b44240485c074048b40' + encoded + 'c333c0c3')), (expected, 15))

    def test_wrong_branch_rejected(self):
        with self.assertRaises(ValueError):
            runner.recognize_getter(bytes.fromhex('8b44240485c074038b4010c333c0c3'))

    def test_changed_instruction_rejected(self):
        for body in ('8b44240485c075048b4010c333c0c3', '8b44240485c074048b4010c331c0c3', '8b44240485c074048b4040c333c0c3'):
            with self.assertRaises(ValueError):
                runner.recognize_getter(bytes.fromhex(body))

    def test_pe_mapping_and_architecture(self):
        raw = bytearray(0x300)
        raw[:2] = b'MZ'
        struct.pack_into('<I', raw, 0x3c, 0x80)
        raw[0x80:0x84] = b'PE\0\0'
        struct.pack_into('<HH', raw, 0x84, 0x14c, 1)
        struct.pack_into('<H', raw, 0x94, 0xe0)
        struct.pack_into('<H', raw, 0x98, 0x10b)
        struct.pack_into('<I', raw, 0x98+28, 0x400000)
        struct.pack_into('<IIII', raw, 0x178+8, 0x100, 0x1000, 0x100, 0x200)
        raw[0x210:0x21f] = bytes.fromhex('8b44240485c074048b4010c333c0c3')
        with tempfile.TemporaryDirectory() as temp:
            binary = Path(temp) / 'fixture.exe'
            binary.write_bytes(raw)
            profile = runner.recognize_nullable_member_getter(binary, '0x401010')
            self.assertEqual(profile['offset'], 16)
            self.assertEqual(profile['length'], 15)
            self.assertIsNone(runner.recognize_nullable_member_getter(binary, '0x500000'))
            struct.pack_into('<H', raw, 0x84, 0x8664)
            binary.write_bytes(raw)
            self.assertIsNone(runner.recognize_nullable_member_getter(binary, '0x401010'))

    def test_symbol_injection_rejected(self):
        for symbol in ('foo();bad', 'foo\nbar', 'a::', 'a/*x*/', 'a<int>'):
            with self.assertRaises(ValueError):
                runner.validate_symbol(symbol)
        self.assertEqual(runner.validate_symbol('igi::retail::sys::Ilff_GetFormType'), 'igi::retail::sys::Ilff_GetFormType')


if __name__ == '__main__':
    unittest.main()
