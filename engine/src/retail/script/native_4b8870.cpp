// Native: Script_QHashNodeRegisterData_4B8870 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004B8870.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B8870-0x004B888F (32 bytes, 1 block, 12 instructions, 0 edges; no frame and no saved registers - the whole body is two loads, seven pushes, one call, one stack cleanup and a ret).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image, plus the raw bytes read at the VA (8b442408 8b4c2404 6a00 6a02 6a00 6a08 50 68e07f4b00 51 e834220000 83c41c c3).
//   The call is at 0x004B8887 with rel32 0x00002234, and 0x004B888C + 0x2234 = 0x004BAAC0, so the single callee is Script_QHashNodeCreate
//   (script/native_4baac0.cpp, transcribed there as a seven-argument cdecl routine). The pushes are, in retail order, 0 (0x004B8878), 2
//   (0x004B887A), 0 (0x004B887C), 8 (0x004B887E), the second argument (0x004B8880), the immediate 0x004B7FE0 (0x004B8881) and the first argument
//   (0x004B8886); cdecl makes the last push argument 1, so the callee receives (argument 1, 0x004B7FE0, argument 2, 8, 0, 2, 0). The
//   `add esp,0x1c` at 0x004B888C pops seven arguments, which is the independent confirmation that the callee takes seven. 0x004B7FE0 is CODE,
//   not data - its bytes begin 83ec208b44242856578d742410dd00 - so it is a callback passed as an argument, and it is passed as a cast literal
//   following the tree's existing precedent in script/native_4b7f40.cpp (which passes 0x004B7F60 the same way), so this file introduces no
//   undefined symbol for it. The width constant 8 and the two call sites agree with the objects they register: the only caller in the image is
//   0x00415530 Game_Init, which calls this routine at 0x004156CB with "WORLD_METER" (0x00539664) and 0x00539558, and at 0x004156DA with
//   "MATH_PI" (0x0053965C) and 0x00539550 - two 8-byte objects - and discards eax at both sites.
// Behavior/CFG: forwards its two arguments to Script_QHashNodeCreate together with the callback 0x004B7FE0 and the constants 8, 0, 2 and 0, and
//   passes back whatever the callee leaves in eax. A single basic block with no branch, no loop and no local state.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Two arguments, both read before any frame exists: the first at [esp+4] (0x004B8874) and the
//   second at [esp+8] (0x004B8870). The second is typed std::uint64_t* to match the two 8-byte objects Game_Init passes, and the callee's third
//   parameter is declared void const* in script/native_4baac0.cpp, so the argument is passed through with a reinterpret_cast and its bytes are
//   unchanged. The constants 8, 0, 2 and 0 are pushed as void* to match the callee's declared parameter types; retail pushes them as immediates
//   and the byte values are identical. Declared void because eax is not touched between the call and the ret, so the value is the callee's own.
//   Domain: script/, with 0x004BAAC0, 0x004B8810, 0x004B8890 and 0x004B88F0 in the same named-value registry family.
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

void NATIVE_DECL Script_QHashNodeRegisterData_4B8870(char const* name, std::uint64_t* value)
{
    Script_QHashNodeCreate(name,                                             // 0x004B8886: push ecx (argument 1)
                           reinterpret_cast<void*>(0x004B7FE0U),             // 0x004B8881: push 0x004B7FE0, a callback
                           value,                                            // 0x004B8880: push eax (argument 2)
                           reinterpret_cast<void*>(8),                       // 0x004B887E: push 8
                           reinterpret_cast<void*>(0),                       // 0x004B887C: push 0
                           reinterpret_cast<void*>(2),                       // 0x004B887A: push 2
                           reinterpret_cast<void*>(0));                      // 0x004B8878: push 0
    // 0x004B888C: add esp,0x1c - seven arguments; 0x004B888F: ret.
}

} // extern "C"

#undef NATIVE_DECL
