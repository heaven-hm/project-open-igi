#!/usr/bin/env python3
"""Generate a native evidence ledger; never promote manifest verification status."""
import hashlib
import json
import sqlite3
import os
import tempfile
from native_test_config import load_config
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest() if path.is_file() else None


def evidence(ref):
    path = ROOT / ref.get('evidence_path', '')
    if not path.is_file() or sha(path) != ref.get('evidence_sha256'):
        raise ValueError('missing/stale evidence: ' + str(path))
    return json.loads(path.read_text())


def generate():
    doc = json.loads((ROOT / 'docs/fidelity/native-verification.json').read_text())
    config = load_config(ROOT)
    image_hash = sha(ROOT / config['candidate'])
    rows = []
    for record in doc['natives']:
        impl = record.get('implementation', {})
        v = record.get('verification', {})
        row = dict(address=record['address'], name=record['name'], source=impl.get('path'),
                   recorded_status=v.get('status', 'UNKNOWN'), cfg=v.get('cfg_similarity'),
                   scoped_evidence_valid=False, current_image_accepted=False)
        try:
            if not impl.get('path') or sha(ROOT / impl['path']) != impl.get('sha256'):
                raise ValueError('source missing or changed')
            if float(v.get('cfg_similarity', 0)) < .85:
                raise ValueError('CFG below 0.85')
            for key in ('binary_ninja', 'radare2', 'angr', 'semantic_review', 'build',
                        'runtime_tests', 'natural_runtime_observation', 'strict_focus_gate'):
                evidence(v.get(key, {}))
            accepted = evidence(v.get('acceptance', {}))
            paired = evidence(v.get('runtime_differential', {}))
            if (accepted.get('kind') != 'igi-single-native-scoped-acceptance' or
                accepted.get('result') != 'passed' or paired.get('result') != 'passed' or
                accepted.get('address') != record['address'] or
                accepted.get('implementation_sha256') != impl['sha256'] or
                accepted.get('binary_sha256') != doc['binary_sha256'] or
                paired.get('binarySha256') != doc['binary_sha256'] or
                paired.get('candidateSha256') != accepted.get('candidate_sha256')):
                raise ValueError('acceptance identity/result mismatch')
            for key in ('gate', 'typed_differential'):
                evidence(accepted.get(key, {}))
            checker = evidence(accepted.get('runtime_checker_run', accepted.get('runtime_checker', {})))
            if checker.get('result') != 'passed' or (checker.get('verified') is not True and checker.get('kind') != 'igi-single-native-comparison-recheck'):
                raise ValueError('runtime checker did not accept')
            if sha(ROOT / accepted.get('retained_image', '')) != accepted.get('candidate_sha256'):
                raise ValueError('tested executable unavailable or changed')
            row.update(scoped_evidence_valid=True, scope=accepted.get('scope'),
                       tested_candidate_sha256=accepted['candidate_sha256'],
                       current_image_accepted=image_hash is not None and image_hash == accepted['candidate_sha256'],
                       evidence=v['acceptance']['evidence_path'])
        except (ValueError, OSError, KeyError, TypeError) as error:
            row['limitation'] = str(error)
        rows.append(row)
    # Current strict worker passes enter automatically, without manually editing
    # a historical manifest status. Revalidate receipt, report and source identity.
    state_path = ROOT / 'build/native-auto/state.json'
    if state_path.is_file():
        from native_autoverify import complete_report
        from candidate_build_receipt import validate_receipt
        state = json.loads(state_path.read_text())
        for row in rows:
            job = state.get('tasks', {}).get(row['address'], {})
            try:
                path = Path(job.get('report', ''))
                if job.get('status') != 'passed' or not path.is_file() or sha(path) != job.get('report_sha256'):
                    continue
                report = json.loads(path.read_text())
                current_source_hash = sha(ROOT / row['source'])
                if job.get('source_sha256') != current_source_hash:
                    continue
                if not complete_report(report, job, config, image_hash):
                    continue
                validate_receipt(ROOT / config['build_directory'] / 'candidate-receipt.json',
                                 ROOT, ROOT / config['candidate'], [ROOT / row['source']])
                row.update(scoped_evidence_valid=True, current_image_accepted=True,
                           tested_candidate_sha256=image_hash, evidence=str(path),
                           scope=report.get('scope'))
                row.pop('limitation', None)
            except (OSError, ValueError, KeyError, TypeError):
                continue
    result = dict(kind='igi-native-evidence-ledger', schema_version=1,
                  manifest_sha256=sha(ROOT / 'docs/fidelity/native-verification.json'),
                  current_candidate_sha256=image_hash, native_count=len(rows),
                  scoped_evidence_count=sum(r['scoped_evidence_valid'] for r in rows),
                  current_image_count=sum(r['current_image_accepted'] for r in rows), natives=rows)
    out = ROOT / 'build/native-auto'
    out.mkdir(parents=True, exist_ok=True)
    (out / 'database.json').write_text(json.dumps(result, indent=2) + '\n')
    # Catalog diagnostics remain outside the verified-only SQLite database.
    store_database(out / 'natives.db', result)
    (out / 'database.md').write_text(read_table(out / 'natives.db') + '\n')
    return result


def store_database(path, result):
    """Atomically replace the verified-only DB; no pending or historical rows."""
    accepted = [r for r in result['natives']
                if r.get('scoped_evidence_valid') is True
                and r.get('current_image_accepted') is True
                and result.get('current_candidate_sha256')
                and r.get('tested_candidate_sha256') == result['current_candidate_sha256']
                and float(r.get('cfg') or 0) >= .85]
    filtered = dict(result, natives=accepted, native_count=len(accepted),
                    scoped_evidence_count=len(accepted), current_image_count=len(accepted))
    snapshot = hashlib.sha256(json.dumps(filtered, sort_keys=True).encode()).hexdigest()
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, temporary = tempfile.mkstemp(prefix='verified-', suffix='.db', dir=path.parent)
    os.close(fd)
    try:
        with sqlite3.connect(temporary) as db:
            db.executescript("""
                CREATE TABLE snapshots (
                    id TEXT PRIMARY KEY, captured_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
                    manifest_sha256 TEXT NOT NULL, candidate_sha256 TEXT, report_json TEXT NOT NULL);
                CREATE TABLE natives (
                    snapshot_id TEXT NOT NULL REFERENCES snapshots(id), address TEXT NOT NULL,
                    name TEXT NOT NULL, source TEXT, cfg REAL NOT NULL CHECK(cfg>=0.85),
                    scoped_evidence_valid INTEGER NOT NULL CHECK(scoped_evidence_valid=1),
                    current_image_accepted INTEGER NOT NULL CHECK(current_image_accepted=1),
                    tested_candidate_sha256 TEXT NOT NULL,
                    evidence_path TEXT NOT NULL, limitation TEXT, record_json TEXT NOT NULL,
                    PRIMARY KEY(snapshot_id,address));
                CREATE TABLE active_snapshot (
                    singleton INTEGER PRIMARY KEY CHECK(singleton=1),
                    snapshot_id TEXT NOT NULL REFERENCES snapshots(id));
            """)
            db.execute('INSERT INTO snapshots(id,manifest_sha256,candidate_sha256,report_json) VALUES(?,?,?,?)',
                       (snapshot, result['manifest_sha256'], result['current_candidate_sha256'], json.dumps(filtered)))
            for row in accepted:
                db.execute('INSERT INTO natives VALUES(?,?,?,?,?,?,?,?,?,?,?)',
                           (snapshot, row['address'], row['name'], row['source'], row['cfg'], 1, 1,
                            row['tested_candidate_sha256'], row['evidence'], None, json.dumps(row)))
            db.execute('INSERT INTO active_snapshot VALUES(1,?)', (snapshot,))
        os.replace(temporary, path)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)


def read_table(path, verified_only=False, address=None):
    # URI read-only plus query_only: this interface has no SQL/write option.
    with sqlite3.connect(path.resolve().as_uri() + '?mode=ro', uri=True) as db:
        db.execute('PRAGMA query_only=ON')
        sql = 'SELECT n.address,n.name,n.cfg,n.scoped_evidence_valid,n.current_image_accepted FROM natives n JOIN active_snapshot a ON n.snapshot_id=a.snapshot_id WHERE 1=1'
        args = []
        if verified_only:
            sql += ' AND n.scoped_evidence_valid=1'
        if address:
            sql += ' AND n.address=?'
            args.append(f'0x{int(address,0):08X}')
        rows = db.execute(sql + ' ORDER BY n.address', args).fetchall()
    lines = ['# Verified natives only', '',
             'Only complete static and paired-runtime acceptance for the current source/image is included. No pending or historical natives. Verification covers the documented scope; it is not universal-path proof.', '',
             '| VA | Native | CFG | Scoped evidence | Exact current image |', '|---|---|---|---|---|']
    for va, name, cfg, scoped, current in rows:
        name = name.replace('|', r'\|').replace('\n', ' ')
        lines.append(f"| {va} | {name} | {cfg} | {'yes' if scoped else 'incomplete/stale'} | {'yes' if current else 'unaccepted'} |")
    return '\n'.join(lines)


if __name__ == '__main__':
    report = generate()
    print(json.dumps({k:v for k,v in report.items() if k != 'natives'}, indent=2))
