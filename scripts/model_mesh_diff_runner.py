#!/usr/bin/env python3
"""Execute the retail and candidate model loader on generated ILFF fixtures.

Allocator, VFS, path formatting, and backend interfaces are explicit boundaries.
Chunk navigation and the model loader bodies execute as real x86 instructions.
"""
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


def digest(path): return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser=argparse.ArgumentParser()
    for key in ("address","binary","binary-sha256","implementation","implementation-sha256","root"):
        parser.add_argument("--"+key,required=True)
    args=parser.parse_args(); root=pathlib.Path(args.root).resolve()
    try: import angr
    except ImportError:
        interpreter=os.environ.get("IGI_ANGR_PYTHON",str(root/".venv-angr/bin/python"))
        if os.path.abspath(interpreter)==os.path.abspath(sys.executable): raise RuntimeError("angr missing")
        return subprocess.call([interpreter,__file__,*sys.argv[1:]])
    if args.address!="0x004CF390": raise ValueError("unsupported native")
    binary,source=pathlib.Path(args.binary),pathlib.Path(args.implementation)
    if digest(binary)!=args.binary_sha256 or digest(source)!=args.implementation_sha256:
        raise RuntimeError("binary or implementation changed")
    compiler=shutil.which("i686-w64-mingw32-g++")
    if not compiler: raise RuntimeError("x86 compiler unavailable")
    wrapper_text=r'''
#include "igi/retail/dynobj/local_models_mef.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"
#include "igi/retail/error/error_show.hpp"
#include "igi/retail/render/backend.hpp"
#include <cstdarg>
extern "C" {
__declspec(dllexport) void* NativeAllocate(int,int) {return nullptr;}
__declspec(dllexport) void* NativeLoad(const char*,int*) {return nullptr;}
__declspec(dllexport) int NativeFormat(char*,const char*,const char*) {return 0;}
__declspec(dllexport) void NativeBackend(void**) {}
}
namespace igi::retail::mem { void* Mem_Alloc(int size,int align) {return NativeAllocate(size,align);} }
namespace igi::retail::resource { void* Resource_Load(const char* path,int* size) {return NativeLoad(path,size);} }
namespace igi::retail::error {
int Game_DataSymbolLoad(char* out,const char* format,...) {
 std::va_list args; va_start(args,format); const char* name=va_arg(args,const char*); va_end(args);
 return NativeFormat(out,format,name);
}
int Error_Show(const char*,...) {return 0;}
}
extern "C" __declspec(dllexport) void* NativeEntry(const char* name) {
 igi::retail::render::Host_SetRenderBackendId(0);
 igi::retail::dynobj::Host_SetModelBackendInit(0,NativeBackend);
 return igi::retail::dynobj::DynObj_LoadModelMesh(name);
}
'''
    base,heap,name_address,callback=0x50000000,0x50008000,0x50004000,0x13371000
    fixtures=[(0,0),(1,0),(2,1),(4,2),(8,0),(16,1),(31,0),(31,3),(0,-1)]
    retail=angr.Project(str(binary),auto_load_libs=False); observations=[]
    with tempfile.TemporaryDirectory(prefix="igi-model-mesh-") as temp:
        wrapper=pathlib.Path(temp)/"entry.cpp"; candidate=pathlib.Path(temp)/"candidate.dll"
        wrapper.write_text(wrapper_text)
        command=[compiler,"-std=c++20","-O0","-shared",f"-I{root/'engine/include'}",str(wrapper),str(source),
            str(root/"engine/src/retail/sys/ilff_navigation.cpp"),str(root/"engine/src/retail/render/backend.cpp"),"-o",str(candidate)]
        built=subprocess.run(command,capture_output=True,text=True,timeout=90)
        if built.returncode: raise RuntimeError(built.stderr)
        rebuilt=angr.Project(str(candidate),auto_load_libs=False)
        if retail.arch.bits!=32 or rebuilt.arch.bits!=32: raise RuntimeError("expected x86 images")

        def make_container(mask,animations):
            version=0x3E4CCCCD if animations<0 else 0x3DCCCCCD
            chunks=[(0x4D455348,version.to_bytes(4,"little")+bytes(0xA0-4))]
            for bit,tag in enumerate((0x41545441,0x4D565458,0x504F5254,0x50544643,0x50545658)):
                if mask&(1<<bit): chunks.append((tag,(bit+1).to_bytes(4,"little")+bytes(12)))
            for index in range(animations):
                chunks.append((0x4E415854,(index+2).to_bytes(4,"little")+(index+5).to_bytes(4,"little")+bytes(16)))
            data=bytearray(20)
            for index,(tag,payload) in enumerate(chunks):
                length=16+len(payload); length=(length+3)&~3
                data+=tag.to_bytes(4,"little")+len(payload).to_bytes(4,"little")+(4).to_bytes(4,"little")
                data+=(length if index+1<len(chunks) else 0).to_bytes(4,"little")+payload
                data+=bytes((-len(data))%4)
            data[0:4]=b"ILFF"; data[4:8]=len(data).to_bytes(4,"little"); data[8:12]=(4).to_bytes(4,"little")
            data[16:20]=b"MEF "
            return bytes(data)

        def run(project,reference,data):
            def text(state,pointer):
                result=bytearray()
                for offset in range(256):
                    byte=state.solver.eval(state.memory.load(pointer+offset,1))
                    if byte==0: return bytes(result)
                    result.append(byte)
                raise RuntimeError("fixture text not terminated")
            def event(state,value): state.globals["events"]=tuple(state.globals.get("events",()))+(value,)
            class Allocate(angr.SimProcedure):
                def run(self,size,alignment):
                    size=self.state.solver.eval(size); alignment=self.state.solver.eval(alignment)
                    count=int(self.state.globals.get("allocation_count",0))
                    result=heap+count*0x1000
                    if size>0x1000: raise RuntimeError("allocation exceeds fixture")
                    self.state.memory.store(result,b"\xa5"*size)
                    self.state.globals["allocation_count"]=count+1
                    event(self.state,["allocate",size,alignment])
                    return result
            class Load(angr.SimProcedure):
                def run(self,path,size):
                    if self.state.solver.eval(size)!=0: raise RuntimeError("unexpected size output")
                    event(self.state,["load",text(self.state,path).decode()]); return base
            class Format(angr.SimProcedure):
                def run(self,out,fmt,name):
                    format_bytes=text(self.state,fmt); model=text(self.state,name)
                    if format_bytes!=b"LOCAL:models/%s.mef": raise RuntimeError("unexpected model path format")
                    value=b"LOCAL:models/"+model+b".mef"
                    self.state.memory.store(out,value+b"\x00")
                    event(self.state,["path",value.decode()]); return len(value)
            class Backend(angr.SimProcedure):
                def run(self,pair):
                    pointers=[self.state.solver.eval(self.state.memory.load(pair+offset,4,endness="Iend_LE")) for offset in (0,4)]
                    event(self.state,["backend",*pointers]); return 0
            class Error(angr.SimProcedure):
                def run(self,fmt,first,second,version,expected):
                    bits=lambda value: self.state.solver.eval(value.raw_to_bv())
                    event(self.state,["error",text(self.state,fmt).decode(),text(self.state,first).decode(),
                        text(self.state,second).decode(),hex(bits(version)),hex(bits(expected))])
                    return 0
            hooks=[(0x004B0C60,"NativeAllocate",Allocate,"void* f(int,int)"),
                   (0x004B5F00,"NativeLoad",Load,"void* f(char*,int*)"),
                   (0x004A53B3,"NativeFormat",Format,"int f(char*,char*,char*)"),
                   (callback,"NativeBackend",Backend,"void f(void**)"),
                   (0x004AF7B0,"Error_Show",Error,"int f(char*,char*,char*,double,double)")]
            for address,symbol,procedure,prototype in hooks:
                if not reference:
                    found=next((s for s in rebuilt.loader.symbols if symbol in s.name),None)
                    if found is None: raise RuntimeError("candidate boundary symbol missing: "+symbol)
                    address=found.rebased_addr
                project.hook(address,procedure(prototype=prototype,cc=angr.calling_conventions.SimCCCdecl(project.arch)),replace=True)
            address=int(args.address,16) if reference else rebuilt.loader.find_symbol("NativeEntry").rebased_addr
            state=project.factory.call_state(address,name_address,ret_addr=0x13370000,prototype="void* f(char*)",
                cc=angr.calling_conventions.SimCCCdecl(project.arch),
                add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
            state.memory.store(base,data); state.memory.store(name_address,b"fixture\x00")
            if reference:
                state.memory.store(0x00548208,(0).to_bytes(4,"little"))
                state.memory.store(0x00A94E84,callback.to_bytes(4,"little"))
            manager=project.factory.simgr(state)
            final,termination=None,None
            for _ in range(12000):
                if len(manager.active)!=1 or manager.errored:
                    raise RuntimeError(f"model execution failed: {[str(x.error) for x in manager.errored]}")
                active=manager.active[0]
                if active.addr==0x13370000:
                    final,termination=active,"returned"; break
                if active.solver.eval(active.memory.load(active.addr,2),cast_to=bytes)==b"\xeb\xfe":
                    final,termination=active,"fatal-self-loop"; break
                manager.step()
            if final is None: raise RuntimeError("model execution exceeded fixture bound")
            count=int(final.globals.get("allocation_count",0))
            memory={"termination":termination,"container":final.solver.eval(final.memory.load(base,len(data)),cast_to=bytes).hex()}
            # Compare every byte of each requested allocation, not host addresses.
            allocation_events=[item for item in final.globals.get("events",()) if item[0]=="allocate"]
            for index in range(count):
                memory[f"allocation-{index}"]=final.solver.eval(final.memory.load(heap+index*0x1000,allocation_events[index][1]),cast_to=bytes).hex()
            return {"return_value":final.solver.eval(final.regs.eax) if termination=="returned" else None,"state_delta":memory,
                    "side_effects":list(final.globals.get("events",()))}

        for mask,animations in fixtures:
            data=make_container(mask,animations)
            observations.append({"id":f"mask-{mask}-animations-{animations}","input":{"optional_mask":mask,"animations":animations},
                "categories":["boundary" if mask==0 or animations==0 else "nominal"],
                "reference_output":run(retail,True,data),"candidate_output":run(rebuilt,False,data)})
        candidate_hash=digest(candidate)
    failed=sum(case["reference_output"]!=case["candidate_output"] for case in observations)
    report={"schema_version":1,"kind":"behavioral-test-report","result":"failed" if failed else "passed",
        "executed":True,"oracle":"retail-binary-execution","address":args.address,
        "binary_sha256":args.binary_sha256,"implementation_sha256":args.implementation_sha256,
        "method":"Retail and compiled x86 loader bodies execute their actual ILFF navigation; compare optional-chunk relocations, NAXT arrays, allocated records and backend dispatch under identical boundary models.",
        "observable_fields":["return_value","state_delta","side_effects"],"float_tolerance":{"absolute":0,"relative":0},
        "tests_total":len(observations),"tests_passed":len(observations)-failed,"tests_failed":failed,"cases":observations,
        "retail_execution":{"tool":"angr","engine":"VEX","version":angr.__version__,"function_entry":args.address,
            "candidate_sha256":candidate_hash,"candidate_build_command":command,"binary_sha256":args.binary_sha256},
        "limitations":["Valid version-0.1 meshes and one incompatible-version fixture with backend zero; malformed containers and absent backend are excluded.",
            "Allocator, VFS, path formatter and backend implementations are modeled and not verified by this fixture.",
            "Generated ILFF records test loader semantics, not compatibility with every original asset or a live game run."]}
    print(json.dumps(report)); return int(bool(failed))


if __name__=="__main__": raise SystemExit(main())
