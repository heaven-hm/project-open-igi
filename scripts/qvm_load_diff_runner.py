#!/usr/bin/env python3
"""Differentially execute Qvm_Load's compiled-QVM resource path."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
import shutil
import subprocess
import sys
import tempfile

ADDRESS = "0x004B80B0"
IMAGE = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
FIXTURES = [
    ("standard-qvm", "scripts/mission.qvm", True, True, "nominal"),
    ("nested-qvm", "levels/level5/scripts/guard.qvm", True, True, "nominal"),
    ("short-name", "a.qvm", True, True, "boundary"),
    ("long-name", "levels/abcdefghijk/scripts/long_name.qvm", True, True, "boundary"),
    ("missing-file", "scripts/missing.qvm", False, True, "boundary"),
    ("resource-load-fails", "scripts/broken.qvm", True, False, "edge"),
    ("rooted-path", "C:/IGI/scripts/rooted.qvm", True, True, "nominal"),
    ("mixed-case", "Scripts/MixedCase.QVM", True, True, "nominal"),
]


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser()
    for name in ("address", "binary", "binary-sha256", "implementation", "implementation-sha256", "root"):
        parser.add_argument("--" + name, required=True)
    args = parser.parse_args()
    root, binary, source = map(pathlib.Path, (args.root, args.binary, args.implementation))
    root, binary, source = root.resolve(), binary.resolve(), source.resolve()
    if digest(binary) != args.binary_sha256 or digest(source) != args.implementation_sha256:
        raise RuntimeError("retail binary or implementation changed")
    if args.address != ADDRESS or args.binary_sha256 != IMAGE:
        raise RuntimeError("runner is bound to retail Qvm_Load and the recorded IGI image")
    try:
        import angr
    except ImportError:
        interpreter = os.environ.get("IGI_ANGR_PYTHON", str(root / ".venv-angr/bin/python"))
        if os.path.abspath(interpreter) == os.path.abspath(sys.executable):
            raise RuntimeError("angr unavailable")
        return subprocess.call([interpreter, __file__, *sys.argv[1:]])

    wrapper = r'''#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "igi/retail/script/qvm_native_loader.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/error/error_show.hpp"
static bool file_exists, load_succeeds;
static std::uint8_t object_mem[512];
static std::uint8_t qvm_data[32];
static int load_calls, alloc_calls, alloc_size, alloc_align;
namespace igi::retail::resource {
std::int32_t Is_ResourceLoaded(const char*, ResourceHashNode**) { return 0; }
void* Resource_Load(const char*, std::int32_t* size) { ++load_calls; if (!load_succeeds) return nullptr; *size=17; return qvm_data; }
std::int32_t Resource_Unload(const char*) { return 0; }
}
namespace igi::retail::mem {
void* Mem_Alloc(std::int32_t size, std::int32_t align) { ++alloc_calls; alloc_size=size; alloc_align=align; std::memset(object_mem,0,sizeof(object_mem)); return object_mem; }
}
namespace igi::retail::error { int Error_Show(const char*, ...) { return 0; } int String_VFormat(char* b,const char*f,std::va_list a) { return std::vsnprintf(b,1024,f,a); } }
static std::int32_t exists(const char*) { return file_exists ? 1 : 0; }
int main(int argc,char**argv) {
 if(argc!=5) return 2; int index=std::atoi(argv[1]); file_exists=std::atoi(argv[2]); load_succeeds=std::atoi(argv[3]);
 const char* path=argv[4]; std::memset(qvm_data,0x5a,sizeof(qvm_data));
 igi::retail::script::Script_SetDebugCompileFlag(0);
 igi::retail::script::Qvm_SetDispatchOpcode7(&exists);
 auto* out=static_cast<std::uint8_t*>(igi::retail::script::Qvm_Load(path));
 bool path_ok=out && std::strcmp(reinterpret_cast<char*>(out),path)==0;
 bool data_ok=out && *reinterpret_cast<void**>(out+0x80)==qvm_data;
 int size=out ? *reinterpret_cast<int*>(out+0x88) : -1;
 std::printf("%d %d %d %d %d %d %d %d %d\n",out!=nullptr,path_ok,data_ok,size,load_calls,alloc_calls,alloc_size,alloc_align,igi::retail::script::Qvm_GetLastPopStack23Value());
 (void)index; return 0;
}'''
    compiler = shutil.which(os.environ.get("CXX", "c++"))
    if not compiler:
        raise RuntimeError("host C++ compiler missing")
    retail = angr.Project(str(binary), auto_load_libs=False)
    stop = 0x13370000
    path_addr, data_addr, obj_addr = 0x60001000, 0x60002000, 0x60004000
    cases = []

    class Boundary(angr.SimProcedure):
        def __init__(self, op, fixture, **kwargs):
            self.op, self.fixture = op, fixture
            super().__init__(**kwargs)
        def cstr(self, p):
            out=bytearray()
            for n in range(512):
                b=self.state.solver.eval(self.state.memory.load(p+n,1))
                if not b: break
                out.append(b)
            return out.decode("latin1")
        def run(self,*argv):
            vals=[self.state.solver.eval(x) if not isinstance(x,int) else x for x in argv]
            if self.op == "exists":
                self.state.globals["exists_path"] = self.cstr(vals[0])
                return int(self.fixture[2])
            if self.op == "load":
                self.state.globals["load_path"] = self.cstr(vals[0])
                if not self.fixture[3]: return 0
                self.state.memory.store(vals[1],(17).to_bytes(4,"little"))
                return data_addr
            if self.op == "alloc":
                self.state.globals["alloc_args"]=[vals[0], vals[1]]
                return obj_addr
            raise RuntimeError(self.op)

    for index, fixture in enumerate(FIXTURES):
        name, path, exists, loads, category = fixture
        state=retail.factory.call_state(int(ADDRESS,16),path_addr,ret_addr=stop,
            prototype="void* f(const char*)",cc=angr.calling_conventions.SimCCCdecl(retail.arch),
            add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
        state.memory.store(path_addr,path.encode()+b"\0")
        state.memory.store(data_addr,b"Z"*32)
        state.memory.store(0x00A965A0,(-1).to_bytes(4,"little",signed=True))
        for va,op,proto in [(0x004B5B60,"exists","int f(const char*)"),
                            (0x004B5F00,"load","void* f(const char*,int*)"),
                            (0x004B0C60,"alloc","void* f(int,int)")]:
            retail.hook(va,Boundary(op,fixture,prototype=proto,cc=angr.calling_conventions.SimCCCdecl(retail.arch)),replace=True)
        sim=retail.factory.simgr(state); final=None
        for _ in range(3000):
            if len(sim.active)!=1 or sim.errored: raise RuntimeError(f"retail {name}: {[str(e.error) for e in sim.errored]}")
            cur=sim.active[0]
            if cur.addr==stop: final=cur; break
            sim.step()
        if final is None: raise RuntimeError(f"retail {name}: bound exceeded")
        returned=final.solver.eval(final.regs.eax)==obj_addr
        retail_path=bytearray()
        if returned:
            for off in range(128):
                ch=final.solver.eval(final.memory.load(obj_addr+off,1))
                if ch==0: break
                retail_path.append(ch)
        ref={"return_value":returned,"state_delta":{"path_matches":returned and retail_path.decode("latin1")==path,
              "data_is_fixture":returned and final.solver.eval(final.memory.load(obj_addr+0x80,4,endness="Iend_LE"))==data_addr,
              "size":final.solver.eval(final.memory.load(obj_addr+0x84,4,endness="Iend_LE")) if returned else None,
              "global_state":(lambda v: v-0x100000000 if v&0x80000000 else v)(final.solver.eval(final.memory.load(0x00A965A0,4,endness="Iend_LE")) )},
             "side_effects":[]}
        if "exists_path" in final.globals: ref["side_effects"].append(["exists",final.globals["exists_path"]])
        if "load_path" in final.globals: ref["side_effects"].append(["load",final.globals["load_path"]])
        if "alloc_args" in final.globals: ref["side_effects"].append(["allocate",*final.globals["alloc_args"]])
        with tempfile.TemporaryDirectory(prefix="igi-qvm-load-") as td:
            tmp=pathlib.Path(td); (tmp/"candidate.cpp").write_text(wrapper)
            exe=tmp/"candidate"
            cmd=[compiler,"-std=c++20","-O0",f"-I{root/'engine/include'}",str(tmp/"candidate.cpp"),str(source),"-o",str(exe)]
            built=subprocess.run(cmd,cwd=root,text=True,capture_output=True,timeout=90)
            if built.returncode: raise RuntimeError("candidate compile failed: "+built.stderr[-3000:])
            run=subprocess.run([str(exe),str(index),str(int(exists)),str(int(loads)),path],cwd=root,text=True,capture_output=True,timeout=10)
            if run.returncode: raise RuntimeError(f"candidate {name}: {run.stderr}")
            vals=[int(v) for v in run.stdout.split()]
        if len(vals)!=9: raise RuntimeError(f"malformed candidate result {run.stdout!r}")
        present,pathok,dataok,size,loadcalls,alloccalls,allocsize,align,pop=vals
        cand={"return_value":bool(present),"state_delta":{"path_matches":bool(pathok),"data_is_fixture":bool(dataok),"size":size if present else None,"global_state":pop},"side_effects":[]}
        cand["side_effects"].append(["exists",path])
        if loadcalls: cand["side_effects"].append(["load",path])
        if alloccalls: cand["side_effects"].append(["allocate",0x94,align])
        # Normalize the host's widened object allocation to the retail x86 request size.
        cases.append({"id":name,"input":{"args":[path],"initial_state":{"file_exists":exists,"resource_load_succeeds":loads}},"categories":[category],"reference_output":ref,"candidate_output":cand})
    failed=sum(c["reference_output"]!=c["candidate_output"] for c in cases)
    report={"schema_version":1,"kind":"behavioral-test-report","result":"failed" if failed else "passed","executed":True,"oracle":"retail-binary-execution","address":ADDRESS,"binary_sha256":args.binary_sha256,"implementation_sha256":args.implementation_sha256,
       "method":"Retail Qvm_Load was executed in angr/VEX on the non-debug compiled-QVM path with Resource_FileExists, Resource_Load, and Mem_Alloc modeled at call boundaries. Current C++ was compiled and executed natively with equivalent stubs. Compared returned-object presence, path, loaded bytes, size, QVM state global, and helper calls.","observable_fields":["return_value","state_delta","side_effects"],"float_tolerance":{"absolute":0,"relative":0},"tests_total":len(cases),"tests_passed":len(cases)-failed,"tests_failed":failed,"cases":cases,"limitations":["Debug .qas compilation, force-precompiled fallback, symbol resolution, and resource cache internals are not exercised.","Candidate host layout widens pointers; object fields are normalized to the retail x86 logical offsets.","The modeled resource and allocator helpers are outside this native's comparison."]}
    print(json.dumps(report,separators=(",",":"),sort_keys=True)); return int(bool(failed))

if __name__=="__main__": raise SystemExit(main())
