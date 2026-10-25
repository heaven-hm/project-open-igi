// Native: Sys_GetGlobalPointer_47CF10 (address-suffixed placeholder name; the retail symbol is unknown and the name is the one the two existing callers in this tree already declare, so the C symbol matches theirs); retail VA 0x0047CF10-0x0047CF15.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 6 bytes, 1 block, 2 instructions.
// Tools/evidence: radare2 6.2.0 `af`/`afbj` at 0x0047CF10 with bin.relocs.apply over the exact image reports one function of size 6 with 2
//   instructions (0x0047CF10 and 0x0047CF15) and no edges, and `pd 8 @ 0x0047CF10` prints exactly `mov eax,0x5C10F0` / `ret` followed by ten
//   0x90 padding bytes and then the next function at 0x0047CF20, so the extent is 6 bytes and cannot be longer. `px 16 @ 0x0047CF10` gives the
//   bytes b8 f0 10 5c 00 c3 90 90 90 90 90 90 90 90 90 90, which is the same two instructions plus that padding. The objdump listing
//   build/retail-dis.txt resolves both references to this VA image-wide: 0x0046D59C (in the accepted core/native_46d420.cpp) and 0x0046DBD7 (in
//   the accepted core/native_46db40.cpp), and both are `call` instructions, so the routine has exactly two callers in the image and neither
//   pushes an argument for it. `pxw 8 @ 0x005C10F0` gives four zero dwords in the image; the storage itself is owned and defined by
//   core/native_47cf20.cpp (see Status/limits), which was established from that file's own body and from the callee 0x004F1D60.
// Behavior/CFG: one basic block, no branch, no loop, no frame, no parameter, no stack use. It loads the immediate 0x005C10F0 into EAX and returns
//   it. Retail calls it with no argument and ignores nothing: both call sites use the returned address directly (0x0046D5A1 and 0x0046DBDC push
//   EAX straight after the call), and 0x004F1E20, which is called in between, reads the global 0x0054E158 and never touches its own arguments,
//   so the returned value survives to become the 0x004F1A80 record's sixth argument. It reads no memory and writes none: the address is an
//   immediate, so this is a pure address-of routine, not a load of a pointer stored at some other location.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The return type is std::uint32_t because both call sites consume the full 32-bit EAX as the
//   address 0x005C10F0; no caller inspects only a part of it. 0x005C10F0 is NOT declared or defined here: its single definition in this tree is
//   `std::int32_t g_5c10f0[4]` in core/native_47cf20.cpp, which is the four-dword list head that retail's 0x004F1D60 initialises through
//   0x004F1CD0 and that 0x0047CF20 fills with the "AMMO_ID_.*" record. Declaring a second copy here would violate the one-implementation-per-VA
//   rule and would also break the identity the value carries, so this file declares that existing symbol and returns its address, which is the
//   port's storage for retail address 0x005C10F0. Consequence to record: this file therefore LINK-depends on core/native_47cf20.cpp being in the
//   build; if 47cf20 is ever held out, this file fails to link rather than silently duplicating the storage. The port address is not the retail
//   constant 0x005C10F0 and is not intended to be: every accepted file in this tree passes the address of the storage it owns (the append frame
//   convention in 0x004813E0/0x00470FA0/0x0046DB40/0x0046D420 and 0x005C10F0 here), and the two callers pass the value through to 0x004F1A80,
//   which stores it in the record at +0x08 without dereferencing it. What consumes that stored address is above 0x0048FFFF (the 0x004F1A80
//   implementation and the 0x004F1E20 family) and is outside this lane's scope. The name keeps its address suffix: this routine is not verified
//   at runtime, and renaming it before verification would break the two existing callers' declarations. Domain: core/, because both of its
//   callers are core registrars and because the storage it returns belongs to the 0x0047CF20 core registrar.
#include <cstdint>

extern "C" {

// 0x005C10F0: DECLARATION ONLY. The single definition is `std::int32_t g_5c10f0[4]` in core/native_47cf20.cpp, under the same C linkage, so this
// reference is unmangled and the linker resolves it to that one object. Four dwords is every field retail writes to this head.
extern std::int32_t g_5c10f0[4];

std::uint32_t Sys_GetGlobalPointer_47CF10(void)
{
    // 0x0047CF10: mov eax,0x5C10F0; 0x0047CF15: ret. The address of the port's storage for that retail address, not the retail constant.
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_5c10f0[0]));
} // end of Sys_GetGlobalPointer_47CF10

} // extern "C"
