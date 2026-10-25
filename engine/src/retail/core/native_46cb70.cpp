// Native: Task_DefineGunPickup (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0046CB70-0x0046CD18.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 425 bytes, 1 block, 118 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 118 instructions / 425 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x0046CB70-0x0046CD18; the ret is at 0x0046CD18, seven nops follow, and
//   0x0046CD20 is the next function, which this body uses as a handler); `ps` at 0x0053FDE8 for the name literal (byte-identical to
//   "GunPickup"); `px`/`pxw` at 0x0053FDE0, 0x005BE2E0 and 0x005BE2F8 for the image values; a stack-pointer simulation over all 118
//   instructions, which returns esp to its entry value at the ret and accounts for every cleanup (0x3C = the two inert arguments, the three
//   registration arguments, the QTask_AllocateEvent argument and three three-push links; 0xC six times; 0x10 once = the 0x4167E0 link's three
//   pushes plus the container count; 0x170 = the 0x158 frame plus the last append's six pushes) and which resolved all three `lea reg,[esp+N]`
//   pairs to frame-relative addresses. Every callee was re-read at its own VA, which is how 0x004138E0-style getters, 0x00416850, 0x00416880,
//   0x004167E0, 0x004F1210, 0x004F1220 and 0x004F1E20 were established as argument-less loads despite being called with pushes. Argument numbers
//   were derived by counting the pushes above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body with a 0x158-byte frame. It registers the "GunPickup" task type of size 0x158 with parent
//   Sys_GetGlobal16_4E0830() -> id at 0x005BE2E0, allocates one event id through QTask_AllocateEvent(0) and stores its low byte at 0x0053FDE0
//   (image value 0xFF), then links ten handlers: e0->0x0046CD20 (0x00401400), e1->0x0046CEA0 and e2->0x0046D760 (0x00401530),
//   0x004F1A60->0x0046D040 and 0x004F1A70->0x0046D050 (0x00401400), 0x004F1210->0x0046D170 and 0x004F1220->0x0046D1D0 (0x00401530), then
//   three more through 0x00401400 whose event ids come from the argument-less byte getters 0x00416850, 0x00416880 and 0x004167E0 and whose
//   handlers are 0x0045F920 (twice, the second event of the same handler) and 0x0046D230. It allocates a THREE-entry parameter container through
//   0x004F1950 into 0x005BE2F8 and appends three 16-byte records through 0x004F1A80 with recorded deltas 0xF0, 0x70 and 0x130. The third
//   record's type is not a constant getter: it is 0x004F1E20(0x00413DE0(), 7), the same computed-type shape the accepted 0x0046DB40 and
//   0x0046D420 use, and the address 0x0056E228 that 0x00413DE0 returns is that record's sixth argument. One basic block, no branch, no loop,
//   28 call sites over 18 distinct callees. There is NO 0x004B8810 script-name tail in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x005BE2E0 and the container slot 0x005BE2F8 both hold image value 0; the byte at
//   0x0053FDE0 holds image value 0xFF and is written by the QTask_AllocateEvent result at 0x0046CB9C, three bytes before the "GunPickup" literal
//   at 0x0053FDE8 (the second case of a name literal adjacent to the allocated-event byte, after 0x0046D420's 0x0053FE28/0x0053FE2C). The
//   container is allocated with count 3 and exactly three records are appended. This body and the accepted 0x0046D420 share a parent, the same
//   event getters 0x004F1A60/0x004F1A70/0x004F1210/0x004F1220, the same byte getters 0x00416850/0x00416880/0x004167E0 with the same 0x0045F920
//   handler, the same two record getters 0x004F1010/0x004F0F70 and the same first two deltas 0xF0/0x70 - but they do NOT share a link set: the
//   frame sizes differ (0x158 against 0x138), the e0 and e1 handlers differ, 0x0046D420 has a fourth append at delta 0x110 that this body does
//   not, and their computed-type records use different getters (0x0047CF10 here versus 0x00413DE0). The shared 0x0046D760 is registered as
//   event 2 by BOTH types, so it is an event-2 handler shared across two task types rather than a generic filler. The sixth argument of 0x004F1A80
//   is transcribed here (0 at two appends, 0x0056E228 at the third) with the corrected six-argument shape: the callee reads arg1..arg4, arg5 as a
//   byte (record +0x0C) and arg6 as a dword (record +0x08). Documented shape differences, all value-preserving: (1) the getters are
//   argument-less loads in retail that ignore the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; each is simultaneously the next 0x00401400/0x00401530 link call's pre-pushed handler, which the port keeps as the
//   handler value in the same order, and for the three appends they are the values that become the append's sixth and fifth arguments; (2)
//   byte-valued event ids are zero-extended in the port, which cannot change behaviour because every link fed by a byte getter here goes through
//   0x00401400 or 0x00401530, which mask the event word for the phase index; (3) retail's 0x158-byte frame is reproduced as a single local slot
//   whose address stands in for the frame base, the same convention the accepted 0x004813E0, 0x00470FA0, 0x0046DB40 and 0x0046D420 files use -
//   only the two addresses passed to 0x004F1A80 matter and the callee stores their difference, and nothing in this body reads the intervening
//   bytes. Handlers 0x0046CD20, 0x0046CEA0, 0x0046D040, 0x0046D050, 0x0046D170, 0x0046D1D0 and 0x0046D230 are raw code immediates in retail and
//   are not transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx); 0x0046D760 belongs to 0x0046D420's extent neighbourhood and 0x0045F920 lies
//   below 0x00460000 (other lane). Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4E0830(void);            // 0x004E0830: `mov ax,[0x548698]; ret` (parent type)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);            // 0x004F1210: `mov eax,[0x54DDA8]; ret` (event id)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);            // 0x004F1220: `mov eax,[0x54DDAC]; ret` (event id)
extern "C" std::uint8_t Sys_GetSettingByte_416850(void);          // 0x00416850: `mov al,[0x539548]; ret` (event id; ignores its push)
extern "C" std::uint8_t Sys_GetSettingByte_416880(void);          // 0x00416880: `mov al,[0x53954D]; ret` (event id; ignores its push)
extern "C" std::uint8_t Sys_GetSettingByte_4167E0(void);          // 0x004167E0: `mov al,[0x539541]; ret` (event id; ignores its push)
extern "C" std::uint32_t Sys_GetGlobalPtr_413DE0(void);           // 0x00413DE0: `mov eax,0x56E228; ret` (below 0x00460000, other lane)
extern "C" std::uint32_t Sys_GetGlobal32_4F1E20(void);            // 0x004F1E20: `mov eax,[0x54E158]; ret` (ignores both pushed arguments)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);            // 0x004F0F70: `mov eax,[0xA70C6C]; ret`
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script_state.hpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08); the two
// reads are both `[esp+0x20]` in retail at different push depths with a `pop edi` between them.
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x005BE2E0: the "GunPickup" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be2e0 = 0U;

// 0x005BE2F8: the three-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_5be2f8 = 0U;

// 0x0053FDE0: the low byte of the event id allocated by QTask_AllocateEvent(0). Image value 0xFF, written at 0x0046CB9C by a byte store, and
// the "GunPickup" name literal starts eight bytes later at 0x0053FDE8.
std::uint8_t g_53fde0 = 0xFFU;

void Task_DefineGunPickup()
{
    // 0x0046CB70: sub esp,0x158 - retail's 344-byte frame, which is the registered object size. The three 0x004F1A80 calls below pass frame+0x00
    // and frame+0xF0 / +0x70 / +0x130; the callee stores the differences. A single local slot stands in for the frame base, which is the
    // convention the accepted 0x004813E0, 0x00470FA0, 0x0046DB40 and 0x0046D420 files use.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0046CB76/0x0046CB78: push 0; push 0 (both inert arguments of the parent getter); 0x0046CB7A: call 0x4E0830; 0x0046CB7F: push eax;
    // 0x0046CB80: push 0x158; 0x0046CB85: push "GunPickup"; 0x0046CB8A: call 0x401900.
    const std::uint16_t gun_pickup_id = TaskType_Register("GunPickup", // 0x0046CB85: push 0x0053FDE8
                                                          0x158U,      // 0x0046CB80: push 0x158 (object size, equal to the frame)
                                                          Sys_GetGlobal16_4E0830(), // 0x0046CB7A: call 0x4E0830 (parent type)
                                                          0U);         // 0x0046CB78: push 0 (inert)

    // 0x0046CB8F: push 0 (QTask_AllocateEvent's argument); 0x0046CB91: mov word [0x5BE2E0],ax (the id store lands between the pushes);
    // 0x0046CB97: call 0x4017C0; 0x0046CB9C: mov byte [0x53FDE0],al - only the low byte of the returned id is kept.
    ::g_5be2e0 = gun_pickup_id;
    ::g_53fde0 = static_cast<std::uint8_t>(QTask_AllocateEvent(0));

    // 0x0046CBA1: mov ax,[0x5BE2E0]; 0x0046CBA7: push 0x0046CD20 (pre-pushed handler); 0x0046CBAC: push 0; 0x0046CBAE: push eax;
    // 0x0046CBAF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2e0), // 0x0046CBA1: mov ax,[0x5BE2E0]
                                0U,                                     // 0x0046CBAC: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046CD20U))); // 0x0046CBA7: push handler

    // 0x0046CBB4: mov cx,[0x5BE2E0]; 0x0046CBBB: push 0x0046CEA0; 0x0046CBC0: push 1; 0x0046CBC2: push ecx; 0x0046CBC3: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be2e0), // 0x0046CBB4: mov cx,[0x5BE2E0]
                                           1U,                                     // 0x0046CBC0: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046CEA0U))); // 0x0046CBBB: push handler

    // 0x0046CBC8: mov dx,[0x5BE2E0]; 0x0046CBCF: push 0x0046D760 - the SAME handler 0x0046D420 registers for its own event 2; 0x0046CBD4:
    // push 2; 0x0046CBD6: push edx; 0x0046CBD7: call 0x401530; 0x0046CBDC: add esp,0x3C - the two inert arguments, the three registration
    // arguments, the QTask_AllocateEvent argument and three three-push links.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be2e0), // 0x0046CBC8: mov dx,[0x5BE2E0]
                                           2U,                                     // 0x0046CBD4: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046D760U))); // 0x0046CBCF: push handler (shared)

    // 0x0046CBDF: push 0x0046D040; 0x0046CBE4: call 0x4F1A60; 0x0046CBE9: push eax; 0x0046CBEA: mov ax,[0x5BE2E0]; 0x0046CBF0: push eax;
    // 0x0046CBF1: call 0x401400; 0x0046CBF6: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2e0), // 0x0046CBEA: mov ax,[0x5BE2E0]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x0046CBE9: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046D040U))); // 0x0046CBDF: push handler

    // 0x0046CBF9: push 0x0046D050; 0x0046CBFE: call 0x4F1A70; 0x0046CC03: mov cx,[0x5BE2E0]; 0x0046CC0A: push eax; 0x0046CC0B: push ecx;
    // 0x0046CC0C: call 0x401400; 0x0046CC11: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2e0), // 0x0046CC03: mov cx,[0x5BE2E0]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x0046CC0A: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046D050U))); // 0x0046CBF9: push handler

    // 0x0046CC14: push 0x0046D170; 0x0046CC19: call 0x4F1210; 0x0046CC1E: mov dx,[0x5BE2E0]; 0x0046CC25: push eax; 0x0046CC26: push edx;
    // 0x0046CC27: call 0x401530; 0x0046CC2C: add esp,0xC.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be2e0), // 0x0046CC1E: mov dx,[0x5BE2E0]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // 0x0046CC25: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046D170U))); // 0x0046CC14: push handler

    // 0x0046CC2F: push 0x0046D1D0; 0x0046CC34: call 0x4F1220; 0x0046CC39: push eax; 0x0046CC3A: mov ax,[0x5BE2E0]; 0x0046CC40: push eax;
    // 0x0046CC41: call 0x401530; 0x0046CC46: add esp,0xC.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be2e0), // 0x0046CC3A: mov ax,[0x5BE2E0]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // 0x0046CC39: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046D1D0U))); // 0x0046CC2F: push handler

    // 0x0046CC49: push 0x0045F920 (the handler, and the inert argument of the byte getter); 0x0046CC4E: call 0x416850;
    // 0x0046CC53: mov cx,[0x5BE2E0]; 0x0046CC5A: push eax; 0x0046CC5B: push ecx; 0x0046CC5C: call 0x401400; 0x0046CC61: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2e0), // 0x0046CC53: mov cx,[0x5BE2E0]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416850()), // 0x0046CC5A: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x0046CC49: push handler (below 0x00460000)

    // 0x0046CC64: push 0x0045F920 (the same handler again, for its second event); 0x0046CC69: call 0x416880; 0x0046CC6E: mov dx,[0x5BE2E0];
    // 0x0046CC75: push eax; 0x0046CC76: push edx; 0x0046CC77: call 0x401400; 0x0046CC7C: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2e0), // 0x0046CC6E: mov dx,[0x5BE2E0]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416880()), // 0x0046CC75: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x0046CC64: push handler (below 0x00460000)

    // 0x0046CC7F: push 0x0046D230; 0x0046CC84: call 0x4167E0; 0x0046CC89: push eax; 0x0046CC8A: mov ax,[0x5BE2E0]; 0x0046CC90: push eax;
    // 0x0046CC91: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2e0), // 0x0046CC8A: mov ax,[0x5BE2E0]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4167E0()), // 0x0046CC89: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046D230U))); // 0x0046CC7F: push handler

    // 0x0046CC96: push 3 (Script_TaskContainer_Alloc's argument); 0x0046CC98: call 0x4F1950;
    // 0x0046CC9D: add esp,0x10 - the last link's three pushes plus this argument; 0x0046CCA0: mov [0x5BE2F8],eax. The count 3 matches the
    // three appends below.
    ::g_5be2f8 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(3));

    // Append 1: 0x0046CCA5/0x0046CCA7: push 0/7 (the append's sixth and fifth arguments); 0x0046CCA9: call 0x4F1010;
    // 0x0046CCAE: lea ecx,[esp+0xF8] -> frame+0xF0; 0x0046CCB5: push eax (the record type); 0x0046CCB6: mov eax,[0x5BE2F8] (the container);
    // 0x0046CCBB: lea edx,[esp+0xC] -> frame+0x00; 0x0046CCBF/0x0046CCC0/0x0046CCC1: the three argument pushes;
    // 0x0046CCC2: call 0x4F1A80; 0x0046CCC7: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be2f8)),
                                append_frame, append_frame + 0xF0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // Append 2: 0x0046CCCA/0x0046CCCC: push 0/7; 0x0046CCCE: call 0x4F0F70; 0x0046CCD3: lea ecx,[esp+0x78] -> frame+0x70;
    // 0x0046CCDD: lea edx,[esp+0xC] -> frame+0x00; 0x0046CCE4: call 0x4F1A80; 0x0046CCE9: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be2f8)),
                                append_frame, append_frame + 0x70,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // Append 3: 0x0046CCEC: call 0x413DE0 (eax = 0x0056E228); 0x0046CCF1: push eax (inert for 0x4F1E20, and the append's sixth argument);
    // 0x0046CCF2: push 7 (inert, and the append's fifth argument); 0x0046CCF4: call 0x4F1E20 (eax = the global 0x0054E158, the record type);
    // 0x0046CCF9: lea ecx,[esp+0x138] -> frame+0x130; 0x0046CD06: lea edx,[esp+0xC] -> frame+0x00; 0x0046CD0D: call 0x4F1A80. This append has
    // no cleanup of its own.
    const std::uint32_t gun_pickup_type_address = Sys_GetGlobalPtr_413DE0(); // 0x0046CCEC: call 0x413DE0 (0x0056E228)
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be2f8)),
                                append_frame, append_frame + 0x130,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1E20()), 7,
                                static_cast<std::int32_t>(gun_pickup_type_address));

    // 0x0046CD12: add esp,0x170 - the 0x158 frame plus the last append's six pushes; 0x0046CD18: ret.
} // end of Task_DefineGunPickup

} // extern "C"
