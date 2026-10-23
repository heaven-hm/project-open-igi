#!/usr/bin/env python3
"""Compare retail texture parsers with x86 candidates at explicit I/O boundaries."""
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
    targets={"0x004B24C0":"Texture_LoadLoopTextureFile","0x005175A0":"Terrain_LoadTexturePackage",
             "0x004CF790":"DynObj_LoadModelTextures"}
    if args.address not in targets: raise ValueError("unsupported native")
    binary,source=pathlib.Path(args.binary),pathlib.Path(args.implementation)
    if digest(binary)!=args.binary_sha256 or digest(source)!=args.implementation_sha256: raise RuntimeError("source changed")
    compiler=shutil.which("i686-w64-mingw32-g++")
    if not compiler: raise RuntimeError("x86 compiler missing")
    wrapper_text=r'''
#include "igi/retail/texture/texture_loaders.hpp"
#include "igi/retail/error/error_show.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"
#include <cstdarg>
extern "C" {
__declspec(dllexport) void* NativeAllocate(int,int) {return nullptr;}
__declspec(dllexport) void* NativeLoad(const char*,int*) {return nullptr;}
__declspec(dllexport) int NativeUnload(const char*) {return 0;}
__declspec(dllexport) int NativeError(const char*,const char*) {return 0;}
__declspec(dllexport) int NativeFormat(char*,const char*,const char*) {return 0;}
}
namespace igi::retail::mem {void* Mem_Alloc(int size,int align) {return NativeAllocate(size,align);}}
namespace igi::retail::resource {
void* Resource_Load(const char* path,int* size) {return NativeLoad(path,size);}
int Resource_Unload(const char* path) {return NativeUnload(path);}
}
namespace igi::retail::error {
int Error_Show(const char* fmt,...) {std::va_list ap; va_start(ap,fmt); auto path=va_arg(ap,const char*); va_end(ap); return NativeError(fmt,path);}
int sprintf(char* out,const char* fmt,...) {std::va_list ap; va_start(ap,fmt); auto name=va_arg(ap,const char*); va_end(ap); return NativeFormat(out,fmt,name);}
}
extern "C" __declspec(dllexport) int NativeLoop(int* out,void* pal,const char* path) {return igi::retail::texture::Texture_LoadLoopTextureFile(out,pal,path);}
extern "C" __declspec(dllexport) int* NativeTerrain(const char* path) {return igi::retail::texture::Terrain_LoadTexturePackage(path);}
extern "C" __declspec(dllexport) void* NativeModels(void* ctx) {return igi::retail::texture::DynObj_LoadModelTextures(ctx);}
'''
    base,file,heap,path_pointer=0x50000000,0x50001000,0x50008000,0x50000400
    if args.address=="0x004B24C0":
        fixtures=[{"version":version,"seed":seed} for version in (2,11) for seed in (1,2,0x8000,0xFFFF)]
        fixtures += [{"version":2,"seed":1,"bad":True},{"version":0,"seed":1}]
    elif args.address=="0x005175A0":
        fixtures=[{"version":version,"palette":palette,"count":count} for version in (7,9)
                  for palette in (False,True) for count in (0,2)]
        fixtures += [{"version":0,"palette":False,"count":0},{"version":7,"palette":False,"count":0,"missing":True}]
    else:
        fixtures=[{"count":count} for count in (-1,0,1,2,3,4,5,7)]
    retail=angr.Project(str(binary),auto_load_libs=False); observations=[]
    with tempfile.TemporaryDirectory(prefix="igi-texture-loaders-") as temp:
        wrapper=pathlib.Path(temp)/"entry.cpp"; candidate=pathlib.Path(temp)/"candidate.dll"
        wrapper.write_text(wrapper_text)
        command=[compiler,"-std=c++20","-O0","-shared",f"-I{root/'engine/include'}",str(wrapper),str(source),
                 str(root/"engine/src/retail/dynobj/hash_lookup.cpp"),"-o",str(candidate)]
        built=subprocess.run(command,capture_output=True,text=True,timeout=90)
        if built.returncode: raise RuntimeError(built.stderr)
        rebuilt=angr.Project(str(candidate),auto_load_libs=False)
        if retail.arch.bits!=32 or rebuilt.arch.bits!=32: raise RuntimeError("expected x86 images")
        candidate_flag=next((s.rebased_addr for s in rebuilt.loader.symbols if "g_943b3c" in s.name),None)
        if candidate_flag is None: raise RuntimeError("candidate upload-state symbol missing")

        def make_data(fixture):
            if args.address=="0x004CF790":
                count=fixture["count"]
                payload=(count&0xFFFFFFFF).to_bytes(4,"little")+b"".join(f"tex{i}".encode()+b"\x00" for i in range(max(0,count)))
                chunks=b"TEXF"+len(payload).to_bytes(4,"big")+payload+bytes((-len(payload))%4)
                return b"FORM"+(4+len(chunks)).to_bytes(4,"big")+b"TEST"+chunks
            raw=bytearray(4096); raw[:4]=b"BADD" if fixture.get("bad") else b"LOOP"
            put=lambda offset,value,width=4: raw.__setitem__(slice(offset,offset+width),(value&((1<<(width*8))-1)).to_bytes(width,"little"))
            version=fixture["version"]; put(4,version)
            if args.address=="0x004B24C0":
                for offset in (8,16,18,20,22): put(offset,fixture["seed"]+(offset%3),2)
                if version==11:
                    put(8,0x12345678); put(12,0x01234567)
                    for offset,value in ((20,0x8001),(22,0xFFFE),(24,17),(26,3),(30,5)): put(offset,value,2)
            else:
                put(18,0x8001,2); put(24,0x300 if fixture["palette"] else 0); put(32,fixture["count"])
                if fixture["palette"]: put(0x30C,4)
                for index in range(fixture["count"]):
                    offset=0x34+index*(40 if version==7 else 32)
                    put(offset,0x600+index*0x80)
                    if version==7:
                        for delta,value in ((4,8+index),(8,16+index),(12,2+index)): put(offset+delta,value)
                    else:
                        put(offset+4,0x12340000+index)
                        for delta,value in ((8,8+index),(10,16+index),(12,2+index)): put(offset+delta,value,2)
            return bytes(raw)

        def run(project,reference,fixture,data):
            flag_address=0x00943B3C if reference else candidate_flag
            def text(state,pointer):
                result=bytearray()
                for offset in range(256):
                    byte=state.solver.eval(state.memory.load(pointer+offset,1))
                    if byte==0: return bytes(result)
                    result.append(byte)
                raise RuntimeError("unterminated text fixture")
            def event(state,value): state.globals["events"]=tuple(state.globals.get("events",()))+(value,)
            class Allocate(angr.SimProcedure):
                def run(self,size,alignment):
                    size=self.state.solver.eval(size); alignment=self.state.solver.eval(alignment)
                    count=int(self.state.globals.get("allocation_count",0)); result=heap+count*0x1000
                    if size>0x1000: raise RuntimeError("allocation exceeds fixture")
                    self.state.memory.store(result,b"\xa5"*size); self.state.globals["allocation_count"]=count+1
                    event(self.state,["allocate",size,alignment]); return result
            class Load(angr.SimProcedure):
                def run(self,path,size):
                    event(self.state,["load",text(self.state,path).decode(),self.state.solver.eval(size)])
                    return 0 if fixture.get("missing") else file
            class Unload(angr.SimProcedure):
                def run(self,path):
                    event(self.state,["unload",text(self.state,path).decode(),self.state.solver.eval(self.state.memory.load(flag_address,1))])
                    return 0x1234
            class Error(angr.SimProcedure):
                def run(self,fmt,path):
                    event(self.state,["error",text(self.state,fmt).decode(),text(self.state,path).decode()])
                    # Error_Show is a fatal retail path.  Returning a value here
                    # lets the candidate unwind while the retail body reaches
                    # its deliberate self-loop, creating a harness-only diff.
                    self.state.globals["fatal_error"] = True
                    return 0x5678
            class Format(angr.SimProcedure):
                def run(self,out,fmt,name):
                    if text(self.state,fmt)!=b"LOCAL:textures/%s.tex": raise RuntimeError("unexpected texture path format")
                    value=b"LOCAL:textures/"+text(self.state,name)+b".tex"; self.state.memory.store(out,value+b"\x00")
                    event(self.state,["path",value.decode()]); return len(value)
            class Upload(angr.SimProcedure):
                def run(self,a,b,c,d,e,f,g,h,i):
                    values=[self.state.solver.eval(value) for value in (a,b,c,d,e,f,g,h,i)]
                    event(self.state,["upload",*values,self.state.solver.eval(self.state.memory.load(flag_address,1))])
                    self.state.memory.store(a,(7).to_bytes(4,"little")); return 0x55
            class Palette(angr.SimProcedure):
                def run(self,a,b,c):
                    event(self.state,["palette",*[self.state.solver.eval(v) for v in (a,b,c)]])
                    self.state.memory.store(a,(7).to_bytes(4,"little")); return 0x66
            class Loop(angr.SimProcedure):
                def run(self,a,b,path):
                    event(self.state,["loop",self.state.solver.eval(a),self.state.solver.eval(b),text(self.state,path).decode(),
                        self.state.solver.eval(self.state.memory.load(b+4,4,endness="Iend_LE"))])
                    self.state.memory.store(a,(7).to_bytes(4,"little")); return 0
            hooks=[(0x004B0C60,"NativeAllocate",Allocate,"void* f(int,int)"),
                   (0x004B5F00,"NativeLoad",Load,"void* f(char*,int*)"),
                   (0x004B6380,"NativeUnload",Unload,"int f(char*)"),
                   (0x004AF7B0,"NativeError",Error,"int f(char*,char*)"),
                   (0x004A53B3,"NativeFormat",Format,"int f(char*,char*,char*)"),
                   (0x004B23B0,"Texture_UploadMipmapPacket",Upload,"int f(int*,void*,int,int,int,int,int,int,void*)"),
                   (0x004B7CF0,"Texture_UploadPalettePacket",Palette,"int f(int*,void*,int)")]
            if args.address=="0x004CF790": hooks.append((0x004B24C0,"Texture_LoadLoopTextureFile",Loop,"int f(int*,void*,char*)"))
            for address,symbol,procedure,prototype in hooks:
                if not reference:
                    found=next((s for s in rebuilt.loader.symbols if symbol in s.name),None)
                    if found is None: raise RuntimeError("candidate hook missing: "+symbol)
                    address=found.rebased_addr
                project.hook(address,procedure(prototype=prototype,cc=angr.calling_conventions.SimCCCdecl(project.arch)),replace=True)
            exports={"0x004B24C0":"NativeLoop","0x005175A0":"NativeTerrain","0x004CF790":"NativeModels"}
            address=int(args.address,16) if reference else rebuilt.loader.find_symbol(exports[args.address]).rebased_addr
            arguments=([base+0x100,base+0x180,path_pointer] if args.address=="0x004B24C0" else
                       [path_pointer] if args.address=="0x005175A0" else [base])
            prototype="int f(int*,void*,char*)" if args.address=="0x004B24C0" else "void* f(void*)"
            state=project.factory.call_state(address,*arguments,ret_addr=0x13370000,prototype=prototype,
                cc=angr.calling_conventions.SimCCCdecl(project.arch),
                add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
            state.memory.store(base,b"\xa5"*0x1000); state.memory.store(path_pointer,b"LOCAL:fixture.tex\x00")
            state.memory.store(base+8,file.to_bytes(4,"little")); state.memory.store(file,data)
            state.memory.store(flag_address,b"\x00")
            manager=project.factory.simgr(state); final,termination=None,None
            for _ in range(10000):
                if len(manager.active)!=1 or manager.errored: raise RuntimeError(f"texture execution failed: {[str(x.error) for x in manager.errored]}")
                active=manager.active[0]
                if active.globals.get("fatal_error",False): final,termination=active,"fatal-self-loop"; break
                if active.addr==0x13370000: final,termination=active,"returned"; break
                if active.solver.eval(active.memory.load(active.addr,2),cast_to=bytes)==b"\xeb\xfe": final,termination=active,"fatal-self-loop"; break
                manager.step()
            if final is None: raise RuntimeError("texture execution exceeded bound")
            memory={"termination":termination,"upload_flag":final.solver.eval(final.memory.load(flag_address,1)),
                "context":final.solver.eval(final.memory.load(base,0x1000),cast_to=bytes).hex(),
                "file":final.solver.eval(final.memory.load(file,len(data)),cast_to=bytes).hex()}
            allocations=[event for event in final.globals.get("events",()) if event[0]=="allocate"]
            for index,item in enumerate(allocations): memory[f"allocation-{index}"]=final.solver.eval(final.memory.load(heap+index*0x1000,item[1]),cast_to=bytes).hex()
            return {"return_value":final.solver.eval(final.regs.eax) if termination=="returned" else None,
                    "state_delta":memory,"side_effects":list(final.globals.get("events",()))}

        for index,fixture in enumerate(fixtures):
            data=make_data(fixture)
            observations.append({"id":f"fixture-{index}","input":fixture,
                "categories":["boundary" if fixture.get("count",1)<=0 or fixture.get("version",2)==0 or fixture.get("bad") or fixture.get("missing") else "nominal"],
                "reference_output":run(retail,True,fixture,data),"candidate_output":run(rebuilt,False,fixture,data)})
        candidate_hash=digest(candidate)
    failed=sum(case["reference_output"]!=case["candidate_output"] for case in observations)
    report={"schema_version":1,"kind":"behavioral-test-report","result":"failed" if failed else "passed",
        "executed":True,"oracle":"retail-binary-execution","address":args.address,
        "binary_sha256":args.binary_sha256,"implementation_sha256":args.implementation_sha256,
        "method":"Retail and compiled PE32 texture parser bodies execute on identical generated LOOP/FORM records; compare extracted upload arguments, allocations, context bytes, upload state, callee order and fatal termination.",
        "observable_fields":["return_value","state_delta","side_effects"],"float_tolerance":{"absolute":0,"relative":0},
        "tests_total":len(observations),"tests_passed":len(observations)-failed,"tests_failed":failed,"cases":observations,
        "retail_execution":{"tool":"angr","engine":"VEX","version":angr.__version__,"function_entry":args.address,
            "candidate_sha256":candidate_hash,"candidate_build_command":command,"binary_sha256":args.binary_sha256},
        "limitations":["VFS, allocator, upload-device callbacks, formatting and fatal-error callees are explicit boundary models.",
            "Generated bounded records do not prove GPU rendering, all malformed-input paths, allocation failure or every original asset."]}
    print(json.dumps(report)); return int(bool(failed))


if __name__=="__main__": raise SystemExit(main())
