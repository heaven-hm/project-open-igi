"""Bounded tool checks and retail analysis; never assign native verification status."""
from __future__ import annotations
import hashlib
import json
import os
from pathlib import Path
import subprocess
import re
import struct
import sys
from datetime import datetime, timezone

ROOT = Path(__file__).resolve().parents[1]


def config():
    return json.loads((ROOT / 'scripts/igi-workflow.json').read_text())


def run(command, timeout=60, cwd=ROOT):
    try:
        p = subprocess.run(command, cwd=cwd, text=True, capture_output=True, timeout=timeout,
                           env=dict(os.environ, LC_ALL='C', LANG='C', PYTHONDONTWRITEBYTECODE='1'))
        return dict(command=command, exit_code=p.returncode, stdout=p.stdout, stderr=p.stderr)
    except (OSError, subprocess.SubprocessError) as error:
        return dict(command=command, exit_code=1, error=str(error))


def save(report, output):
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2) + '\n')
    print(f"{report['result']}: {output}")


def doctor(output):
    c = config()
    checks = {}
    checks['binary_ninja_mcp'] = run([sys.executable, str(ROOT/'scripts/bn_mcp_call.py'), 'bn_binary_view_info'])
    checks['radare2'] = run(['r2', '-v'])
    checks['angr'] = run([os.environ.get('IGI_ANGR_PYTHON', c['angr_python']), '-c', 'import angr; print(angr.__version__)'])
    checks['frida_server'] = run([c['frida_python'], '-c',
        'import frida,json,sys; d=frida.get_device_manager().add_remote_device(sys.argv[1]); '
        'print(json.dumps([dict(pid=p.pid,name=p.name) for p in d.enumerate_processes()]))',
        os.environ.get('FRIDA_HOST', c['frida_host'])], timeout=20)
    re_agent = Path(c['re_agent_root'])
    checks['re_agent_cli'] = run(['node', str(re_agent/'scripts/rea.mjs'), '--version'])
    checks['re_agent_frida_module'] = run(['node', '--input-type=module', '-e',
        "await import('frida'); console.log('Frida JS module loaded')"], cwd=re_agent)
    checks['compiler'] = run(['i686-w64-mingw32-g++', '--version'])
    retail = Path(c['retail_exe'])
    checks['retail_identity'] = dict(exit_code=0 if retail.is_file() and hashlib.sha256(retail.read_bytes()).hexdigest()==c['retail_sha256'] else 1)
    checks['catalog'] = dict(exit_code=0 if (ROOT/'assets/IGINatives_v4.json').is_file() else 1)
    ok = all(v['exit_code']==0 for v in checks.values())
    report = dict(kind='igi-tool-health', schema_version=1, result='passed' if ok else 'blocked',
                  checked_at=datetime.now(timezone.utc).isoformat(), checks=checks,
                  limitations=['Tool readiness does not establish game parity.',
                               'Ghidra is fallback only; no function was analyzed with Ghidra.',
                               'RE-Agent CLI and its optional Frida module are checked separately; CLI health is not proof of a live MCP connection.'])
    save(report, output)
    for name, value in checks.items(): print(f"{name}: {'OK' if value['exit_code']==0 else 'BLOCKED'}")
    return 0 if ok else 1


def analyze(address, output):
    c = config()
    binary = Path(c['retail_exe'])
    if hashlib.sha256(binary.read_bytes()).hexdigest() != c['retail_sha256']:
        raise ValueError('retail identity mismatch')
    artifacts = {}
    for tool, arguments in [('bn_function_info', {'function':address}),
                            ('bn_function_decompile', {'function':address}),
                            ('bn_function_disassembly', {'function':address}),
                            ('bn_function_basic_blocks', {'function':address}),
                            ('bn_memory_read', {'address':address,'length':16})]:
        artifacts[tool] = run([sys.executable, str(ROOT/'scripts/bn_mcp_call.py'), tool, json.dumps(arguments)])
    # This is radare2 command language, never shell command interpolation.
    artifacts['radare2_cfg'] = run(['r2','-q','-2','-e','bin.relocs.apply=true','-c',
        f'af @ {address}; agfj @ {address}; q',str(binary)], timeout=120)
    helper=ROOT/'scripts/angr_analyze.py'
    requested_interpreter=Path(os.environ.get('IGI_ANGR_PYTHON',c['angr_python']))
    interpreter=requested_interpreter.resolve()
    helper_hash=hashlib.sha256(helper.read_bytes()).hexdigest()
    interpreter_hash=hashlib.sha256(interpreter.read_bytes()).hexdigest()
    artifacts['angr'] = run([str(requested_interpreter),str(helper),
                             '--binary',str(binary),'--address',address], timeout=180)
    artifacts['angr'].update(helper_sha256=helper_hash,interpreter_path=str(interpreter),interpreter_sha256=interpreter_hash)
    if hashlib.sha256(helper.read_bytes()).hexdigest()!=helper_hash or hashlib.sha256(interpreter.read_bytes()).hexdigest()!=interpreter_hash:
        artifacts['angr']['exit_code']=1
        artifacts['angr']['stderr']+='\nangr helper/interpreter changed during capture'
    ok = all(v['exit_code']==0 for v in artifacts.values())
    summary = {}
    if ok:
        try:
            summary = summarize_analysis(artifacts, binary, address, c['retail_sha256'])
            ok = summary['cfg_similarity'] >= 0.85 and summary['angr_exact_entry']
        except (ValueError, KeyError, TypeError, struct.error) as error:
            ok = False
            summary = {'error': str(error)}
    report = dict(kind='igi-retail-analysis-capture',schema_version=1,
                  result='analysis_collected' if ok else 'blocked', verified=False,
                  binary_sha256=c['retail_sha256'],address=address,artifacts=artifacts,structural_summary=summary,
                  limitations=['Raw analysis capture only; no source-semantic or runtime acceptance.',
                               'Binary Ninja bytes are bound only at the sampled entry; complete function-byte/source review is still required.',
                               'Graph similarity must be reviewed separately; successful collection is not a CFG score.'])
    save(report,output)
    return 0 if ok else 1


def summarize_analysis(artifacts, binary, address, expected_hash):
    from native_gate import cfg_score, parse_cfg_graph
    normalize = lambda value: f'0x{int(value, 16) if isinstance(value,str) else value:08X}'
    table = artifacts['bn_function_basic_blocks']['stdout']
    if 'truncated: true' in table:
        raise ValueError('Binary Ninja block table was truncated')
    nodes, edges = [], []
    for line in table.splitlines():
        parts = [part.strip() for part in line.split('|')]
        if len(parts) < 7 or not parts[1].isdigit(): continue
        source = normalize(parts[2].split('-')[0])
        nodes.append({'address':source})
        for kind, target in re.findall(r'(TrueBranch|FalseBranch|UnconditionalBranch)\s*#\d+\s*(0x[0-9a-fA-F]+)-',parts[5]):
            edges.append(dict(source=source,target=normalize(target),kind='unconditional' if kind=='UnconditionalBranch' else 'conditional'))
    bn = parse_cfg_graph({'cfg':dict(nodes=nodes,edges=edges)},'live Binary Ninja')
    graphs = json.loads(artifacts['radare2_cfg']['stdout'])
    if len(graphs)!=1 or graphs[0]['addr']!=int(address,16):
        raise ValueError('radare2 did not recover the exact requested entry')
    blocks=graphs[0]['blocks']; rn=[]; re_edges=[]
    for block in blocks:
        source=normalize(block['addr']); rn.append({'address':source})
        for label in ('jump','fail'):
            target=block.get(label)
            if target not in (None,-1,0xffffffffffffffff):
                re_edges.append(dict(source=source,target=normalize(target),kind='conditional' if 'fail' in block else 'unconditional'))
    r2=parse_cfg_graph({'cfg':dict(nodes=rn,edges=re_edges)},'live radare2')
    recovery=json.loads(artifacts['angr']['stdout'])
    recovered=recovery.get('functions',{}).get(address,{})
    exact=(recovery.get('binary_sha256')==expected_hash and recovery.get('mapped_base')=='0x400000'
           and recovery.get('bits')==32 and recovered.get('function_entry')==address
           and recovered.get('result')=='pass' and recovered.get('has_unresolved_jumps') is False)
    observed=json.loads(artifacts['bn_memory_read']['stdout'])
    raw=binary.read_bytes(); pe=struct.unpack_from('<I',raw,0x3c)[0]
    if raw[pe:pe+4]!=b'PE\0\0': raise ValueError('invalid PE')
    count=struct.unpack_from('<H',raw,pe+6)[0]; optsize=struct.unpack_from('<H',raw,pe+20)[0]
    base=struct.unpack_from('<I',raw,pe+24+28)[0]; rva=int(address,16)-base; offset=None
    for index in range(count):
        section=pe+24+optsize+40*index
        virtual, size, pointer=struct.unpack_from('<III',raw,section+12)
        if virtual<=rva and rva+16<=virtual+size: offset=pointer+rva-virtual; break
    if offset is None or observed.get('bytesRead')!=16 or observed.get('hex')!=raw[offset:offset+16].hex():
        raise ValueError('Binary Ninja entry bytes do not match the selected retail image')
    return dict(cfg_similarity=cfg_score(bn,r2),binary_ninja_blocks=len(bn['nodes']),
                radare2_blocks=len(r2['nodes']),angr_exact_entry=exact,entry_bytes_match=True)
