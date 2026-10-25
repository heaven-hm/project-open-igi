"""Sequential read-only model assistance for evidence-bound probe proposals.

Model output is never verification. A separate critic must approve observation
code, and strict static/typed runtime gates still determine acceptance.
"""
import json
from pathlib import Path
import re
import shutil
import subprocess


PROPOSAL={'type':'object','additionalProperties':False,'properties':{
    'address':{'type':'string'},'probe_js':{'type':'string'},'scope':{'type':'string'},
    'evidence':{'type':'array','items':{'type':'string'}},
    'unresolved':{'type':'array','items':{'type':'string'}}},
    'required':['address','probe_js','scope','evidence','unresolved']}
CRITIC={'type':'object','additionalProperties':False,'properties':{
    'approved_for_observation':{'type':'boolean'},
    'reasons':{'type':'array','items':{'type':'string'}},
    'unresolved':{'type':'array','items':{'type':'string'}}},
    'required':['approved_for_observation','reasons','unresolved']}


def ask(executable,root,folder,name,schema,prompt):
    spec=folder/(name+'-schema.json');spec.write_text(json.dumps(schema))
    output=folder/(name+'.json')
    command=[executable,'exec','--ignore-user-config','--ephemeral','--sandbox','read-only',
             '--cd',str(root),'--output-schema',str(spec),'--output-last-message',str(output),'-']
    with (folder/(name+'.log')).open('w') as log:
        result=subprocess.run(command,input=prompt,text=True,stdout=log,stderr=subprocess.STDOUT,timeout=120)
    if result.returncode or not output.is_file():raise ValueError('probe agent failed: '+name)
    return json.loads(output.read_text())


def generate_probe(root,folder,address,source,retail,candidate,candidate_va,symbols):
    executable=shutil.which('codex')
    if executable is None:raise ValueError('Codex probe-generation backend unavailable')
    folder.mkdir(parents=True,exist_ok=True)
    # Use the single selected decompiler's already collected output, never a
    # second decompiler. The worker's capture is the sibling static.json.
    capture_path=folder.parent.parent/'static.json'
    capture=json.loads(capture_path.read_text())
    bn={k:v.get('stdout','') for k,v in capture.get('artifacts',{}).items() if k.startswith('bn_')}
    assembly=subprocess.check_output(['i686-w64-mingw32-objdump','-d','-Mintel',
        '--start-address='+hex(candidate_va),'--stop-address='+hex(candidate_va+256),str(candidate)],text=True,timeout=20)
    references=[]
    for path in sorted((root/'scripts/GameNatives').rglob('*.cpp')):
        if path.name.lower().startswith(address[2:].lower()):
            references.append(dict(path=str(path.relative_to(root)),text=path.read_text(errors='replace')[:12000]))
            if len(references)>=4:break
    facts=dict(address=address,source=source.read_text(),candidate_references=references,selected_decompiler=bn,
               candidate_assembly_window=assembly,candidate_symbols=symbols,
               note='Assembly window can include following functions; preserve exact native boundaries.')
    instruction='''Return JSON only. Do not call tools, change files, invoke another agent, or invent game behavior.
Using only supplied static evidence, generate a READ-ONLY Frida JavaScript natural-call probe.
It must export rpc.exports.hook(address,layout), summary(), detach(). Relocate against
layout.moduleName and layout.preferredBase. Use layout.evidenceInput for mappings only if supplied.
Hook must return {installed:true}. Send observed entry and return payloads with the supplied
retail address and ordered callIndex; return payload must include typedState with scope,
inputs, outputs, sideEffects. Observe proven argument values, returns and bounded typed memory
contents; never compare pointer addresses across processes. Validate readable ranges before reads.
Do not invoke native functions, replace hooks, write process memory, access files/network,
or execute shell commands. No invented bindings/types/offsets/constants or missing side effects.
If exact ABI or required effects are unknown, return empty probe_js and explain unresolved gaps.
A proposal is observation tooling only and must not claim VERIFIED or alter any status.
FACTS:\n'''
    proposal=ask(executable,root,folder,'proposal',PROPOSAL,instruction+json.dumps(facts))
    if proposal.get('address')!=address or proposal.get('unresolved') or not proposal.get('evidence'):
        raise ValueError('probe agent retained unresolved semantics; see proposal.json')
    code=proposal.get('probe_js','')
    # Reject obvious active instrumentation; independent review is additional
    # evidence, not a substitute for the strict native acceptance checker.
    forbidden=r'NativeFunction|NativeCallback|Interceptor\s*\.\s*replace|\bwrite[A-Z]\w*\s*\(|\b(?:File|Socket|Worker)\b|Memory\s*\.\s*(?:patchCode|protect)|\beval\s*\(|new\s+Function'
    if not code or re.search(forbidden,code):raise ValueError('agent probe rejected: active/unsupported instrumentation')
    reviewed=ask(executable,root,folder,'critic',CRITIC,
        'Independently review this read-only Frida probe against the original facts. Do not call tools or edit files. '
        'Approve observation only if ABI, both image mappings, all observables, bounds and absence of writes '
        'are justified by evidence. Any unresolved assumption requires rejection. Never approve native parity.\n'
        +json.dumps(dict(facts=facts,proposal=proposal)))
    if reviewed.get('approved_for_observation') is not True or reviewed.get('unresolved'):
        raise ValueError('probe critic rejected proposal; see critic.json')
    return code,dict(kind='model-reviewed-observation-proposal',scope=proposal['scope'],
                     evidence=proposal['evidence'],verified=False)
