import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1]))
from discover_referenced_natives import code_only, has_same_file_reference  # noqa: E402


class CodeOnlyReferenceTests(unittest.TestCase):
    def test_ignores_provenance_comments_and_literal_text(self):
        source = r'''// Human_CameraEyePosition 0x00465820
const char* note = "Human_CameraEyePosition 0x00465820";
/* Human_CameraEyePosition(0x00465820); */
'''
        cleaned = code_only(source)
        self.assertNotIn("Human_CameraEyePosition", cleaned)
        self.assertNotIn("0x00465820", cleaned)

    def test_keeps_executable_symbol_and_address_references(self):
        source = "Human_CameraEyePosition(out, human); call_native(0x00465820);"
        cleaned = code_only(source)
        self.assertIn("Human_CameraEyePosition", cleaned)
        self.assertIn("0x00465820", cleaned)


    def test_same_file_caller_is_not_hidden_by_own_definition(self):
        source = "int Native(int x) { return x; } int Caller() { return Native(3); }"
        self.assertTrue(has_same_file_reference(code_only(source), "Native"))
        self.assertFalse(has_same_file_reference("int Native(int x) { return x; }", "Native"))

    def test_own_forward_declaration_does_not_count_as_call(self):
        self.assertFalse(has_same_file_reference("int Native(int); int Native(int x) { return x; }", "Native"))

    def test_function_pointer_and_qualified_call_are_references(self):
        self.assertTrue(has_same_file_reference("table[0] = &Native;", "Native"))
        self.assertTrue(has_same_file_reference("igi::Native();", "Native"))

    def test_control_statement_prefixes_are_calls(self):
        for source in ("else Native();", "do Native(); while(flag);", "if (flag) Native();"):
            with self.subTest(source=source):
                self.assertTrue(has_same_file_reference(source, "Native"))

    def test_recursive_body_and_noexcept_definition(self):
        self.assertFalse(has_same_file_reference("int Native(int x) noexcept { return x; }", "Native"))
        self.assertTrue(has_same_file_reference("int Native(int x) { return Native(x-1); }", "Native"))


if __name__ == "__main__":
    unittest.main()
