// Native: Task_DefineAddAmmo (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0046DB40-0x0046DC1F.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 224 bytes, 1 block, 67 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 67 instructions / 224 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x0046DB40-0x0046DC1F; the ret is at 0x0046DC1F, and 0x0046DC20 is the next
//   function, which this body uses as a handler); `ps` at 0x0053FE60 for the name literal (byte-identical to "AddAmmo"); `pxw` at 0x005BE30C for
//   the image value; a stack-pointer simulation over all 67 instructions, which returns esp to its entry value at the ret and accounts for every
//   cleanup (0x38 = 14 pushes for the two inert arguments, the registration and the first two links; 0xc twice; 0x10 once = the second link's
//   three pushes plus the container count; 0x18 once for the first append's six pushes; 0x60 once = the 0x48 frame plus the second append's six
//   pushes) and which resolved both `lea reg,[esp+N]` pairs to frame-relative addresses. Every callee was re-read at its own VA, which is how
//   0x004F1E20 was established as an argument-less load despite being called with two pushes. Argument numbers were derived by counting the
//   pushes above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body with a 0x48-byte frame. It registers the "AddAmmo" task type of size 0x48 with parent
//   QTask_GetCurrentType() -> id at 0x005BE30C, links e0->0x0046DC20 (0x00401400), e1->0x0046DC40 and e2->0x004D3710 (0x00401530), then links
//   0x004F1A60->0x0046DCA0 and 0x004F1A70->0x0046DCB0 (0x00401400). It allocates a TWO-entry parameter container through 0x004F1950 into
//   0x005BE310 and appends two 16-byte records through 0x004F1A80. The first record's type is NOT a constant getter: it is
//   0x004F1E20(0x0047CF10(), 7), i.e. the value of the global 0x0054E158 selected by the address 0x005C10F0 that 0x0047CF10 returns, and the
//   address 0x005C10F0 is also the record's sixth argument. The second record's type is the plain argument-less load 0x004F0FB0 and its sixth
//   argument is 0. Recorded deltas 0x24 and 0x44. One basic block, no branch, no loop, 15 call sites over 11 distinct callees. There is NO
//   0x004B8810 script-name tail in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x005BE30C holds image value 0 and is one of the image-zero slots in the
//   0x005BE3xx cluster. The container is allocated with count 2 and exactly two records are appended. 0x0054E158 holds image value 0xFFFFFFFF
//   and is the value 0x004F1E20 returns; 0x004F1A80 treats a type of -1 as "Undefined parameter type." and spins, so at runtime that global must
//   already hold a valid type when this registrar runs - it is written by the 0x004F1E35 initializer before the 0x00402930 registrar sequence,
//   which is why the image value is only the pre-initialisation state and not the value seen here. The same shape (0x004F1E20 fed by an
//   address-returning getter and followed by 7) already appears in this lane's accepted 0x00477E30, where the getter is 0x00413DE0 and the
//   address is 0x0056E228. The sixth argument of 0x004F1A80 is transcribed here: retail's callee reads SIX stack arguments and stores arg6 at
//   record +0x08 and byte(arg5) at record +0x0C, while the tree's current implementation of 0x004F1A80 declares five and writes arg5 into both
//   fields. That implementation is above 0x0048FFFF and is not in this lane's scope; this file passes the correct sixth argument (0x005C10F0
//   for the first record, 0 for the second) and the field will stay wrong until the implementation is corrected. Because the symbol has C
//   linkage, the extra argument links against the five-parameter definition and is ignored, so nothing here breaks the link. The two deltas
//   were resolved rather than read: the frame is 0x48 and at each `lea` the two inert pushes plus the getter result are on the stack (12 bytes
//   for the second, 20 for the first because its type comes from a two-argument 0x004F1E20 call), so raw 0x2C and raw 0x4C are not offsets and
//   only the resolved values 0x24 and 0x44 are. Documented shape differences, all value-preserving: (1) every getter callee is an argument-less
//   load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not
//   reproduced; each is simultaneously the next 0x00401400/0x00401530 link call's pre-pushed handler, which the port keeps as the handler value
//   in the same order, and for the two appends they are the values that become the append's sixth and fifth arguments; (2) byte-valued event ids
//   are zero-extended in the port, which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400 or
//   0x00401530, which mask the event word for the phase index; (3) retail's 0x48-byte frame is reproduced as a single local slot whose address
//   stands in for the frame base, the same convention the accepted 0x004813E0 and 0x00470FA0 files use - only the two addresses passed to
//   0x004F1A80 matter and the callee stores their difference, and nothing in this body reads the intervening bytes. Handlers 0x0046DC20,
//   0x0046DC40, 0x0046DCA0 and 0x0046DCB0 are raw code immediates in retail and are not transcribed yet (native gaps in
//   0x0046xxxx-0x0048xxxx); handler 0x004D3710 is referenced 163 times in the image as a generic filler and is implemented elsewhere. The getter
//   0x0047CF10 (`mov eax,0x5C10F0; ret`) is in this lane's range and has no implementation file yet. Domain: core/, because it is one of the
//   0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint32_t Sys_GetGlobalPointer_47CF10(void);       // 0x0047CF10: `mov eax,0x5C10F0; ret` (in-lane native gap)
extern "C" std::uint32_t Sys_GetGlobal32_4F1E20(void);            // 0x004F1E20: `mov eax,[0x54E158]; ret` (ignores both pushed arguments)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FB0(void);            // 0x004F0FB0: `mov eax,[0xA70C78]; ret`
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script_state.hpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). Six arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08). The tree's
// implementation still declares five and writes arg5 into both fields; see the status block above.
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x005BE30C: the "AddAmmo" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be30c = 0U;

// 0x005BE310: the two-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_5be310 = 0U;

void Task_DefineAddAmmo()
{
    // 0x0046DB40: sub esp,0x48 - retail's 72-byte frame, which is the registered object size. The two 0x004F1A80 calls below pass
    // frame+0x00 and frame+0x24 / frame+0x44; the callee stores the differences 0x24 and 0x44. A single local slot stands in for the frame
    // base, which is the convention the accepted 0x004813E0 and 0x00470FA0 files use.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0046DB43/0x0046DB45: push 0; push 0 (both inert arguments of the parent getter); 0x0046DB47: call 0x4018F0; 0x0046DB4C: push eax;
    // 0x0046DB4D: push 0x48; 0x0046DB4F: push "AddAmmo"; 0x0046DB54: call 0x401900.
    const std::uint16_t add_ammo_id = TaskType_Register("AddAmmo", // 0x0046DB4F: push 0x0053FE60
                                                        0x48U,      // 0x0046DB4D: push 0x48 (object size, equal to the frame)
                                                        QTask_GetCurrentType(), // 0x0046DB47: call 0x4018F0 (parent type)
                                                        0U);        // 0x0046DB45: push 0 (inert)

    // 0x0046DB59: push 0x0046DC20 (pre-pushed handler); 0x0046DB5E: push 0; 0x0046DB60: push eax;
    // 0x0046DB61: mov word [0x5BE30C],ax (the store lands between the pushes and the call); 0x0046DB67: call 0x401400.
    ::g_5be30c = add_ammo_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be30c), // 0x0046DB6C: mov ax,[0x5BE30C]
                                0U,                                     // 0x0046DB5E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046DC20U))); // 0x0046DB59: push handler

    // 0x0046DB72: push 0x0046DC40; 0x0046DB77: push 1; 0x0046DB79: push eax; 0x0046DB7A: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be30c), // 0x0046DB6C: mov ax,[0x5BE30C]
                                           1U,                                     // 0x0046DB77: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046DC40U))); // 0x0046DB72: push handler

    // 0x0046DB7F: mov cx,[0x5BE30C]; 0x0046DB86: push 0x004D3710; 0x0046DB8B: push 2; 0x0046DB8D: push ecx; 0x0046DB8E: call 0x401530;
    // 0x0046DB93: add esp,0x38 - the fourteen pushes above (two inert, three registration, three per link).
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be30c), // 0x0046DB7F: mov cx,[0x5BE30C]
                                           2U,                                     // 0x0046DB8B: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x0046DB86: push handler

    // 0x0046DB96: push 0x0046DCA0; 0x0046DB9B: call 0x4F1A60; 0x0046DBA0: mov dx,[0x5BE30C]; 0x0046DBA7: push eax; 0x0046DBA8: push edx;
    // 0x0046DBA9: call 0x401400; 0x0046DBAE: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be30c), // 0x0046DBA0: mov dx,[0x5BE30C]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x0046DBA7: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046DCA0U))); // 0x0046DB96: push handler

    // 0x0046DBB1: push 0x0046DCB0; 0x0046DBB6: call 0x4F1A70; 0x0046DBBB: push eax; 0x0046DBBC: mov ax,[0x5BE30C]; 0x0046DBC2: push eax;
    // 0x0046DBC3: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be30c), // 0x0046DBBC: mov ax,[0x5BE30C]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x0046DBBB: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046DCB0U))); // 0x0046DBB1: push handler

    // 0x0046DBC8: push 2 (Script_TaskContainer_Alloc's argument); 0x0046DBCA: call 0x4F1950;
    // 0x0046DBCF: add esp,0x10 - the last link's three pushes plus this argument; 0x0046DBD2: mov [0x5BE310],eax. The count 2 matches the
    // two appends below.
    ::g_5be310 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(2));

    // 0x0046DBD7: call 0x0047CF10 (eax = 0x005C10F0); 0x0046DBDC: push eax (inert for 0x4F1E20, and the append's sixth argument);
    // 0x0046DBDD: push 7 (inert, and the append's fifth argument); 0x0046DBDF: call 0x4F1E20 (eax = the global 0x0054E158);
    // 0x0046DBE4: lea ecx,[esp+0x2C] -> frame+0x24; 0x0046DBE8: push eax (the record type); 0x0046DBE9: mov eax,[0x5BE310] (the container);
    // 0x0046DBEE: lea edx,[esp+0xC] -> frame+0x00; 0x0046DBF2: push ecx; 0x0046DBF3: push edx; 0x0046DBF4: push eax;
    // 0x0046DBF5: call 0x4F1A80; 0x0046DBFA: add esp,0x18.
    const std::uint32_t add_ammo_type_address = Sys_GetGlobalPointer_47CF10(); // 0x0046DBD7: call 0x47CF10
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be310)),
                                append_frame, append_frame + 0x24,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1E20()), // 0x0046DBDF: call 0x4F1E20 (type)
                                7,                                                    // 0x0046DBDD: push 7
                                static_cast<std::int32_t>(add_ammo_type_address));    // 0x0046DBDC: push 0x005C10F0

    // 0x0046DBFD/0x0046DBFF: push 0/7; 0x0046DC01: call 0x4F0FB0; 0x0046DC06: lea ecx,[esp+0x4C] -> frame+0x44; 0x0046DC0A: push eax;
    // 0x0046DC0B: mov eax,[0x5BE310]; 0x0046DC10: lea edx,[esp+0xC] -> frame+0x00; 0x0046DC14: push ecx; 0x0046DC15: push edx;
    // 0x0046DC16: push eax; 0x0046DC17: call 0x4F1A80. This append has no cleanup of its own.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be310)),
                                append_frame, append_frame + 0x44,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FB0()), 7, 0);

    // 0x0046DC1C: add esp,0x60 - the 0x48 frame plus the second append's six pushes; 0x0046DC1F: ret.
} // end of Task_DefineAddAmmo

} // extern "C"
