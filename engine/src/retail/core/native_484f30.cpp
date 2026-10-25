// Native: Core_RegisterMission_484F30; retail VA 0x00484F30 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00484F30-0x0048516E
//   (70 bytes, 2 blocks, 17 instructions; per-block 10 / 7 at 0x00484F30 and 0x00485150 from af + afbj).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image. `/r 0x485150` finds exactly one reference - the
//   `jmp 0x00485150` at 0x00484F52 - so the second block is this routine's own continuation, not a separate native. Bytes 0x00484F57-0x00484F5F
//   are nop padding between the blocks. The script strings are pushed as immediate addresses 0x00540A44 "DefineMission", 0x00540A24
//   "MISSION_NEXT_MISSION_UNDEFINED" and 0x00540A70 "LOCAL:missions".
// Behavior/CFG: block 1 registers the script command "DefineMission" with handler 0x00484F60 through 0x004B8890 using the tuple
//   (name, handler, 0, -1), then registers the script constant "MISSION_NEXT_MISSION_UNDEFINED" with value -1 through 0x004B8810, then jumps to
//   its own second block. Block 2 zeroes the dword at 0x005C89FC, dispatches the path "LOCAL:missions" with callback 0x00484E60 and context 0
//   through 0x004B1420, and returns. No branch, no loop. Cleanup 0x18 in block 1 is 16 + 8 bytes, matching the two calls' argument pushes.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The two handlers 0x00484F60 and 0x00484E60 are untranscribed (native gaps), so retail addresses
//   are handed to the callees exactly as retail does. 0x005C89FC has no other owner and image value 0, so this file defines it. Domain: core/,
//   because it is one of the 0x00402930 startup registrars; its subject matter is mission script names, so script/ would be the alternative.
//   The store to 0x005C89FC sits after retail's three argument pushes and before the call; pushes are not observable, so the store precedes the
//   call statement here.
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

// 0x004B8890: copied from its definition in script/native_4b8890.cpp (extern "C", no NATIVE_DECL there).
void Script_QHashNodeRegisterEx(void* const arg0, void* const arg1, char const* const arg2, void* const arg3);

// 0x004B8810: copied from the declaration the tree already uses (core/native_0046dd00.cpp), defined extern "C" in script/native_4b8810.cpp.
void Script_QHashNodeRegister(void* const arg1, void* const value);

// 0x004B1420: copied from its definition in qfile/native_4b1420.cpp.
std::int32_t NATIVE_DECL QFile_DispatchDevicePath_4B1420(const char* path, void* callback, std::int32_t context);

// 0x005C89FC: dword zeroed by this routine; no other owner, image value 0.
std::uint32_t g_5c89fc = 0U;

void NATIVE_DECL Core_RegisterMission_484F30(void)
{
    NATIVE_LOG_ENTER("Core_RegisterMission_484F30", "0x00484F30");
    Script_QHashNodeRegisterEx(                                      // 0x00484F3E: call 0x004B8890
        const_cast<char*>("DefineMission"),                          // 0x00484F39: push 0x00540A44
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00484F60U)), // 0x00484F34: push 0x00484F60 (handler)
        nullptr,                                                     // 0x00484F32: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x00484F30: push -1
    Script_QHashNodeRegister(                                        // 0x00484F4A: call 0x004B8810
        const_cast<char*>("MISSION_NEXT_MISSION_UNDEFINED"),         // 0x00484F45: push 0x00540A24
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x00484F43: push -1
    // 0x00484F4F: add esp,0x18 (16 + 8 argument bytes)
    // 0x00484F52: jmp 0x00485150 (this routine's own second block)
    ::g_5c89fc = 0U;                                                 // 0x0048515C: mov dword [0x005C89FC],0
    QFile_DispatchDevicePath_4B1420("LOCAL:missions",                // 0x00485157: push 0x00540A70
                                    reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00484E60U)), // 0x00485152: push 0x00484E60 (callback)
                                    0);                              // 0x00485150: push 0
    // 0x00485166: call 0x004B1420; 0x0048516B: add esp,0xc
}                                                                    // 0x0048516E: ret

} // extern "C"

#undef NATIVE_DECL
