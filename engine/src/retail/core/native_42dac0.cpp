// Native: QTask_LinkClassHandlersA_42DAC0; retail VA 0x0042DAC0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0042DAC0-0x0042DAF2 (51 bytes, 1 block, 0 edges, 15 instructions).
// Tools/evidence: GNU objdump i386 disassembly build/retail-dis.txt (0x0042DAC0-0x0042DAF2), with the byte span and instruction count re-derived from
//   that listing; the private phase ids, the handler ABI and the whole class family are in docs/fidelity/evidence/qtask-handler-abi.md.
// Behavior/CFG: called with no arguments by the start-up sequence (0x00402B88: call 0x0042DAC0, right after the "Car" class init 0x0042DCF0 at
//   0x00402B83). It links the two class callbacks of the class whose id word is 0x0053BDA8 into the handler table through 0x00401400: the parse
//   callback 0x0042DB00 at the private phase byte 0x0054DC30 (read through 0x004EE0E0), then the size callback 0x0042DCE0 at the private phase
//   byte 0x0054DC31 (read through 0x004EE0F0). Each link is QTask_LinkQTaskEventHandler(class, phase, handler) with the class word read through
//   0x0042FDC0; the second link's result is left in EAX on return. Single block, no branch, two calls to 0x00401400.
// Status/limits: TRANSCRIBED (static only; no build and no runtime). The two callbacks are transcribed in native_42db00.cpp and native_42dce0.cpp and
//   are declared here with NATIVE_DECL so the pushed function addresses are the retail ones. The first link's return value is discarded in retail as
//   well (the following instructions overwrite EAX through the next getter call). The class word at 0x0053BDA8 is written by 0x0042DCF0, which is not
//   transcribed yet; until it runs, both links use the zero-initialised id 0 like the retail would before that init. The candidate compiles for the x86
//   target as a single translation unit (i686-w64-mingw32-g++ 16.2.0, -std=c++20 -O2); the compiled body keeps the retail order (handler address,
//   phase getter, class getter, 0x00401400, twice) and returns the second link result, with a different register allocation; the repo build target does
//   not list this file yet.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004EE0E0, 0x004EE0F0 and 0x0042FDC0: transcribed as engine/src/retail/sys/native_4ee0e0.cpp, native_4ee0f0.cpp and
// engine/src/retail/sys/native_42fdc0.cpp.
std::uint8_t NATIVE_DECL Sys_GetGlobal8_4EE0E0(void);
std::uint8_t NATIVE_DECL Sys_GetGlobal8_4EE0F0(void);
std::uint16_t NATIVE_DECL Sys_GetGlobal16_42FDC0(void);

// The two callbacks this registrar links; defined in native_42db00.cpp and native_42dce0.cpp.
void NATIVE_DECL QTask_HandlerA_42DB00(std::uint32_t arg1, void* context);
std::uint32_t* NATIVE_DECL QTask_HandlerB_42DCE0(std::uint32_t arg1, std::uint32_t* size_out);

std::uint32_t NATIVE_DECL QTask_LinkClassHandlersA_42DAC0(void)
{
    const std::uint32_t parse_phase = Sys_GetGlobal8_4EE0E0();                                  // 0x0042DAC5: byte 0x0054DC30
    const std::uint16_t class_id = Sys_GetGlobal16_42FDC0();                                    // 0x0042DACB: word 0x0053BDA8
    QTask_LinkQTaskEventHandler(class_id, parse_phase,                                          // 0x0042DAD1: call 0x00401400
                                reinterpret_cast<igi::retail::core::FrameHandler>(QTask_HandlerA_42DB00));
    const std::uint32_t size_phase = Sys_GetGlobal8_4EE0F0();                                   // 0x0042DADE: byte 0x0054DC31
    const std::uint16_t class_id_again = Sys_GetGlobal16_42FDC0();                              // 0x0042DAE4: word 0x0053BDA8
    return QTask_LinkQTaskEventHandler(class_id_again, size_phase,                              // 0x0042DAEA: call 0x00401400
                                       reinterpret_cast<igi::retail::core::FrameHandler>(QTask_HandlerB_42DCE0));
}                                                                                               // 0x0042DAF2: ret

} // extern "C"

#undef NATIVE_DECL
