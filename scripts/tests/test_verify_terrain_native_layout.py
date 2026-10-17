"""Tests for the terrain verification tool, not game-engine behavior."""
import importlib.util
from pathlib import Path
import struct
import tempfile
import unittest

specification = importlib.util.spec_from_file_location(
    'terrain_audit', Path(__file__).resolve().parents[1] / 'verify_terrain_native_layout.py')
terrain_audit = importlib.util.module_from_spec(specification)
specification.loader.exec_module(terrain_audit)


class TerrainAuditTests(unittest.TestCase):
    def grid_file(self, directory, dimension, payload):
        content = bytearray(0xC00)
        struct.pack_into('<I', content, 8, dimension)
        path = Path(directory) / 'grid.dat'
        path.write_bytes(content + payload)
        return path

    def test_optional_heightmap(self):
        self.assertEqual(terrain_audit.grid_table(None, True)['status'], 'optional_absent')

    def test_short_grid_uses_retail_fallback(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'short.dat'
            path.write_bytes(b'\0' * 12)
            self.assertEqual(terrain_audit.grid_table(path, False)['status'], 'retail_zero_table_fallback')

    def test_height_grid_has_corner_row_and_column(self):
        with tempfile.TemporaryDirectory() as directory:
            path = self.grid_file(directory, 2, b'\x40' * 9)
            report = terrain_audit.grid_table(path, True)
            self.assertEqual(report['status'], 'exact')
            self.assertEqual(report['occupied'], 1)

    def test_bit_grid_preserves_native_truncation(self):
        with tempfile.TemporaryDirectory() as directory:
            path = self.grid_file(directory, 3, b'\0')
            self.assertEqual(terrain_audit.grid_table(path, False)['status'], 'exact')

    def test_truncated_grid_is_reported(self):
        with tempfile.TemporaryDirectory() as directory:
            path = self.grid_file(directory, 8, b'\0')
            self.assertEqual(terrain_audit.grid_table(path, True)['status'], 'outside_source_bytes')

    def test_contour_command_bounds(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'TERRAIN.CTR').write_bytes(b'\0' * 32)
            (root / 'TERRAIN.CMD').write_bytes(b'\0' * 8)
            report = terrain_audit.audit_level(root)
            self.assertEqual(report['invalid_offsets'], 0)
            self.assertEqual(report['invalid_command_blocks'], 0)
            contour = bytearray(32)
            struct.pack_into('<I', contour, 28, 8)
            (root / 'TERRAIN.CTR').write_bytes(contour)
            self.assertEqual(terrain_audit.audit_level(root)['invalid_offsets'], 1)


if __name__ == '__main__':
    unittest.main()
