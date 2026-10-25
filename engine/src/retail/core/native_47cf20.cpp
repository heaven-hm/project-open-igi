// Native: Task_DefineAmmoPatterns (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0047CF20-0x0047CF48.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 41 bytes, 1 block, 10 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 10 instructions / 41 bytes / 0 edges; `pd`
//   stops at the ret at 0x0047CF48) plus the objdump listing build/retail-dis.txt for the same extent; `p8` at 0x005408EC for the pattern name
//   (bytes 414d4d4f5f49445f2e2a00 = "AMMO_ID_.*", byte-identical to the literal below); `pxw` at 0x005C10F0 and 0x005C10E8 for the image values;
//   a full stack-pointer simulation over the 10 instructions, which returns esp to its entry value at the ret (0x10 = the four pushes above).
//   Callee argument mapping was derived by counting the pushes above each access, not by reading an [esp+N] offset as an argument number:
//   0x004F1CD0 takes the one pushed address as its first argument, 0x004F1DB0's `mov eax,[esp+0x14]` at 0x004F1DBF is its SECOND argument
//   (entry [esp+8]) because three pushes precede it, and its `mov edx,[esp+0x1C]` at 0x004F1DE7 is its FIRST argument (entry [esp+4]) because
//   six pushes precede it; so the call here is (head, "AMMO_ID_.*"). The same counting rule was confirmed by the Lead on Mem_AllocIndexedBlock
//   (0x004B0B14 reads [esp+0xC] after two pushes, which is arg1).
// Behavior/CFG: initialises the global list head at 0x005C10F0 through 0x004F1CD0, appends one record for the pattern "AMMO_ID_.*" to that head
//   through 0x004F1DB0, then allocates an event id with QTask_AllocateEvent(0) and stores its low byte at 0x005C10E8. Straight-line: one basic
//   block, no branch, no loop, no frame, no parameter container, 3 call sites over 3 distinct callees. There is no 0x004B8810/0x004B8890
//   script-name tail in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. 0x005C10F0 is declared as a four-dword list head because that is exactly what 0x004F1D60
//   writes through 0x004F1CD0 (`mov [eax+0xc],ecx` at 0x004F1D68, `mov [eax],eax+4` at 0x004F1D6E, `mov [eax+8],eax` at 0x004F1D70,
//   `mov dword [ecx],0` at 0x004F1D73, where ecx = eax+4) - four dwords at +0x00/+0x04/+0x08/+0x0C. The image region from 0x005C10F0 to
//   0x005C1108 is 0x18 bytes and the last 8 of them (0x005C1100-0x005C1107) are never referenced anywhere in build/retail-dis.txt, so they are
//   NOT modelled here and their owner is unestablished; whoever needs them must reconcile with this file. 0x005C10E8 is a byte in retail
//   (0x0047CF43 stores AL) and sys/native_47cdc0.cpp is a separate unimplemented getter that reads the same byte as AL, so it is declared as
//   std::uint8_t here. Neither 0x005C10F0 nor 0x005C10E8 is referenced by any file in the tree yet, and both have image value 0. The pattern
//   string is a retail .rdata literal at 0x005408EC, so it is passed as a pointer to the byte-identical literal. 0x004F1DB0 is NOT transcribed
//   anywhere in the tree: it is a real routine (it allocates 0x480C bytes through 0x004B0C60, stores its second argument at +0x4808, zeroes
//   +0x00 and +0x04, calls 0x004AF9D0 on +0x08/+0x2808 and 0x004AF8F0 on (head, record)) and is declared here as a native gap with a
//   behavior-derived placeholder name. Its return value is discarded at this call site and its return type is therefore unestablished; the
//   declaration says void, which cannot change the call site's behaviour. 0x004F1CD0 is already transcribed as Sys_InitGlobalListHead_4F1CD0 in
//   sys/native_4f1cd0.cpp under C linkage (the file was wrapped in namespace igi::retail::sys when this native was first written, which made the
//   reference C++-mangled; handler-abi-worker moved it to extern "C" and the Lead verified the unmangled symbol, so this file now declares and
//   calls it unmangled and the object has zero mangled symbols again). 0x0047CF10 (`mov $0x5C10F0,%eax; ret`, an
//   argument-less getter returning this head's address) and 0x0047CDC0 (`mov 0x5C10E8,%al; ret`, the reader of this event id) are separate
//   missing natives in 0x0046xxxx-0x0048xxxx and are not written here. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// 0x004F1DB0: no implementation exists in the tree. Placeholder name describes the proven allocation/registration only; the retail symbol is
// unknown. The return value is discarded at this call site, so the type is unestablished and declared void.
extern "C" void Sys_NewListRecord_4F1DB0(void* head, char const* pattern);

extern "C" {

// 0x004F1CD0: one-pointer-argument list-head initialiser, defined in sys/native_4f1cd0.cpp under C linkage (cdecl, which is the i686 default), so
// it is declared inside this extern "C" block and referenced unmangled.
void Sys_InitGlobalListHead_4F1CD0(void* head);

// 0x005C10F0: the list head initialised by 0x004F1D60 (through 0x004F1CD0). Four dwords, which is every field retail writes.
std::int32_t g_5c10f0[4] = {0, 0, 0, 0};

// 0x005C10E8: the allocated event id, stored as a byte (0x0047CF43 stores AL) and read as a byte by 0x0047CDC0.
std::uint8_t g_5c10e8 = 0U;

void Task_DefineAmmoPatterns()
{
    // 0x0047CF20: push 0x005C10F0; 0x0047CF25: call 0x4F1CD0.
    Sys_InitGlobalListHead_4F1CD0(::g_5c10f0);

    // 0x0047CF2A: push "AMMO_ID_.*" (0x005408EC); 0x0047CF2F: push 0x005C10F0; 0x0047CF34: call 0x4F1DB0. The last push is the first argument,
    // so this is (head, pattern).
    Sys_NewListRecord_4F1DB0(::g_5c10f0, "AMMO_ID_.*");

    // 0x0047CF39: push 0 (QTask_AllocateEvent's argument); 0x0047CF3B: call 0x4017C0; 0x0047CF40: add esp,0x10 - the four pushes above;
    // 0x0047CF43: mov byte [0x5C10E8],al - the low byte of the allocated id is what is cached.
    ::g_5c10e8 = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x0047CF48: ret
} // end of Task_DefineAmmoPatterns

} // extern "C"
