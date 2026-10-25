// Native: Core_RegisterAmmoType_47CAB0; retail VA 0x0047CAB0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0047CAB0-0x0047CD0E
//   (58 bytes, 2 blocks, 14 instructions; per-block 7 / 7 at 0x0047CAB0 and 0x0047CCF0 from af + afbj).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image. The entry is called from the startup sequence at
//   0x004029A8 (`call 0x0047CAB0`), and `/r 0x47CCF0` finds exactly one reference - the `jmp 0x0047CCF0` at 0x0047CAC6 - so 0x0047CCF0 is this
//   routine's own continuation and not a separate native; that is why the two blocks are one function. Bytes 0x0047CACB-0x0047CACF are nop padding
//   between the two blocks.
// Behavior/CFG: two straight-line halves. First, 0x0047CAB0 registers the script command "DefineAmmoType" (0x005408D0) with handler 0x0047CAD0 and
//   the argument tuple (name, handler, 0, -1) through 0x004B8890, then jumps to 0x0047CCF0. Second, 0x0047CCF0 zeroes the dword at 0x005C10E4 and
//   dispatches the path "LOCAL:ammo" (0x005408E0) with callback 0x0047CA40 and context 0 through 0x004B1420. No branch, no loop.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Both handlers 0x0047CAD0 and 0x0047CA40 are untranscribed (native gaps), so this routine currently
//   hands retail addresses to the two callees exactly as retail does. 0x005C10E4 has no other owner in the tree; the image holds 0 there and this
//   routine is the only writer, so the file defines it. Domain: core/, because it is one of the 0x00402930 startup registrars; the strings name an
//   ammo script command and a "LOCAL:ammo" path, so qfile/ would be the alternative reading of its subject matter. The store to 0x005C10E4 happens
//   after retail's three argument pushes and before the call; the pushes are not observable, so the store is placed before the call statement.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004B8890: copied from its definition in script/native_4b8890.cpp (extern "C", no NATIVE_DECL there, so none here).
void Script_QHashNodeRegisterEx(void* const arg0, void* const arg1, char const* const arg2, void* const arg3);

// 0x004B1420: copied from its definition in qfile/native_4b1420.cpp.
std::int32_t NATIVE_DECL QFile_DispatchDevicePath_4B1420(const char* path, void* callback, std::int32_t context);

// 0x005C10E4: dword zeroed by this routine; no other owner, image value 0.
std::uint32_t g_5c10e4 = 0U;

void NATIVE_DECL Core_RegisterAmmoType_47CAB0(void)
{
    Script_QHashNodeRegisterEx(                                     // 0x0047CABE: call 0x004B8890
        const_cast<char*>("DefineAmmoType"),                        // 0x0047CAB9: push 0x005408D0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0047CAD0U)), // 0x0047CAB4: push 0x0047CAD0 (handler)
        nullptr,                                                    // 0x0047CAB2: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x0047CAB0: push -1
    // 0x0047CAC3: add esp,0x10
    // 0x0047CAC6: jmp 0x0047CCF0 (this routine's own second block)
    ::g_5c10e4 = 0U;                                                // 0x0047CCFC: mov dword [0x005C10E4],0
    QFile_DispatchDevicePath_4B1420("LOCAL:ammo",                   // 0x0047CCF7: push 0x005408E0
                                    reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0047CA40U)), // 0x0047CCF2: push 0x0047CA40 (callback)
                                    0);                             // 0x0047CCF0: push 0
    // 0x0047CD06: call 0x004B1420; 0x0047CD0B: add esp,0xc
}                                                                   // 0x0047CD0E: ret

} // extern "C"

#undef NATIVE_DECL
