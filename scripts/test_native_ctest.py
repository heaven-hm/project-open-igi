#!/usr/bin/env python3
"""Infrastructure regressions only; generated fixtures are never game evidence."""
import copy,json,tempfile,unittest
from pathlib import Path
from unittest.mock import patch
import native_ctest as bridge


class NativeCTestTests(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory();self.addCleanup(self.temp.cleanup)
        self.root=Path(self.temp.name);self.source=self.root/'engine/src/retail/core/frame_natives.cpp'
        self.source.parent.mkdir(parents=True);self.source.write_text('fixture implementation')
        self.address='0x00401050';manifest=self.root/'docs/fidelity/native-verification.json';manifest.parent.mkdir(parents=True)
        self.records=[{'address':self.address,'implementation':{'path':self.source.relative_to(self.root).as_posix(),'sha256':bridge.digest(self.source)}}]
        self.manifest=manifest;self.write_manifest()
    def write_manifest(self):self.manifest.write_text(json.dumps({'natives':self.records}))
    def static(self):return {'kind':'igi-retail-analysis-capture','address':self.address,'binary_sha256':'retail','result':'analysis_collected','verified':False,'structural_summary':{'cfg_similarity':1,'angr_exact_entry':True}}
    def runtime(self):return {'kind':'igi-one-native-verification','address':self.address,'binary_sha256':'retail','implementation_sha256':'source','candidate_sha256':'candidate','result':'passed','verified':True,'phases':{k:{'exit_code':0} for k in ['static_semantic_behavioral','paired_runtime_capture','typed_runtime_comparison']}}
    def test_compiled_scope_maps_normal_source_filename(self):
        d=bridge.discover(self.root,scope='compiled',sources=[str(self.source.relative_to(self.root))])
        self.assertEqual([x['address'] for x in d['natives']],[self.address]);bridge.require_scope(d)
    def test_compiled_absolute_sources_are_normalized(self):
        d=bridge.discover(self.root,scope='compiled',sources=[str(self.source.resolve())])
        self.assertEqual(d['natives'][0]['address'],self.address)
    def test_unexpanded_compiled_source_expression_is_blocked(self):
        with self.assertRaises(ValueError):bridge.discover(self.root,scope='compiled',sources=['$<TARGET_OBJECTS:child>'])
    def test_changed_scope_maps_normal_source_filename(self):
        import subprocess
        with patch.object(bridge.subprocess,'run',side_effect=[subprocess.CompletedProcess([],0,stdout=''),subprocess.CompletedProcess([],0,stdout=str(self.source.relative_to(self.root))+'\0'),subprocess.CompletedProcess([],0,stdout='')]):
            d=bridge.discover(self.root,scope='changed')
        self.assertEqual(d['natives'][0]['address'],self.address)
    def test_empty_scope_cannot_pass(self):
        with self.assertRaises(ValueError):bridge.require_scope({'natives':[],'unbound_sources':[]})
    def test_unbound_required_source_cannot_pass(self):
        with self.assertRaises(ValueError):bridge.require_scope({'natives':[{}],'unbound_sources':['engine/src/retail/new.cpp']})
    def test_duplicate_selection_rejected(self):
        with self.assertRaises(ValueError):bridge.discover(self.root,[self.address,self.address])
    def test_duplicate_manifest_rejected(self):
        self.records.append(copy.deepcopy(self.records[0]));self.write_manifest()
        with self.assertRaises(ValueError):bridge.discover(self.root,[self.address])
    def test_unknown_address_rejected(self):
        with self.assertRaises(ValueError):bridge.discover(self.root,['0x00402000'])
    def test_missing_engine_implementation_rejected(self):
        self.source.unlink()
        with self.assertRaises(ValueError):bridge.discover(self.root,[self.address])
    def test_reference_source_cannot_stand_in_for_engine(self):
        self.records[0]['implementation']['path']='scripts/reference.cpp';self.write_manifest()
        with self.assertRaises(ValueError):bridge.discover(self.root,[self.address])
    def test_stale_source_is_exposed_without_promoting_it(self):
        self.source.write_text('changed');d=bridge.discover(self.root,[self.address]);self.assertFalse(d['natives'][0]['source_hash_matches_record']);self.assertFalse(d['verified'])
    def test_valid_static_collection_is_not_acceptance(self):bridge.validate_report('static',self.static(),self.address,'source','retail')
    def test_static_cannot_claim_verified(self):
        d=self.static();d['verified']=True
        with self.assertRaises(ValueError):bridge.validate_report('static',d,self.address,'source','retail')
    def test_static_requires_exact_entry_and_cfg(self):
        for field,value in [('angr_exact_entry',False),('cfg_similarity',0.84)]:
            d=self.static();d['structural_summary'][field]=value
            with self.assertRaises(ValueError):bridge.validate_report('static',d,self.address,'source','retail')
    def test_invalid_cfg_numbers_rejected(self):
        for value in [float('nan'),float('inf'),1.1,True,None,'0.9']:
            d=self.static();d['structural_summary']['cfg_similarity']=value
            with self.assertRaises(ValueError):bridge.validate_report('static',d,self.address,'source','retail')
    def test_source_fingerprint_changes_for_new_header(self):
        before=bridge.scope_fingerprint(self.root)
        h=self.root/'engine/include/new.hpp';h.parent.mkdir(parents=True);h.write_text('changed dependency')
        self.assertNotEqual(before,bridge.scope_fingerprint(self.root))
    def test_changed_scope_blocks_shared_header_changes(self):
        import subprocess
        with patch.object(bridge.subprocess,'run',return_value=subprocess.CompletedProcess([],0,stdout='engine/include/shared.hpp\0')):
            with self.assertRaises(ValueError):bridge.discover(self.root,scope='changed')
    def test_unleased_frida_runner_blocks_before_launch(self):
        self.records[0]['verification']={'behavioral_runner':{'path':'scripts/frida_old.py'}};self.write_manifest()
        runner=self.root/'scripts/frida_old.py';runner.parent.mkdir();runner.write_text('import frida\n')
        with patch.object(bridge,'invoke') as invoke:
            value=bridge.runtime_preflight(self.root,{}, {'natives':[{'address':self.address}]})
        self.assertEqual(value['exit_code'],1);invoke.assert_not_called()

    def test_report_address_and_binary_must_match(self):
        for field in ['address','binary_sha256']:
            d=self.static();d[field]='wrong'
            with self.assertRaises(ValueError):bridge.validate_report('static',d,self.address,'source','retail')
    def test_valid_runtime_requires_all_phases(self):bridge.validate_report('runtime',self.runtime(),self.address,'source','retail','candidate')
    def test_runtime_success_flag_is_not_enough(self):
        for key in self.runtime()['phases']:
            d=self.runtime();del d['phases'][key]
            with self.assertRaises(ValueError):bridge.validate_report('runtime',d,self.address,'source','retail','candidate')
    def test_runtime_rejects_stale_source_candidate(self):
        for key in ['implementation_sha256','candidate_sha256']:
            d=self.runtime();d[key]='stale'
            with self.assertRaises(ValueError):bridge.validate_report('runtime',d,self.address,'source','retail','candidate')
    def test_failed_phase_blocks_runtime(self):
        d=self.runtime();d['phases']['typed_runtime_comparison']['exit_code']=1
        with self.assertRaises(ValueError):bridge.validate_report('runtime',d,self.address,'source','retail','candidate')
    def test_registered_scope_cannot_silently_expand(self):
        expected={'fingerprint':'same','natives':[{'address':self.address}]}
        actual={'fingerprint':'same','natives':[{'address':self.address},{'address':'0x00402000'}]}
        with self.assertRaises(ValueError):bridge.validate_expected_scope(actual,expected,'scope')
    def test_registered_scope_fingerprint_must_match(self):
        expected={'fingerprint':'old','natives':[{'address':self.address}]}
        actual={'fingerprint':'new','natives':[{'address':self.address}]}
        with self.assertRaises(ValueError):bridge.validate_expected_scope(actual,expected,'static')
    def test_per_native_probe_can_come_from_manifest(self):
        from types import SimpleNamespace
        probe=self.root/'probe.js';probe.write_text('reviewed fixture only')
        self.records[0]['verification']={'runtime_probe':{'path':'probe.js'}};self.write_manifest()
        self.assertEqual(bridge.resolve_probe(self.root,SimpleNamespace(probe=None,probe_map=None),self.address),probe.resolve())
    def test_missing_probe_is_not_skipped(self):
        from types import SimpleNamespace
        with self.assertRaises(ValueError):bridge.resolve_probe(self.root,SimpleNamespace(probe=None,probe_map=None),self.address)

    def test_live_runner_without_frida_filename_is_blocked(self):
        self.records[0]['verification']={'behavioral_runner':{'path':'scripts/native_iff_diff_runner.py'}};self.write_manifest()
        runner=self.root/'scripts/native_iff_diff_runner.py';runner.parent.mkdir();runner.write_text('import frida\ndef capture(device):\n    return device.spawn(["igi.exe"])\n')
        with patch.object(bridge,'invoke') as invoke:
            value=bridge.runtime_preflight(self.root,{}, {'natives':[{'address':self.address}]})
        self.assertEqual(value['exit_code'],1);invoke.assert_not_called()

    def test_compile_only_target_is_not_called_linked(self):
        name=str(self.source.relative_to(self.root))
        d=bridge.discover(self.root,scope='compiled',sources=['engine/platform/windows/scene_main.cpp'],compile_only_sources=[name])
        self.assertEqual(d['natives'][0]['link_state'],'compile_only')

    def test_x86_range_enforced(self):
        for x in ['-1','0x100000000']:
            with self.assertRaises(ValueError):bridge.normalize(x)

if __name__=='__main__':unittest.main()
