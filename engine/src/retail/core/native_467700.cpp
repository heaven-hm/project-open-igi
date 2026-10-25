// Native: Task_DefineComputerObject (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00467700-0x004677FE.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 255 bytes, 1 block, 67 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 67 instructions / 255 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00467700-0x004677FE; the ret is at 0x004677FE and 0x00467800 is the next
//   function, which this body uses as a handler); `ps` at 0x0053F6C0 and at the seven 0x0053F644-0x0053F6B4 literals for the names and their
//   bytes; `px 8 @ 0x0053F580` for the id slot and the three byte slots (81 01 ff ff ff 00 00 00, so the id is 0x0181 and 0x0053F582,
//   0x0053F583 and 0x0053F584 are each 0xFF); a stack-pointer simulation over all 67 instructions, which returns esp to its entry value at the
//   ret and accounts for every cleanup (0x2C = eleven pushes: the two zeros, the parent-getter result, the size, the name, and the three pushes
//   of each of the first two links; 0xC twice; 0x44 = seventeen pushes: the third link's three plus two for each of the seven
//   script-registration tails) - note this body has NO frame, so every cleanup is pure argument space. Every callee was re-read at its own VA,
//   which is how 0x004E7530, 0x00467D90, 0x00467DB0, 0x00467DD0, 0x004F45F0 and 0x004C48C0 were established as argument-less loads despite
//   being called with pushes. Argument numbers were derived by counting the pushes above each access.
// Behavior/CFG: one straight-line body with no frame of its own. It registers the "ComputerObject" task type of size 0x24 with parent
//   QTask_GetCurrentType() -> id at 0x0053F580, links e0->0x004678F0 (0x00401400) and **event 2**->0x00467960 (0x00401530 - event 1 is
//   skipped), then links 0x00467970 on the event id from 0x004E7530 through 0x00401530. It then calls the three argument-less byte getters
//   0x00467D90, 0x00467DB0 and 0x00467DD0 and stores each returned byte back into 0x0053F582, 0x0053F583 and 0x0053F584 respectively - the two
//   of those three getters that read exactly those slots (0x00467DB0 reads 0x0053F583, 0x00467DD0 reads 0x0053F584), so those two stores write
//   back the value they have just read. It then links two further handlers THROUGH 0x00401400 to the type id that 0x004F45F0() and 0x004C48C0()
//   return, not to its own id: 0x00467C60 on the event id from 0x00467D90, and 0x00467800 on the event id from 0x00467DD0. Finally it registers
//   SEVEN script names through 0x004B8810: "MARKER_NONE" with 0xFFFFFFFF, "MARKER_BOX" with 0, "MARKER_COLOR_NONE" with 0xFFFFFFFF,
//   "MARKER_COLOR_RED" with 0, "MARKER_COLOR_YELLOW" with 1, "MARKER_COLOR_GREEN" with 2 and "MARKER_COLOR_BLUE" with 4. One basic block, no
//   branch, no loop, 22 call sites over 11 distinct callees. It allocates no parameter container, appends no records and allocates no event id
//   of its own through QTask_AllocateEvent.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x0053F580 is declared std::uint16_t and initialised to 0x0181 because that is its
//   IMAGE VALUE: `px 8 @ 0x0053F580` gives 81 01 ff ff ff 00 00 00. Its accesses are 16-bit (0x0046771E `mov %ax,0x53F580`, 0x00467729
//   `mov 0x53F580,%ax`, 0x00467749 `%cx`), so width and image value are both established the same way as for 0x0053F6D0 in
//   core/native_4682f0.cpp, and the same non-zero image value makes it initialised data (`D`). 0x0053F582, 0x0053F583 and 0x0053F584 each hold
//   image value 0xFF and are declared std::uint8_t with that initialiser; the registrar writes all three from the getters' return values, and
//   0x00467DB0/0x00467DD0 read the latter two, so this file declares the byte slots and the two in-lane getters that read them. The two
//   0x00401400 links in the middle are NOT linked to this type: their argument 1 comes from 0x004F45F0 (`mov ax,[0x54E200]`) and 0x004C48C0
//   (`mov ax,[0x547F42]`), and radare2 reports 385 = 0x0181 at both globals - the same value this id slot holds in the image, which is recorded as
//   an observation and not as a claim about what 0x0181 means. This is the first registrar in this lane that links handlers to a type other than
//   the one it defines, and it is transcribed exactly as the instructions order it. The seven 0x004B8810 tails are (name, value) pairs in retail
//   push order (the name is the last push, so it is argument 1), with 0xFFFFFFFF passed as a raw bit pattern for the two "NONE" names and small
//   integers for the rest; the values are passed through as void* exactly as the tree's other 0x004B8810 call sites do. Documented shape
//   differences, all value-preserving: (1) the getters are argument-less loads in retail that ignore the arguments the caller pushes, and the
//   tree's transcriptions take no arguments, so those inert pushes are not reproduced; each is simultaneously the next 0x00401400/0x00401530
//   link call's pre-pushed handler, which the port keeps as the handler value in the same order; (2) byte-valued event ids are zero-extended in
//   the port, which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask the
//   event word for the phase index; (3) the deferred cleanup is reproduced implicitly by the tree's calling convention rather than by explicit
//   `add esp` arithmetic, since the port never pushes the inert arguments; the retail size 0x44 is recorded here so a future reviewer can check
//   the arithmetic. Handlers 0x004678F0, 0x00467960, 0x00467970, 0x00467C60 and 0x00467800 are raw code immediates in retail and are not
//   transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx; 0x00467800 was read far enough to confirm it is a separate function with its own
//   extent). Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint32_t Core_GetEventWord_4E7530(void);          // 0x004E7530: `mov eax,[0x5488C8]; ret` (event id)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);       // 0x00467D90: lazily allocated event id (core/native_467d90.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_467DB0(void);              // 0x00467DB0: `mov al,[0x53F583]; ret` (in-lane native gap)
extern "C" std::uint8_t Sys_GetGlobal8_467DD0(void);              // 0x00467DD0: `mov al,[0x53F584]; ret` (in-lane native gap)
extern "C" std::uint16_t Sys_GetGlobal16_4F45F0(void);            // 0x004F45F0: `mov ax,[0x54E200]; ret` (image 385 = 0x0181)
extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);            // 0x004C48C0: `mov ax,[0x547F42]; ret` (image 385 = 0x0181)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0053F580: the "ComputerObject" task type id. Image value 0x0181, so it is initialised data (`D`); the accesses are 16-bit.
std::uint16_t g_53f580 = 0x0181U;

// 0x0053F582: image value 0xFF. Written here from 0x00467D90's return value.
std::uint8_t g_53f582 = 0xFFU;

// 0x0053F583: image value 0xFF. Written here from 0x00467DB0's return value, and read by 0x00467DB0 itself.
std::uint8_t g_53f583 = 0xFFU;

// 0x0053F584: image value 0xFF. Written here from 0x00467DD0's return value, and read by 0x00467DD0 itself.
std::uint8_t g_53f584 = 0xFFU;

void Task_DefineComputerObject()
{
    // 0x00467700/0x00467702: push 0; push 0 (the parent getter's inert argument, then TaskType_Register's fourth argument);
    // 0x00467704: call 0x4018F0; 0x00467709: push eax; 0x0046770A: push 0x24; 0x0046770C: push "ComputerObject"; 0x00467711: call 0x401900.
    const std::uint16_t computer_object_id = TaskType_Register("ComputerObject", // 0x0046770C: push 0x0053F6C0
                                                               0x24U,             // 0x0046770A: push 0x24 (object size)
                                                               static_cast<std::uint16_t>(QTask_GetCurrentType()), // 0x00467704: call 0x4018F0
                                                               0U);              // 0x00467702: push 0 (fourth argument)

    // 0x00467716: push 0x004678F0 (pre-pushed handler); 0x0046771B: push 0; 0x0046771D: push eax; 0x0046771E: mov word [0x53F580],ax (the id
    // store lands between the pushes); 0x00467724: call 0x401400.
    ::g_53f580 = computer_object_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53f580), // 0x0046771E: mov ax,[0x53F580]
                                0U,                                     // 0x0046771B: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004678F0U))); // 0x00467716: push handler

    // 0x00467729: mov ax,[0x53F580]; 0x0046772F: push 0x00467960; 0x00467734: push 2; 0x00467736: push eax; 0x00467737: call 0x401530;
    // 0x0046773C: add esp,0x2C - eleven pushes: the two zeros, the parent-getter result, the size, the name, and the three pushes of each of the
    // first two links. NOTE the argument is 2, not 1: this registrar has NO event-1 handler.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53f580), // 0x00467729: mov ax,[0x53F580]
                                           2U,                                     // 0x00467734: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00467960U))); // 0x0046772F: push handler

    // 0x0046773F: push 0x00467970; 0x00467744: call 0x4E7530; 0x00467749: mov cx,[0x53F580]; 0x00467750: push eax; 0x00467751: push ecx;
    // 0x00467752: call 0x401530; 0x00467757: add esp,0xC.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53f580), // 0x00467749: mov cx,[0x53F580]
                                           static_cast<std::uint32_t>(Core_GetEventWord_4E7530()), // 0x00467750: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00467970U))); // 0x0046773F: push handler

    // 0x0046775A: call 0x467D90; 0x0046775F: mov byte [0x53F582],al.
    ::g_53f582 = Task_GetCachedEventId_467D90();

    // 0x00467764: call 0x467DB0; 0x00467769: mov byte [0x53F583],al. 0x00467DB0 itself reads 0x0053F583, so this stores back the value it read.
    ::g_53f583 = Sys_GetGlobal8_467DB0();

    // 0x0046776E: call 0x467DD0; 0x00467773: mov byte [0x53F584],al. 0x00467DD0 itself reads 0x0053F584, so this stores back the value it read.
    ::g_53f584 = Sys_GetGlobal8_467DD0();

    // 0x00467773: push 0x00467C60 (pre-pushed handler); 0x0046777D: call 0x467D90; 0x00467782: push eax (argument 2, the event id);
    // 0x00467783: call 0x4F45F0; 0x00467788: push eax (argument 1, the type id); 0x00467789: call 0x401400; 0x0046778E: add esp,0xC. Argument 1
    // is NOT this registrar's id: it is the word at 0x0054E200, which holds 0x0181 in the image.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(Sys_GetGlobal16_4F45F0()), // 0x00467788: push eax (argument 1)
                                static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()), // 0x00467782: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00467C60U))); // 0x00467773: push handler

    // 0x00467791: push 0x00467800 (pre-pushed handler); 0x00467796: call 0x467DD0; 0x0046779B: push eax (argument 2, the event id);
    // 0x0046779C: call 0x4C48C0; 0x004677A1: push eax (argument 1, the type id); 0x004677A2: call 0x401400. Same shape as the previous link, on
    // the word at 0x00547F42, which also holds 0x0181 in the image. This link has no cleanup of its own: the final add esp,0x44 covers it together
    // with the seven script-registration tails.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(Sys_GetGlobal16_4C48C0()), // 0x004677A1: push eax (argument 1)
                                static_cast<std::uint32_t>(Sys_GetGlobal8_467DD0()), // 0x0046779B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00467800U))); // 0x00467791: push handler

    // 0x004677A7/0x004677A9: push 0xFFFFFFFF; push "MARKER_NONE"; 0x004677AE: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("MARKER_NONE"), // 0x004677A9: push 0x0053F6B4 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x004677A7: push 0xFFFFFFFF (argument 2)

    // 0x004677B3/0x004677B5: push 0; push "MARKER_BOX"; 0x004677BA: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("MARKER_BOX"), // 0x004677B5: push 0x0053F6A8 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x004677B3: push 0 (argument 2)

    // 0x004677BF/0x004677C1: push 0xFFFFFFFF; push "MARKER_COLOR_NONE"; 0x004677C6: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("MARKER_COLOR_NONE"), // 0x004677C1: push 0x0053F694 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x004677BF: push 0xFFFFFFFF (argument 2)

    // 0x004677CB/0x004677CD: push 0; push "MARKER_COLOR_RED"; 0x004677D2: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("MARKER_COLOR_RED"), // 0x004677CD: push 0x0053F680 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x004677CB: push 0 (argument 2)

    // 0x004677D7/0x004677D9: push 1; push "MARKER_COLOR_YELLOW"; 0x004677DE: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("MARKER_COLOR_YELLOW"), // 0x004677D9: push 0x0053F66C (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // 0x004677D7: push 1 (argument 2)

    // 0x004677E3/0x004677E5: push 2; push "MARKER_COLOR_GREEN"; 0x004677EA: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("MARKER_COLOR_GREEN"), // 0x004677E5: push 0x0053F658 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(2U))); // 0x004677E3: push 2 (argument 2)

    // 0x004677EF/0x004677F1: push 4; push "MARKER_COLOR_BLUE"; 0x004677F6: call 0x4B8810;
    // 0x004677FB: add esp,0x44 - seventeen pushes: this link's three plus two for each of the seven tails; 0x004677FE: ret.
    Script_QHashNodeRegister(const_cast<char*>("MARKER_COLOR_BLUE"), // 0x004677F1: push 0x0053F644 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(4U))); // 0x004677EF: push 4 (argument 2)
} // end of Task_DefineComputerObject

} // extern "C"
