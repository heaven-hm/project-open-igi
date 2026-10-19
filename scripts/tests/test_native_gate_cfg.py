import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1]))
from native_gate import cfg_topology_score  # noqa: E402


class CfgTopologyTests(unittest.TestCase):
    def test_straight_line_exit_ignores_call_and_repeat_block_splitting(self):
        decompiler = {
            "nodes": {"0x00000100"},
            "edges": set(),
        }
        angr_split = {
            "nodes": {"0x00000100", "0x00000110", "0x00000120", "0x00000130"},
            "edges": {
                ("0x00000100", "0x00000110", "unknown"),
                ("0x00000110", "0x00000120", "unknown"),
                ("0x00000120", "0x00000120", "unknown"),
                ("0x00000120", "0x00000130", "unknown"),
            },
        }
        self.assertEqual(
            cfg_topology_score(
                decompiler, angr_split, "0x00000100",
                right_implicit_repeat_nodes={"0x00000120"},
            ),
            1.0,
        )

    def test_missing_straight_line_exit_reduces_similarity(self):
        returning = {"nodes": {"0x00000100"}, "edges": set()}
        looping = {
            "nodes": {"0x00000100"},
            "edges": {("0x00000100", "0x00000100", "unconditional")},
        }
        self.assertLess(cfg_topology_score(returning, looping, "0x00000100"), 1.0)

    def test_ignores_linear_block_splitting_but_preserves_branches(self):
        decompiler = {
            "nodes": {"0x00000100", "0x00000120", "0x00000130", "0x00000140"},
            "edges": {
                ("0x00000100", "0x00000120", "fallthrough"),
                ("0x00000120", "0x00000130", "conditional"),
                ("0x00000120", "0x00000140", "conditional"),
            },
        }
        angr_split = {
            "nodes": {"0x00000100", "0x00000110", "0x00000118", "0x00000120", "0x00000130", "0x00000140"},
            "edges": {
                ("0x00000100", "0x00000110", "fallthrough"),
                ("0x00000110", "0x00000118", "fallthrough"),
                ("0x00000118", "0x00000120", "fallthrough"),
                ("0x00000120", "0x00000130", "conditional"),
                ("0x00000120", "0x00000140", "conditional"),
            },
        }
        self.assertEqual(cfg_topology_score(decompiler, angr_split, "0x00000100"), 1.0)

    def test_branch_target_change_reduces_similarity(self):
        original = {
            "nodes": {"0x00000100", "0x00000120", "0x00000130", "0x00000140"},
            "edges": {
                ("0x00000100", "0x00000120", "fallthrough"),
                ("0x00000120", "0x00000130", "conditional"),
                ("0x00000120", "0x00000140", "conditional"),
            },
        }
        changed = {
            "nodes": {"0x00000100", "0x00000120", "0x00000130", "0x00000150"},
            "edges": {
                ("0x00000100", "0x00000120", "fallthrough"),
                ("0x00000120", "0x00000130", "conditional"),
                ("0x00000120", "0x00000150", "conditional"),
            },
        }
        self.assertLess(cfg_topology_score(original, changed, "0x00000100"), 1.0)

    def test_ignores_angr_repeat_instruction_self_edge_only(self):
        decompiler = {
            "nodes": {"0x00000100", "0x00000120", "0x00000140", "0x00000150", "0x00000160"},
            "edges": {
                ("0x00000100", "0x00000120", "conditional"),
                ("0x00000100", "0x00000140", "conditional"),
                ("0x00000120", "0x00000140", "unconditional"),
                ("0x00000140", "0x00000150", "conditional"),
                ("0x00000140", "0x00000160", "conditional"),
            },
        }
        angr_repeat = {
            "nodes": {"0x00000100", "0x00000120", "0x00000130", "0x00000140", "0x00000150", "0x00000160"},
            "edges": {
                ("0x00000100", "0x00000120", "conditional"),
                ("0x00000100", "0x00000140", "conditional"),
                ("0x00000120", "0x00000130", "unconditional"),
                ("0x00000130", "0x00000130", "Ijk_Boring"),
                ("0x00000130", "0x00000140", "Ijk_Boring"),
                ("0x00000140", "0x00000150", "conditional"),
                ("0x00000140", "0x00000160", "conditional"),
            },
        }
        self.assertEqual(
            cfg_topology_score(
                decompiler, angr_repeat, "0x00000100",
                right_implicit_repeat_nodes={"0x00000130"},
            ),
            1.0,
        )
        self.assertLess(cfg_topology_score(decompiler, angr_repeat, "0x00000100"), 1.0)


if __name__ == "__main__":
    unittest.main()
