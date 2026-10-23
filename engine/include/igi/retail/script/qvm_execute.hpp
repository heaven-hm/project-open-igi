#pragma once

// The retail's QVM execution entry points.
//
// Qvm_Load (0x004B80B0) builds the 0x94-byte script object and resolves the
// precompiled program into it; Qvm_Execute (0x004B85B0) runs that object, and
// Qvm_GetFloatArg (0x004B8A50) reads one real argument of a running native out
// of the interpreter's argument frame. DefineHumanPlayerGeneral (0x00410070)
// reads all of its arguments through Qvm_GetFloatArg.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

extern "C" {

// Native: Qvm_Execute() - VA 0x004B85B0 - complete transcription.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE;
//                 VA 0x004B85B0-0x004B86DC, 301 bytes, 14 blocks/20 edges, digest ae94640768a02e53ab44e0873dda2ded792eca51ddf4f5dc7b912d359151ecdc;
//                 docs/fidelity/evidence/0x004B85B0-*.json.
// Signature: x86 cdecl int32_t(void* qvm). Binary Ninja's own prototype is int32_t(void*) and its
//            user note calls this "the interpreter entry", but the body contains no instruction loop:
//            the precompiled-program path delegates to 0x004BD080, which drives the opcode interpreter
//            Qvm_ExecuteInstructions (0x004BCFD0), and the fallback path parses and discards script
//            source blocks. Both return values are documented in the source.
// Behavior: clears the two QVM state words, reads the resolved program pointer at qvm+0x8C; when it is
//           non-null runs 0x004BD080(program) and returns 0, or reports "Error in QVM program: %s\r\n%s"
//           with the object path and the program's error text at program+0x268 and returns the reporter's
//           result. When it is null it initialises a script source buffer over qvm+0x80/+0x84 and loops:
//           read the next block, parse it, release the parsed node, and stop on end-of-buffer, on the
//           stop/abort words, or on a reported error (reporting "An error occured in script file: %s"
//           when reporting is enabled), then releases the source reader and returns its result.
// Status/limits: STRUCTURALLY_VERIFIED against retail 0x004B85B0 (14 blocks/20 edges; the recompiled
//           object has 14 blocks/21 edges, the extra edge being GCC's jump from the source-path tail to
//           the shared return block). The eight retail callees are declared but not yet transcribed, and
//           the three shared QVM state words this native reads are still internal to
//           qvm_native_loader.cpp, so the object compiles for x86 but does not link yet. No runtime
//           capture exists, so BUILD_VERIFIED/RUNTIME_TESTED are not claimed.
std::int32_t IGI_RETAIL_CDECL Qvm_Execute(void* qvm);

// Native: Qvm_GetFloatArg() - VA 0x004B8A50 - complete transcription.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE;
//                 VA 0x004B8A50-0x004B8A7B, 44 bytes, digest 209a558f2efce4270ed8eca0ca5e9c586022d1ea9482970483249b59e5944d63;
//                 docs/fidelity/evidence/0x004B8A50-*.json.
// Signature: x86 cdecl long double(void* argument_frame, int32_t index). Binary Ninja's stored name
//            (Profile_ValueSet) and prototype are stale: the callers (for example DefineHumanPlayerGeneral
//            at 0x00410070) call it cdecl and consume the result with `fstp dword [global]`, and both of
//            its callees leave an 80-bit value in ST0, so the helper returns the real on the x87 stack,
//            not an integer in EAX.
// Behavior: reads the frame flag at +0x04; when it is non-zero the argument comes from the frame's
//           indexed value reader 0x004BD010(frame, index), otherwise from the argument table at +0x08:
//           the 32-bit entry at table + index*4 is an expression node evaluated by 0x004B9660. Both
//           callees return the value in ST0 and this helper returns it unchanged.
// Status/limits: STRUCTURALLY_VERIFIED against retail 0x004B8A50 (3 blocks/2 edges in both the retail
//           image and the recompiled object; the only difference is that the candidate tail-jumps to
//           each callee instead of using call/ret). The two retail callees are declared but not yet
//           transcribed, so the object compiles for x86 but does not link yet. No runtime capture
//           exists, so BUILD_VERIFIED/RUNTIME_TESTED are not claimed.
long double IGI_RETAIL_CDECL Qvm_GetFloatArg(void* argument_frame, std::int32_t index);

} // extern "C"
