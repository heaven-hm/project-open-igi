// Native: Task_DefineStatusScreen (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00486900-0x004869AA.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 171 bytes, 1 block, 46 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 46 instructions / 171 bytes / 0 edges; `pd`
//   stops at the ret at 0x004869AA) plus the objdump listing build/retail-dis.txt for the same extent; `ps` at 0x00540D30, 0x00540D14 and
//   0x00540D04 for the three literals (byte-identical to the string literals below); `pxw 40` at 0x00540B70 for the ten dwords this body copies;
//   `px` at 0x00540B5C and 0x005C8A4C and `pxw` at 0x005C8A74 for the image values; a full stack-pointer simulation over the 46 instructions,
//   which returns esp to its entry value after the two pops and confirms both cleanups (0x3c = 15 pushes from the two inert zeros through the
//   event-2 link, 0x14 = 5 pushes for the event-3 link plus the two alias strings). The callees were re-read at their own VAs: 0x004018F0 and
//   0x004E7530 are argument-less getters (`mov eax,[abs]; ret`), 0x004017C0 is the 22-instruction / 6-block slot walk that returns the allocated
//   id, and 0x004B12F0 is a 67-instruction / 7-block routine that takes two pointers (it reads its first argument at [esp+0x10] after three
//   pushes, so cdecl right-to-left applies: the LAST push is argument 1).
// Behavior/CFG: registers the "StatusScreen" task type as TaskType_Register("StatusScreen", 0x4F0, QTask_GetCurrentType(), 0) and stores the
//   returned 16-bit id as a word at 0x005C8A74 (image value 0); then, before any link call, pushes 0x181 and calls QTask_AllocateEvent, storing
//   the returned low byte at 0x00540B5C - the id store happens between the register call and the allocate call, in retail's order. Links the id to
//   handler 0x004869B0 on event 0 through 0x00401400, to 0x00486CD0 on event 1 and 0x00486D70 on event 2 through 0x00401530, and to 0x00486F30
//   on the event id from Core_GetEventWord_4E7530 through 0x00401530. Then registers the file alias QFile_RegisterAlias_4B12F0("STATUSSCREEN:",
//   "LOCAL:screens/game/status/") and copies ten dwords (0x28 bytes) from the image block at 0x00540B70 into the block at 0x005C8A4C with
//   `mov $0xA,%ecx; rep movsl`. Straight-line: one basic block, no branch, no loop in the retail control flow (the rep is the copy), 9 call
//   sites over 7 distinct callees: 0x00486906 QTask_GetCurrentType, 0x00486916 TaskType_Register, 0x00486926 QTask_AllocateEvent,
//   0x0048693E QTask_LinkQTaskEventHandler, 0x00486952/0x00486966/0x00486980 QTask_LinkQTaskEventHandlerNoContextFn,
//   0x00486973 Core_GetEventWord_4E7530 and 0x0048698F QFile_RegisterAlias_4B12F0. (The first draft of this block said 7 call sites; the retail
//   body has 9, and my compiled object shows exactly 9, so the count was corrected against both.)
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Documented shape differences, all value-preserving: (1) 0x004018F0 and 0x004E7530 are
//   argument-less getters in retail that ignore the arguments the caller pushes, and the tree's transcriptions take no arguments, so the two
//   zeros pushed before 0x004018F0 are not reproduced; the 0x00486F30 push is simultaneously the 0x004E7530 getter's inert argument and the next
//   link call's pre-pushed handler slot, which the port keeps as the handler value in the same order; (2) QTask_AllocateEvent returns the id in
//   EAX while retail stores only AL at 0x00540B5C, so the store is a byte store of the returned value, exactly as the tree's other caller
//   0x00467D90 does it; (3) the ten-dword copy is written as a ten-iteration dword loop, which is what `rep movsl` executes, and it is the only
//   writer of 0x005C8A4C - its 0x28-byte extent agrees with the extent sys/native_4871b0.cpp recorded for that block. The id slot 0x005C8A74 is
//   declared as the two 16-bit words it holds (image dword 0x00000000), so retail's 16-bit store and its three 16-bit re-loads are plain 16-bit
//   accesses with no type punning. 0x00540B5C is owned by sys/native_487190.cpp and 0x005C8A4C by sys/native_4871b0.cpp, so this file declares
//   them extern and defines only what no one else owns (0x005C8A74 and the image source block 0x00540B70). Handlers 0x004869B0, 0x00486CD0,
//   0x00486D70 and 0x00486F30 are raw code immediates in retail and are not transcribed yet (native gaps, all inside 0x0046xxxx-0x0048xxxx).
//   The meaning of the 0x181 argument to QTask_AllocateEvent and of the copied ten dwords is not established. Domain: core/, because it is one of
//   the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint32_t Core_GetEventWord_4E7530(void);                            // 0x004E7530: argument-less dword getter
extern "C" void QFile_RegisterAlias_4B12F0(const char* key, const char* target);    // 0x004B12F0 (qfile/native_4b12f0.cpp)

extern "C" {

// 0x00540B5C: one byte, owned and defined by sys/native_487190.cpp; this body is one of its writers.
extern std::uint8_t g_540b5c;

// 0x005C8A4C: 0x28 bytes, owned and defined by sys/native_4871b0.cpp; this body is its only filler.
extern std::uint8_t g_5c8a4c[0x28];

// Retail 0x005C8A74 is a 4-byte slot whose image dword is 0x00000000, and retail stores and re-loads only its low word (0x00486920
// `mov word [0x5C8A74],ax`, then `mov ax/cx/dx,[0x5C8A74]` at 0x00486930/0x00486943/0x00486957/0x00486979). It is declared as the two 16-bit
// words the slot actually holds, so the byte layout is identical and every store and load is a plain 16-bit access.
std::uint16_t g_5c8a74[2] = {0x0000U, 0x0000U}; // retail 0x005C8A74 (low word: the "StatusScreen" task type id) and 0x005C8A76 (high word)

// 0x00540B70: the ten image dwords this body copies; the values are the raw image bytes, not resolved pointers.
std::uint32_t g_540b70[10] = {0x00540C98U, 0x00540C78U, 0x00540C54U, 0x00540C3CU, 0x00540C18U,
                              0x00540C00U, 0x00540BE4U, 0x00540BC8U, 0x00540BB0U, 0x00540B98U};

void Task_DefineStatusScreen()
{
    // 0x00486900/0x00486901: push esi; push edi (the register saves the copy below needs, restored by the two pops at the end);
    // 0x00486902/0x00486904: push 0; push 0 (inert arguments of the getter, and TaskType_Register's fourth/fifth slots);
    // 0x00486906: call 0x4018F0; 0x0048690B: push eax; 0x0048690C: push 0x4F0; 0x00486911: push "StatusScreen"; 0x00486916: call 0x401900.
    const std::uint16_t class_id = TaskType_Register("StatusScreen", // 0x00486911: push 0x00540D30
                                                     0x4F0U,          // 0x0048690C: push 0x4F0 (object size)
                                                     QTask_GetCurrentType(), // 0x00486906: call 0x4018F0 (parent type)
                                                     0U);            // 0x00486904: push 0 (task list count)

    // 0x0048691B: push 0x181 (QTask_AllocateEvent's argument); 0x00486920: mov word [0x5C8A74],ax;
    // 0x00486926: call 0x4017C0; 0x0048692B: mov byte [0x540B5C],al.
    ::g_5c8a74[0] = class_id; // 0x00486920: the id is stored between the register call and the allocate call, exactly as retail orders it
    ::g_540b5c = static_cast<std::uint8_t>(QTask_AllocateEvent(0x181U)); // 0x0048691B: push 0x181; 0x0048692B: the low byte is what is stored

    // 0x00486930: mov ax,[0x5C8A74]; 0x00486936: push 0x004869B0 (pre-pushed third argument); 0x0048693B: push 0;
    // 0x0048693D: push eax; 0x0048693E: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a74[0]), // 0x00486930: mov ax,[0x5C8A74]
                                0U,                                     // 0x0048693B: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004869B0U))); // 0x00486936: push handler

    // 0x00486943: mov cx,[0x5C8A74]; 0x0048694A: push 0x00486CD0 (pre-pushed third argument); 0x0048694F: push 1;
    // 0x00486951: push ecx; 0x00486952: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8a74[0]), // 0x00486943: mov cx,[0x5C8A74]
                                           1U,                                     // 0x0048694F: push 1
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00486CD0U))); // 0x0048694A: push handler

    // 0x00486957: mov dx,[0x5C8A74]; 0x0048695E: push 0x00486D70 (pre-pushed third argument); 0x00486963: push 2;
    // 0x00486965: push edx; 0x00486966: call 0x401530; 0x0048696B: add esp,0x3c - 15 pushes above.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8a74[0]), // 0x00486957: mov dx,[0x5C8A74]
                                           2U,                                     // 0x00486963: push 2
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00486D70U))); // 0x0048695E: push handler

    // 0x0048696E: push 0x00486F30 (pre-pushed third argument, and the getter's inert argument); 0x00486973: call 0x4E7530 (event id);
    // 0x00486978: push eax; 0x00486979: mov ax,[0x5C8A74]; 0x0048697F: push eax; 0x00486980: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8a74[0]), // 0x00486979: mov ax,[0x5C8A74]
                                           static_cast<std::uint32_t>(Core_GetEventWord_4E7530()), // 0x00486978: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00486F30U))); // 0x0048696E: push handler

    // 0x00486985: push "LOCAL:screens/game/status/" (argument 2); 0x0048698A: push "STATUSSCREEN:" (argument 1, pushed last);
    // 0x0048698F: call 0x4B12F0; 0x004869A3: add esp,0x14 - this call's 2 pushes plus the event-3 link's 3.
    QFile_RegisterAlias_4B12F0("STATUSSCREEN:",             // 0x0048698A: push 0x00540D04 (argument 1: the key)
                               "LOCAL:screens/game/status/"); // 0x00486985: push 0x00540D14 (argument 2: the target)

    // 0x00486994: mov $0xA,%ecx; 0x00486999: mov $0x540B70,%esi; 0x0048699E: mov $0x5C8A4C,%edi; 0x004869A6: rep movsl - ten dword moves from
    // the image block to the block at 0x005C8A4C. The loop below is that copy; retail's esi/edi are the two blocks, not a caller's registers.
    for (std::int32_t i = 0; i < 10; ++i) // 0x00486994: mov $0xA,%ecx
    {
        reinterpret_cast<std::uint32_t*>(::g_5c8a4c)[i] = ::g_540b70[i]; // 0x004869A6: rep movsl
    }

    // 0x004869A8/0x004869A9: pop edi; pop esi (restoring the two saves at 0x00486900/0x00486901); 0x004869AA: ret.
} // end of Task_DefineStatusScreen

} // extern "C"
