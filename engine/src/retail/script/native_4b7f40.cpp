// Native: Sys_RegisterNamedWord_4B7F40 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004B7F40.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B7F40-0x004B7F5F (32 bytes, 1 block, 12 instructions, 0 edges; no frame and no saved registers, and the whole body is seven pushes, one call, one stack cleanup and a ret).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image. Bytes read from the image: 8b442408 8b4c2404 6a01 6a01 6a00 6a02 50 68607f4b00 51 e8642b0000 83c41c c3. The call is at 0x004B7F57 with rel32 0x00002B64, and 0x004B7F5C + 0x2B64 = 0x004BAAC0, so the single callee is Script_QHashNodeCreate (script/native_4baac0.cpp). The pushes are, in retail order, 1 (0x004B7F48), 1 (0x004B7F4A), 0 (0x004B7F4C), 2 (0x004B7F4E), the second argument (0x004B7F50), the immediate 0x004B7F60 (0x004B7F51) and the first argument (0x004B7F56); cdecl makes the last push argument 1, so the callee receives (argument 1, 0x004B7F60, argument 2, 2, 0, 1, 1). The `add esp,0x1c` at 0x004B7F5C pops seven arguments, which is the independent confirmation that the callee is cdecl and takes seven. The address 0x004B7F60 is CODE, not data - its bytes begin 83ec188b442420 5657b9060000000fbf0089442428 - so it is a callback passed as an argument, and it is passed as a cast literal following the tree's existing FrameHandler precedent rather than as a symbol, so this file introduces no undefined symbol for it. The only caller in the image is 0x004696B0 at 0x00469734, which pushes "Computer_nZoomLevel" (0x0053F764) and 0x005BDDA0 and then does `add esp,0x18`, and discards eax, so the return value is discarded at every call site.
// Behavior/CFG: forwards its two arguments to 0x004BAAC0 together with the callback 0x004B7F60 and the constants 2, 0, 1 and 1, and passes back whatever the callee leaves in eax. A single basic block with no branch, no loop and no local state.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Two arguments, both read before any frame exists: the first at [esp+4] (0x004B7F44) and the second at [esp+8] (0x004B7F40). Declared `void` because eax is not touched between the call and the ret, so the value is the callee's own, and the callee Script_QHashNodeCreate is itself declared void in the tree because its callers discard the value - the passthrough is documented here rather than invented. The second argument is typed std::uint16_t* to match the declaration the only caller already carries (core/native_4696b0.cpp declares `extern "C" void Sys_RegisterNamedWord_4B7F40(char const* name, std::uint16_t* value)`), and the callee's third parameter is declared char const* in script/native_4baac0.cpp, so the argument is passed through with a reinterpret_cast and its bytes are unchanged. The constants 2, 0, 1 and 1 are pushed as void* to match the callee's declared parameter types; retail pushes them as immediates and the byte values are identical. Domain: script/, with 0x004BAAC0, 0x004B8810, 0x004B8890 and 0x004B88F0 in the same named-value registry family.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Defined by script/native_4baac0.cpp; declared here exactly as that file defines it.
void Script_QHashNodeCreate(char const* const arg1, void* const arg2, void const* const arg3,
                            void* const arg4, void* const arg5, void* const arg6,
                            void* const arg7);

void NATIVE_DECL Sys_RegisterNamedWord_4B7F40(char const* name, std::uint16_t* value)
{
    NATIVE_LOG_ENTER("Sys_RegisterNamedWord_4B7F40", "0x004B7F40");
    Script_QHashNodeCreate(name,                                               // 0x004B7F56: push ecx (argument 1)
                           reinterpret_cast<void*>(0x004B7F60U),               // 0x004B7F51: push 0x004B7F60, a callback
                           value,                                              // 0x004B7F50: push eax (argument 2)
                           reinterpret_cast<void*>(2),                         // 0x004B7F4E: push 2
                           reinterpret_cast<void*>(0),                         // 0x004B7F4C: push 0
                           reinterpret_cast<void*>(1),                         // 0x004B7F4A: push 1
                           reinterpret_cast<void*>(1));                        // 0x004B7F48: push 1
    // 0x004B7F5C: add esp,0x1c - seven arguments; 0x004B7F5F: ret.
}

} // extern "C"

#undef NATIVE_DECL
