#!/usr/bin/env python3
"""Compare fatal-error dispatch with explicit formatter/callback models."""
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


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser()
    for name in ("address", "binary", "binary-sha256", "implementation", "implementation-sha256", "root"):
        parser.add_argument("--"+name,required=True)
    args=parser.parse_args()
    root=pathlib.Path(args.root).resolve()
    try:
        import angr
    except ImportError:
        interpreter=os.environ.get("IGI_ANGR_PYTHON",str(root/".venv-angr/bin/python"))
        if os.path.abspath(interpreter)==os.path.abspath(sys.executable):
            raise RuntimeError("angr unavailable")
        return subprocess.call([interpreter,__file__,*sys.argv[1:]])
    if args.address!="0x004AF7B0": raise ValueError("unsupported native")
    binary,source=pathlib.Path(args.binary),pathlib.Path(args.implementation)
    if digest(binary)!=args.binary_sha256 or digest(source)!=args.implementation_sha256:
        raise RuntimeError("binary or implementation changed")
    compiler=shutil.which("i686-w64-mingw32-g++")
    if not compiler: raise RuntimeError("x86 compiler unavailable")
    wrapper_text=r'''
#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/error/error_show.hpp"
#include <cstring>
extern "C" {
__declspec(dllexport) char* NativeBufferAddress() { return igi::retail::error::Error_GetGlobals().g_a84640; }
__declspec(dllexport) void NativeDisplay(const char*) {}
__declspec(dllexport) int NativeExit() { return 0; }
__declspec(dllexport) int NativeEntry(const char* format, int value, unsigned flags) {
    auto& g=igi::retail::error::Error_GetGlobals();
    g.g_936268=(flags&1)?NativeDisplay:nullptr;
    g.g_93626c=(flags&2)?NativeExit:nullptr;
    const auto result=Debug_Error(format,value);
    return result;
}
}
'''
    retail=angr.Project(str(binary),auto_load_libs=False)
    observations=[]
    with tempfile.TemporaryDirectory(prefix="igi-error-show-") as temp:
        wrapper=pathlib.Path(temp)/"entry.cpp"
        candidate=pathlib.Path(temp)/"candidate.dll"
        wrapper.write_text(wrapper_text)
        # The dispatcher is Debug_Error in retail_error.cpp; the shared error state and
        # String_VFormat live in error_show.cpp, so both units are compiled into the candidate.
        shared_state=root/"engine/src/retail/error/error_show.cpp"
        command=[compiler,"-std=c++20","-O0","-shared",f"-I{root/'engine/include'}",str(wrapper),str(source),str(shared_state),"-o",str(candidate)]
        built=subprocess.run(command,capture_output=True,text=True,timeout=90)
        if built.returncode: raise RuntimeError(built.stderr)
        rebuilt=angr.Project(str(candidate),auto_load_libs=False)
        if retail.arch.bits!=32 or rebuilt.arch.bits!=32: raise RuntimeError("expected x86 images")
        getter=rebuilt.factory.call_state(rebuilt.loader.find_symbol("NativeBufferAddress").rebased_addr,
            ret_addr=0x13370000,prototype="char* f()",
            cc=angr.calling_conventions.SimCCCdecl(rebuilt.arch),
            add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
        getter_manager=rebuilt.factory.simgr(getter)
        getter_manager.explore(find=0x13370000,n=100)
        if len(getter_manager.found)!=1: raise RuntimeError("candidate buffer getter failed")
        candidate_buffer=getter_manager.found[0].solver.eval(getter_manager.found[0].regs.eax)

        def run(project,reference,flags,fmt,value,exit_code):
            def string_at(state,pointer):
                result=bytearray()
                for offset in range(512):
                    byte=state.solver.eval(state.memory.load(pointer+offset,1))
                    if byte==0: return bytes(result)
                    result.append(byte)
                raise RuntimeError("unterminated fixture string")

            class Prefix(angr.SimProcedure):
                def run(self,destination,format_pointer):
                    text=string_at(self.state,format_pointer)
                    if text!=b"Fatal error:\n\n": raise RuntimeError("unexpected prefix format")
                    self.state.memory.store(destination,text+b"\x00")
                    self.state.globals["events"]=tuple(self.state.globals.get("events",()))+(["prefix",text.decode()],)
                    return len(text)

            class Format(angr.SimProcedure):
                def run(self,destination,format_pointer,ap):
                    format_bytes=string_at(self.state,format_pointer)
                    if format_bytes==b"literal": message=b"literal"
                    elif format_bytes==b"value=%d":
                        argument=self.state.solver.eval(self.state.memory.load(ap,4,endness="Iend_LE"))
                        signed=argument-0x100000000 if argument&0x80000000 else argument
                        message=b"value="+str(signed).encode()
                    else: raise RuntimeError("unsupported fixture format")
                    self.state.memory.store(destination,message+b"\x00")
                    self.state.globals["events"]=tuple(self.state.globals.get("events",()))+(["format",format_bytes.decode(),message.decode()],)
                    return len(message)

            class Display(angr.SimProcedure):
                def run(self,text):
                    self.state.globals["events"]=tuple(self.state.globals.get("events",()))+(["display",string_at(self.state,text).decode()],)
                    return 0

            class Exit(angr.SimProcedure):
                def run(self):
                    self.state.globals["events"]=tuple(self.state.globals.get("events",()))+(["exit",exit_code],)
                    return exit_code&0xFFFFFFFF

            hooks=[(0x004A53B3,"sprintf",Prefix,"int f(char*,char*)"),
                   (0x004A5702,"String_VFormat",Format,"int f(char*,char*,char*)"),
                   (0x004988D0,"NativeDisplay",Display,"void f(char*)"),
                   (0x00498930,"NativeExit",Exit,"int f()")]
            for address,name,procedure,prototype in hooks:
                if not reference:
                    matches=[symbol for symbol in rebuilt.loader.symbols if name in symbol.name]
                    if not matches: raise RuntimeError("candidate hook missing: "+name)
                    address=matches[0].rebased_addr
                project.hook(address,procedure(prototype=prototype,cc=angr.calling_conventions.SimCCCdecl(project.arch)),replace=True)
            address=0x004AF7B0 if reference else rebuilt.loader.find_symbol("NativeEntry").rebased_addr
            values=[0x50000000,value&0xFFFFFFFF]
            if not reference: values.append(flags)
            state=project.factory.call_state(address,*values,ret_addr=0x13370000,
                prototype="int f(char*,int)" if reference else "int f(char*,int,unsigned)",
                cc=angr.calling_conventions.SimCCCdecl(project.arch),
                add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
            state.memory.store(0x50000000,fmt.encode()+b"\x00")
            state.memory.store(0x00A84640 if reference else candidate_buffer,b"\xa5"*4096)
            if reference:
                state.memory.store(0x00936268,(0x004988D0 if flags&1 else 0).to_bytes(4,"little"))
                state.memory.store(0x0093626C,(0x00498930 if flags&2 else 0).to_bytes(4,"little"))
            manager=project.factory.simgr(state)
            manager.explore(find=0x13370000,n=3000)
            if len(manager.found)!=1 or manager.errored:
                raise RuntimeError(f"dispatch did not return: {[str(x.error) for x in manager.errored]}")
            final=manager.found[0]
            buffer=0x00A84640 if reference else candidate_buffer
            return {"return_value":final.solver.eval(final.regs.eax),
                "state_delta":{"buffer":final.solver.eval(final.memory.load(buffer,4096),cast_to=bytes).hex()},
                "side_effects":list(final.globals.get("events",()))}

        for flags in range(4):
            for fmt,value in (("literal",0),("value=%d",-17)):
                for exit_code in (23,-7):
                    observations.append({"id":f"flags-{flags}-{fmt}-exit-{exit_code}",
                        "input":{"args":[fmt,value],"initial_state":{"callback_flags":flags,"exit_result":exit_code}},
                        "categories":["boundary" if flags==0 else "nominal"],
                        "reference_output":run(retail,True,flags,fmt,value,exit_code),
                        "candidate_output":run(rebuilt,False,flags,fmt,value,exit_code)})
        candidate_hash=digest(candidate)
    failed=sum(case["reference_output"]!=case["candidate_output"] for case in observations)
    report={"schema_version":1,"kind":"behavioral-test-report","result":"failed" if failed else "passed",
        "executed":True,"oracle":"retail-binary-execution","address":args.address,
        "binary_sha256":args.binary_sha256,"implementation_sha256":args.implementation_sha256,
        "method":"Retail and compiled x86 fatal dispatcher bodies execute with identical formatter and callback models; full buffer, callback order/text and final return compared.",
        "observable_fields":["return_value","state_delta","side_effects"],"float_tolerance":{"absolute":0,"relative":0},
        "tests_total":len(observations),"tests_passed":len(observations)-failed,"tests_failed":failed,"cases":observations,
        "retail_execution":{"tool":"angr","engine":"VEX","version":angr.__version__,"function_entry":args.address,
            "binary_sha256":args.binary_sha256,"candidate_sha256":candidate_hash,"candidate_build_command":command},
        "limitations":["Formatters and callbacks are explicit models; this verifies dispatch and assembly of the message, not CRT, GUI or process termination.",
            "Retail callback addresses match its initializer; only literal and signed-integer formats and valid bounded memory are tested.",
            "No live original-game runtime capture or arbitrary-callback proof."]}
    print(json.dumps(report))
    return int(bool(failed))


if __name__=="__main__": raise SystemExit(main())
