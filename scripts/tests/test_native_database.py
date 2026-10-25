"""SQLite infrastructure fixtures only; no game-native behavior claims."""
import json
from pathlib import Path
import sqlite3
import sys
import tempfile
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from native_database import store_database, read_table


class VerifiedDatabase(unittest.TestCase):
    def test_only_current_complete_acceptance_survives_refresh(self):
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'verified.db'
            base=dict(address='0x00400000',name='fixture',source='fixture.cpp',cfg=1.0,
                      scoped_evidence_valid=True,current_image_accepted=True,
                      tested_candidate_sha256='current',evidence='fixture-report')
            rows=[base,dict(base,address='0x00400010',scoped_evidence_valid=False),
                  dict(base,address='0x00400020',tested_candidate_sha256='old'),
                  dict(base,address='0x00400030',current_image_accepted=False),
                  dict(base,address='0x00400040',cfg=.84)]
            report=dict(manifest_sha256='fixture',current_candidate_sha256='current',natives=rows)
            store_database(path,report)
            with sqlite3.connect(path) as db:
                self.assertEqual(db.execute('select address from natives').fetchall(),[('0x00400000',)])
                metadata=json.loads(db.execute('select report_json from snapshots').fetchone()[0])
                self.assertEqual(len(metadata['natives']),1)
            before=path.read_bytes()
            self.assertIn('fixture',read_table(path))
            self.assertEqual(before,path.read_bytes())
            report['current_candidate_sha256']='new'
            store_database(path,report)
            with sqlite3.connect(path) as db:
                self.assertEqual(db.execute('select count(*) from natives').fetchone()[0],0)
                self.assertEqual(json.loads(db.execute('select report_json from snapshots').fetchone()[0])['natives'],[])


if __name__=='__main__':unittest.main()
