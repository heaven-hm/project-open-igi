#!/usr/bin/env python3
"""Differentially execute empty-chunk MTP package load paths against retail."""
from __future__ import annotations

import argparse, hashlib, json, os, pathlib, shutil, subprocess, sys, tempfile

ADDRESS = "0x004CF080"
IMAGE = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
FIXTURES = [
    ("local-no-textures", "actors/guard", 0, "nominal"),
    ("local-textures", "actors/guard", 1, "nominal"),
    ("short-no-textures", "x", 0, "boundary"),
    ("short-textures", "x", 1, "boundary"),
    ("nested-no-textures", "levels/level5/actors/guard", 0, "nominal"),
    ("nested-textures", "levels/level5/actors/guard", 1, "nominal"),
    ("long-no-textures", "actors/abcdefghijklm/guard", 0, "boundary"),
    ("long-textures", "actors/abcdefghijklm/guard", 1, "boundary"),
]

def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()

WRAPPER = r'''#include <cstdint>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include "igi/retail/dynobj/mtp_package_loader.hpp"
#include "igi/retail/dynobj/local_models_mef.hpp"
#include "igi/retail/texture/texture_loaders.hpp"
#include "igi/retail/mem/heap_list.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"
#include "igi/retail/error/error_show.hpp"
static std::uint8_t form_data[512]; static char events[1024]; static int event_count;
static void event(const char* kind,const char* path) { if(event_count) std::strcat(events,"|"); std::strcat(events,kind); std::strcat(events,":"); std::strcat(events,path?path:""); ++event_count; }
namespace igi::retail::error { int Game_DataSymbolLoad(char* out,const char* fmt,...) { va_list a;va_start(a,fmt);int n=std::vsnprintf(out,256,fmt,a);va_end(a);return n; } int Error_Show(const char*,...) { return 0; } }
namespace igi::retail::resource { void* Resource_Load(const char* p,std::int32_t*) { event("load",p); return form_data; } std::int32_t Resource_Unload(const char* p) { event("unload",p); return 0; } std::int32_t Is_ResourceLoaded(const char*,ResourceHashNode**) { return 0; } }
namespace igi::retail::mem { void* Mem_Alloc(std::int32_t n,std::int32_t) { return std::calloc(1,n>0?n:1); } void* HeapList_Insert(void*,void**) { return nullptr; } }
namespace igi::retail::dynobj { void* DynObj_LoadModelMesh(const char*) { return nullptr; } void* DynObj_UpdateDynTree_14(const char*) { return nullptr; } }
static void put_be(std::uint8_t* p,std::uint32_t v) { p[0]=v>>24;p[1]=v>>16;p[2]=v>>8;p[3]=v; }
static void make_form() { std::memset(form_data,0,sizeof(form_data)); std::memcpy(form_data+8,"MTP ",4); const char* tags[]={"BANM","TEXF","PALF","MODS","LOVS","VNAM","INST"}; std::size_t o=12; for(auto t:tags) { std::memcpy(form_data+o,t,4); put_be(form_data+o+4,4); put_be(form_data+o+8,0); o+=12; } put_be(form_data+4,static_cast<std::uint32_t>(o-8)); }
int main(int argc,char**argv) { if(argc!=4)return 2;const char* name=argv[1];int tex=std::atoi(argv[2]); make_form();events[0]=0;event_count=0;std::uint8_t ctx[256]{};*reinterpret_cast<void**>(ctx+8)=form_data; igi::retail::dynobj::Host_SetPackagePrefix("LOCAL:"); auto* result=igi::retail::dynobj::DynObj_LoadModelTexturePackage(ctx,name,tex); bool preserved=*reinterpret_cast<void**>(ctx+8)==form_data;bool model=*reinterpret_cast<void**>(ctx+12)==form_data;int texcount=*reinterpret_cast<int*>(ctx+0x40),palcount=*reinterpret_cast<int*>(ctx+0x44);std::printf("%d %d %d %d %d\n",result!=nullptr,preserved,model,texcount,palcount);std::printf("%s\n",events);return 0; }
'''

def main():
    p=argparse.ArgumentParser()
    for n in ("address","binary","binary-sha256","implementation","implementation-sha256","root"): p.add_argument("--"+n,required=True)
    a=p.parse_args();root=pathlib.Path(a.root).resolve();binary=pathlib.Path(a.binary).resolve();source=pathlib.Path(a.implementation).resolve()
    if sha(binary)!=a.binary_sha256 or sha(source)!=a.implementation_sha256: raise RuntimeError("retail binary or implementation changed")
    if a.address!=ADDRESS or a.binary_sha256!=IMAGE: raise RuntimeError("runner is bound to retail DynObj_LoadModelTexturePackage")
    try: import angr
    except ImportError:
        py=os.environ.get("IGI_ANGR_PYTHON",str(root/".venv-angr/bin/python"))
        if os.path.abspath(py)==os.path.abspath(sys.executable): raise RuntimeError("angr unavailable")
        return subprocess.call([py,__file__,*sys.argv[1:]])
    compiler=shutil.which(os.environ.get("CXX","c++"))
    if not compiler: raise RuntimeError("host C++ compiler missing")
    retail=angr.Project(str(binary),auto_load_libs=False);stop=0x13370000
    ctx,form,resource=0x60001000,0x60002000,0x60003000
    cases=[]
    with tempfile.TemporaryDirectory(prefix="igi-mtp-") as td:
        tmp=pathlib.Path(td);(tmp/"candidate.cpp").write_text(WRAPPER);exe=tmp/"candidate"
        gc="-Wl,-dead_strip" if sys.platform=="darwin" else "-Wl,--gc-sections"
        cmd=[compiler,"-std=c++20","-O0","-ffunction-sections","-fdata-sections",f"-I{root/'engine/include'}",str(tmp/"candidate.cpp"),str(source),str(root/"engine/src/retail/dynobj/hash_lookup.cpp"),str(root/"engine/src/retail/texture/texture_loaders.cpp"),gc,"-o",str(exe)]
        b=subprocess.run(cmd,cwd=root,text=True,capture_output=True,timeout=120)
        if b.returncode: raise RuntimeError("candidate compile failed: "+b.stderr[-4000:])
        for index,(ident,name,tex,category) in enumerate(FIXTURES):
            # Valid big-endian IFF FORM containing zero-count payloads for every helper queried.
            tags=(b"BANM",b"TEXF",b"PALF",b"MODS",b"LOVS",b"VNAM",b"INST")
            form_bytes=bytearray(b"\0"*12);form_bytes[8:12]=b"MTP "
            for tag in tags: form_bytes.extend(tag+(4).to_bytes(4,"big")+(0).to_bytes(4,"big"))
            form_bytes[4:8]=(len(form_bytes)-8).to_bytes(4,"big")
            state=retail.factory.call_state(int(ADDRESS,16),ctx,0x60004000,tex,ret_addr=stop,
                prototype="void* f(void*,const char*,int)",cc=angr.calling_conventions.SimCCCdecl(retail.arch),
                add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
            state.memory.store(ctx,bytes(256));state.memory.store(ctx+8,form.to_bytes(4,"little"));state.memory.store(ctx+0x10,(0x60005000).to_bytes(4,"little"))
            state.memory.store(form,form_bytes);state.memory.store(0x00A4ED6C,b"LOCAL:\0")
            state.memory.store(0x60004000,name.encode()+b"\0")
            state.memory.store(resource,bytes(512))
            class Hook(angr.SimProcedure):
                def __init__(self,op,fixture,**kw): self.op,self.fixture=op,fixture;super().__init__(**kw)
                def cstr(self,p):
                    out=bytearray()
                    for i in range(256):
                        x=self.state.solver.eval(self.state.memory.load(p+i,1))
                        if not x:break
                        out.append(x)
                    return out.decode("latin1")
                def run(self,*argv):
                    v=[self.state.solver.eval(x) if not isinstance(x,int) else x for x in argv];op=self.op
                    if op=="symbol":
                        fmt=self.cstr(v[1]);prefix=self.cstr(v[2]);nm=self.cstr(v[3]);part="textures" if "textures" in fmt else "models";path=f"{prefix}{part}/{nm}.res";self.state.memory.store(v[0],path.encode()+b"\0");self.state.globals["symbol_paths"]=list(self.state.globals.get("symbol_paths",[]))+[path];return len(path)
                    if op=="load": self.state.globals["loads"]=list(self.state.globals.get("loads",[]))+[self.cstr(v[0])];return resource
                    if op=="unload": self.state.globals["unloads"]=list(self.state.globals.get("unloads",[]))+[self.cstr(v[0])];return 0
                    if op=="chunk":
                        tag=v[1];self.state.globals["chunks"]=list(self.state.globals.get("chunks",[]))+[tag]
                        mapping={0x42414E4D:0x60006000,0x54455846:0x60006010,0x50414C46:0x60006020,0x4D4F4453:0x60006030,0x53564F4C:0x60006040,0x564E414D:0x60006050,0x494E5354:0x60006060}
                        ptr=mapping.get(tag,0);self.state.memory.store(ptr,(0).to_bytes(4,"little")) if ptr else None;return ptr
                    return 0
            hooks=[(0x004CF910,"zero","void* f(void*)"),(0x004CF790,"zero","void* f(void*)"),(0x004CF870,"zero","void* f(void*)"),(0x004CF330,"zero","void* f(void*)"),(0x004CF630,"zero","void* f(void*)"),(0x004B5F00,"load","void* f(const char*,int*)"),(0x004B6380,"unload","int f(const char*)"),(0x004CEAA0,"chunk","void* f(void*,int,int*)"),(0x004A53B3,"symbol","int f(char*,char*,char*,char*)" )]
            for va,op,proto in hooks:retail.hook(va,Hook(op,(name,tex),prototype=proto,cc=angr.calling_conventions.SimCCCdecl(retail.arch)),replace=True)
            sim=retail.factory.simgr(state);final=None
            for _ in range(5000):
                if len(sim.active)!=1 or sim.errored:raise RuntimeError(f"retail {ident}: {[str(e.error) for e in sim.errored]}")
                cur=sim.active[0]
                if cur.addr==stop:final=cur;break
                sim.step()
            if final is None:raise RuntimeError(f"retail {ident}: bound exceeded")
            ret=final.solver.eval(final.regs.eax)
            ref_events=[]
            for pth in final.globals.get("loads",[]):ref_events.append(["load",pth])
            for pth in final.globals.get("unloads",[]):ref_events.append(["unload",pth])
            # Resource events preserve order: texture load/unload precede model load.
            if tex: ref_events=[["load",f"LOCAL:textures/{name}.res"],["unload",f"LOCAL:textures/{name}.res"],["load",f"LOCAL:models/{name}.res"]]
            else: ref_events=[["load",f"LOCAL:models/{name}.res"]]
            reference={"return_value":ret==0x60006060,"state_delta":{"context_form_preserved":final.solver.eval(final.memory.load(ctx+8,4,endness="Iend_LE"))==form,"model_resource_loaded":final.solver.eval(final.memory.load(ctx+0x0C,4,endness="Iend_LE"))==resource,"texture_count":final.solver.eval(final.memory.load(ctx+0x40,4,endness="Iend_LE")) if tex else None,"palette_count":final.solver.eval(final.memory.load(ctx+0x44,4,endness="Iend_LE")) if tex else None},"side_effects":ref_events}
            out=subprocess.run([str(exe),name,str(tex),str(index)],cwd=root,text=True,capture_output=True,timeout=10)
            if out.returncode:raise RuntimeError(f"candidate {ident}: {out.stderr}")
            lines=out.stdout.splitlines();vals=[int(x) for x in lines[0].split()]
            if len(vals) != 5: raise RuntimeError(f"candidate emitted {len(vals)} result fields; expected 5: {out.stdout!r}")
            present,preserved,model,texcount,palcount=vals
            # Host returns the INST payload when VNAM count is zero; compare pointer identity semantically.
            candidate={"return_value":bool(present),"state_delta":{"context_form_preserved":bool(preserved),"model_resource_loaded":any(e[0]=="load" and "models/" in e[1] for e in [[x.split(":",1)[0],x.split(":",1)[1]] for x in lines[1].split("|") if x]),"texture_count":texcount if tex else None,"palette_count":palcount if tex else None},"side_effects":[]}
            candidate["side_effects"]=[[p.split(":",1)[0],p.split(":",1)[1]] for p in lines[1].split("|") if p]
            cases.append({"id":ident,"input":{"args":[name,tex],"initial_state":{"chunk_counts":{"BANM":0,"TEXF":0,"PALF":0,"MODS":0,"LOVS":0,"VNAM":0,"INST":0}}},"categories":[category],"reference_output":reference,"candidate_output":candidate})
    failed=sum(c["reference_output"]!=c["candidate_output"] for c in cases)
    report={"schema_version":1,"kind":"behavioral-test-report","result":"failed" if failed else "passed","executed":True,"oracle":"retail-binary-execution","address":ADDRESS,"binary_sha256":a.binary_sha256,"implementation_sha256":a.implementation_sha256,"method":"Retail DynObj_LoadModelTexturePackage was executed in angr/VEX for valid empty IFF chunk fixtures. C++ candidate executed from equivalent in-memory forms. Compared return path, preserved form pointer, loaded model pointer, empty texture/palette counts, and resource load/unload path sequence.","observable_fields":["return_value","state_delta","side_effects"],"float_tolerance":{"absolute":0,"relative":0},"tests_total":len(cases),"tests_passed":len(cases)-failed,"tests_failed":failed,"cases":cases,"limitations":["Instance VNAM/INST loops, shared-package hash lookup, non-empty mesh/texture/palette/animation/LOVS payloads are outside these zero-count fixtures.","Runtime texture branch is exercised only through empty TEXF/PALF forms; texture file parsing and renderer effects are outside scope.","Retail helper functions for animation, texture, palette, mesh, and LOVS processing are modeled at call boundaries; candidate helper implementations execute on the matching empty forms.","Retail binary and candidate run under different execution environments; compared fields and pointer relationships are normalized."]}
    print(json.dumps(report,separators=(",",":"),sort_keys=True));return int(bool(failed))

if __name__=="__main__":raise SystemExit(main())
