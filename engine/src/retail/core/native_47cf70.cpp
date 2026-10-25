// Native: Task_DefineBulletHole (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0047CF70-0x0047D052.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 227 bytes, 1 block, 63 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 63 instructions / 227 bytes / 0 edges; `pd`
//   stops at the ret at 0x0047D052) plus the objdump listing build/retail-dis.txt for the same extent, read in two ranges
//   (0x0047CF70-0x0047D045 from the r2 dump, 0x0047D045-0x0047D053 from the listing) so no part of the body is transcribed from a summary; `p8`
//   at 0x00540908 for the class name (bytes 42756c6c6574486f6c6500 = "BulletHole", byte-identical to the literal below); `pxw` at 0x00540904,
//   0x005C1138, 0x005C1108 and 0x005C1120 for the image values; a full stack-pointer simulation over the 63 instructions, which returns esp to its
//   entry value at the ret and confirms every cleanup (0x4 = 1 push, 0xc = 3 pushes three times, 0x14 = 5, 0x18 = 6, 0x2c = 11). Every getter was
//   re-read at its own VA and is a single-instruction argument-less load ending in a ret: 0x004018F0, 0x004C48C0, 0x004E5350, 0x004C56F0,
//   0x004D0D80 and 0x004D97E0, so every push that precedes them is inert - and each is simultaneously the following 0x00401400/0x00401530 link
//   call's pre-pushed handler slot, which the same stack simulation confirms (the pushed value is still on the stack at [esp+0xC] when the link
//   call runs, because the getter is __cdecl and cleans nothing). 0x004B0B00 is Mem_AllocIndexedBlock, declared in
//   engine/include/igi/retail/mem/task_list.hpp; its first argument is the LAST push, which is how 0x00401900 calls it too
//   (`push 0; push count; push object_size; push &slot+0x0C`, see core/native_401900.cpp), so the two calls here pass the two descriptors as
//   their first argument.
// Behavior/CFG: allocates an event id with QTask_AllocateEvent(0) and caches its low byte at 0x00540904 (image value 0xFF), then links handlers
//   onto the class ids returned by two existing type-id slots: 0x0047D380 on the id from Sys_GetGlobal16_4C48C0() and 0x0047E0C0 on the id from
//   Sys_GetGlobal16_4E5350(), both on that same allocated event id through 0x00401400. It then registers the "BulletHole" task type as
//   TaskType_Register("BulletHole", 0xE148, QTask_GetCurrentType(), 0) and stores the returned 16-bit id at 0x005C1138, and links four more
//   handlers to that id: 0x0047D0B0 on the event id from DynObj_GetByte_4C56F0() and 0x0047D060 on the event id from Sys_GetGlobal8_4D97E0()
//   through 0x00401400, 0x0047D320 on the constant event 2 and 0x005188C0 on the event id from Sys_GetGlobal32_4D0D80() through 0x00401530.
//   Finally it builds two six-dword Mem_AllocIndexedBlock descriptors: 0x005C1108 with element size 4, count 0x64 and alignment 4, and 0x005C1120
//   with element size 0x40, count 0x64 and alignment 4. Straight-line: one basic block, no branch, no loop, no frame, no parameter container,
//   16 call sites over 11 distinct callees. There is no 0x004B8810/0x004B8890 script-name tail in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Documented shape differences, all value-preserving: (1) every getter callee is an
//   argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; each is simultaneously the next link call's pre-pushed handler, which the port keeps as the handler value in the
//   same order; (2) the first two links take their CLASS ID from a getter rather than from a freshly registered type, so this body links onto
//   types that already exist - 0x004C48C0 returns the low word of 0x00547F42 and 0x004E5350 the low word of 0x00A5E3F4; (3) the event id cached
//   at 0x00540904 is a byte in retail (0x0047CF7A stores AL, 0x0047CF90 and 0x0047CFD8 re-load bytes), and sys/native_47e6f0.cpp owns that slot
//   as a std::uint8_t, so this file declares it extern with that exact type and stores a byte. 0x005C1108 and 0x005C1120
//   are declared as six-dword descriptor arrays because that is the shape Mem_AllocIndexedBlock writes and because the two slots are exactly 0x18
//   bytes apart in the image, which is six dwords; 0x005C1138 is a 16-bit id slot, declared as such because retail stores it with a 16-bit store
//   (`mov word [0x5C1138],ax` at 0x0047CFC8). No global written here is owned elsewhere except 0x00540904, and all of them have image value 0 or
//   0xFF as noted, so this file defines everything it does not import. Handlers 0x0047D380, 0x0047E0C0, 0x0047D0B0, 0x0047D320 and 0x0047D060
//   are raw code immediates in retail and are not transcribed yet (native gaps, all inside 0x0046xxxx-0x0048xxxx). The meaning of the event id
//   allocated with argument 0, of the 0xE148 object size and of the two descriptor parameters is not established. Domain: core/, because it is
//   one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/mem/task_list.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);                  // 0x004C48C0: argument-less word getter
extern "C" std::uint16_t Sys_GetGlobal16_4E5350(void);                  // 0x004E5350: argument-less word getter (sys/native_4e5350.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4D0D80(void);                  // 0x004D0D80: argument-less dword getter (sys/native_4d0d80.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_4D97E0(void);                    // 0x004D97E0: argument-less byte getter (sys/native_4d97e0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56F0(void);                    // 0x004C56F0: argument-less byte getter (dynobj/native_4c56f0.cpp)

extern "C" {

// 0x00540904: one byte, owned and defined by sys/native_47e6f0.cpp; this body is its writer and one of its readers.
extern std::uint8_t g_540904;

// 0x005C1138: the "BulletHole" task type id; retail stores it with a 16-bit store and re-loads it four times as a word (0x0047CFD8,
// 0x0047CFE6, 0x0047D008, 0x0047D021).
std::uint16_t g_5c1138 = 0U;

// 0x005C1108 and 0x005C1120: the two six-dword task-list descriptors written by Mem_AllocIndexedBlock (0x004B0B00). The two slots are exactly
// 0x18 bytes apart in the image, which is six dwords, matching the shape engine/include/igi/retail/core/task_types.hpp describes for the
// task_list member that 0x004B0B00 writes.
std::int32_t g_5c1108[6] = {0, 0, 0, 0, 0, 0}; // retail 0x005C1108 (image: six zero dwords)
std::int32_t g_5c1120[6] = {0, 0, 0, 0, 0, 0}; // retail 0x005C1120 (image: six zero dwords)

void Task_DefineBulletHole()
{
    // 0x0047CF70: push 0 (QTask_AllocateEvent's argument); 0x0047CF72: call 0x4017C0; 0x0047CF77: add esp,4;
    // 0x0047CF7A: mov byte [0x540904],al - the low byte of the allocated id is what is cached.
    ::g_540904 = static_cast<std::uint8_t>(QTask_AllocateEvent(0U));

    // 0x0047CF7F: push 0x0047D380 (the pre-pushed third argument, and the getter's inert argument); 0x0047CF84: push eax (the allocated event
    // id); 0x0047CF85: call 0x4C48C0 (the class id); 0x0047CF8A: push eax; 0x0047CF8B: call 0x401400.
    QTask_LinkQTaskEventHandler(Sys_GetGlobal16_4C48C0(), // 0x0047CF8A: the class id comes from the getter, not from a registration
                                static_cast<std::uint32_t>(::g_540904), // 0x0047CF84: the allocated event id (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0047D380U))); // 0x0047CF7F: push handler (argument 3)

    // 0x0047CF90: mov al,[0x540904]; 0x0047CF95: add esp,0xc; 0x0047CF98: push 0x0047E0C0 (pre-pushed third argument);
    // 0x0047CF9D: push eax; 0x0047CF9E: call 0x4E5350 (the class id); 0x0047CFA3: push eax; 0x0047CFA4: call 0x401400; 0x0047CFA9: add esp,0xc.
    QTask_LinkQTaskEventHandler(Sys_GetGlobal16_4E5350(), // 0x0047CFA3: the class id comes from the getter
                                static_cast<std::uint32_t>(::g_540904), // 0x0047CF9D: the same cached event id (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0047E0C0U))); // 0x0047CF98: push handler (argument 3)

    // 0x0047CFAC/0x0047CFAE: push 0; push 0 (inert arguments of the getter, and TaskType_Register's fourth/fifth slots);
    // 0x0047CFB0: call 0x4018F0; 0x0047CFB5: push eax; 0x0047CFB6: push 0xE148; 0x0047CFBB: push "BulletHole"; 0x0047CFC0: call 0x401900;
    // 0x0047CFC5: add esp,0x14 - 5 pushes above.
    const std::uint16_t bullet_hole_id = TaskType_Register("BulletHole", // 0x0047CFBB: push 0x00540908
                                                           0xE148U,        // 0x0047CFB6: push 0xE148 (object size)
                                                           QTask_GetCurrentType(), // 0x0047CFB0: call 0x4018F0 (parent type)
                                                           0U);            // 0x0047CFAE: push 0 (task list count)

    // 0x0047CFC8: mov word [0x5C1138],ax - the id is stored after the cleanup, exactly as retail orders it.
    ::g_5c1138 = bullet_hole_id;

    // 0x0047CFCE: push 0x0047D0B0 (pre-pushed third argument, and the getter's inert argument); 0x0047CFD3: call 0x4C56F0 (event id);
    // 0x0047CFD8: mov cx,[0x5C1138]; 0x0047CFDF: push eax; 0x0047CFE0: push ecx; 0x0047CFE1: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1138), // 0x0047CFD8: mov cx,[0x5C1138]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56F0()), // 0x0047CFDF: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0047D0B0U))); // 0x0047CFCE: push handler (argument 3)

    // 0x0047CFE6: mov dx,[0x5C1138]; 0x0047CFED: push 0x0047D320 (pre-pushed third argument); 0x0047CFF2: push 2;
    // 0x0047CFF4: push edx; 0x0047CFF5: call 0x401530; 0x0047CFFA: add esp,0x18 - 6 pushes above.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c1138), // 0x0047CFE6: mov dx,[0x5C1138]
                                           2U,                                     // 0x0047CFF2: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0047D320U))); // 0x0047CFED: push handler

    // 0x0047CFFD: push 0x005188C0 (pre-pushed third argument, and the getter's inert argument); 0x0047D002: call 0x4D0D80 (event id);
    // 0x0047D007: push eax; 0x0047D008: mov ax,[0x5C1138]; 0x0047D00E: push eax; 0x0047D00F: call 0x401530; 0x0047D014: add esp,0xc.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c1138), // 0x0047D008: mov ax,[0x5C1138]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4D0D80()), // 0x0047D007: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x005188C0U))); // 0x0047CFFD: push handler

    // 0x0047D017: push 0x0047D060 (pre-pushed third argument, and the getter's inert argument); 0x0047D01C: call 0x4D97E0 (event id);
    // 0x0047D021: mov cx,[0x5C1138]; 0x0047D028: push eax; 0x0047D029: push ecx; 0x0047D02A: call 0x401400 (no cleanup of its own).
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1138), // 0x0047D021: mov cx,[0x5C1138]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4D97E0()), // 0x0047D028: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0047D060U))); // 0x0047D017: push handler (argument 3)

    // 0x0047D02F/0x0047D031/0x0047D033/0x0047D035: push 4; push 0x64; push 4; push 0x005C1108; 0x0047D03A: call 0x4B0B00. The first
    // argument is the last push, so this is Mem_AllocIndexedBlock(&0x5C1108, 4, 0x64, 4): descriptor 0x005C1108, element size 4, count 0x64,
    // alignment 4. The return value is not used, as in retail.
    (void)Mem_AllocIndexedBlock(::g_5c1108, 4, 0x64, 4); // 0x0047D035: push 0x005C1108

    // 0x0047D03F/0x0047D041/0x0047D043/0x0047D045: push 4; push 0x64; push 0x40; push 0x005C1120; 0x0047D04A: call 0x4B0B00. Same first-argument
    // rule, so this is Mem_AllocIndexedBlock(&0x5C1120, 0x40, 0x64, 4).
    (void)Mem_AllocIndexedBlock(::g_5c1120, 0x40, 0x64, 4); // 0x0047D045: push 0x005C1120

    // 0x0047D04F: add esp,0x2c - 11 pushes above (the 0x0047D060 link's 3 plus the two allocation calls' 4 each); 0x0047D052: ret.
} // end of Task_DefineBulletHole

} // extern "C"
