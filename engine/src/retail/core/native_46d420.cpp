// Native: Task_DefineAmmoPickup (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0046D420-0x0046D5ED.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 462 bytes, 1 block, 130 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 130 instructions / 462 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x0046D420-0x0046D5ED; the ret is at 0x0046D5ED, two nops follow, and
//   0x0046D5F0 is the next function, which this body uses as a handler); `ps` at 0x0053FE2C for the name literal (byte-identical to
//   "AmmoPickup"); `pxw`/`px` at 0x005BE300, 0x005BE304 and 0x0053FE28 for the image values; a stack-pointer simulation over all 130
//   instructions, which returns esp to its entry value at the ret and accounts for every cleanup (0x3C = the two inert arguments, the three
//   registration arguments, the QTask_AllocateEvent argument and three three-push links; 0xC five times; 0x10 once = the 0x4167E0 link's three
//   pushes plus the container count; 0x150 = the 0x138 frame plus the last append's six pushes) and which resolved all four `lea reg,[esp+N]`
//   pairs to frame-relative addresses. Every callee was re-read at its own VA, which is how 0x00416850, 0x00416880, 0x004167E0, 0x004F1210,
//   0x004F1220 and 0x004F1E20 were established as argument-less loads despite being called with pushes. Argument numbers were derived by
//   counting the pushes above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body with a 0x138-byte frame. It registers the "AmmoPickup" task type of size 0x138 with parent
//   Sys_GetGlobal16_4E0830() -> id at 0x005BE300, allocates one event id through QTask_AllocateEvent(0) and stores its low byte at 0x0053FE28
//   (image value 0xFF), then links ten handlers: e0->0x0046D5F0 (0x00401400), e1->0x0046D740 and e2->0x0046D760 (0x00401530),
//   0x004F1A60->0x0046D7C0 and 0x004F1A70->0x0046D7D0 (0x00401400), 0x004F1210->0x0046D8D0 and 0x004F1220->0x0046D930 (0x00401530), then
//   three more through 0x00401400 whose event ids come from the argument-less byte getters 0x00416850, 0x00416880 and 0x004167E0 and whose
//   handlers are 0x0045F920 (twice, the second event of the same handler) and 0x0046D990. It allocates a FOUR-entry parameter container through
//   0x004F1950 into 0x005BE304 and appends four 16-byte records through 0x004F1A80 with recorded deltas 0xF0, 0x70, 0x110 and 0x130. The third
//   record's type is not a constant getter: it is 0x004F1E20(0x0047CF10(), 7), the same computed-type shape the accepted 0x0046DB40 uses, and
//   the address 0x005C10F0 that 0x0047CF10 returns is that record's sixth argument. One basic block, no branch, no loop, 30 call sites over 19
//   distinct callees. There is NO 0x004B8810 script-name tail in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x005BE300 holds image value 0 and the container slot 0x005BE304 holds image value
//   0, both in the 0x005BE3xx cluster; the byte at 0x0053FE28 holds image value 0xFF and is written by the QTask_AllocateEvent result at
//   0x0046D44C. Its storage class is a second test of the image-value rule: a non-zero image value must come out in the initialised section.
//   The container is allocated with count 4 and exactly four records are appended. The three getters 0x00416850, 0x00416880 and 0x004167E0 are
//   called with one push each (0x0045F920 twice and 0x0046D990 once) and ignore it - each body is a single `mov al,[global]; ret` - so that
//   pushed value is the handler of the following 0x00401400 link, not an argument of the getter. 0x0045F920 is referenced 32 times image-wide
//   and lies below 0x00460000, so it belongs to the other lane. The sixth argument of 0x004F1A80 is transcribed here (0 at three appends and
//   0x005C10F0 at the third), with the corrected six-argument shape: the callee reads arg1..arg4, arg5 as a byte (record +0x0C) and arg6 as a
//   dword (record +0x08). Documented shape differences, all value-preserving: (1) the getters are argument-less loads in retail that ignore the
//   arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; each is
//   simultaneously the next 0x00401400/0x00401530 link call's pre-pushed handler, which the port keeps as the handler value in the same order,
//   and for the four appends they are the values that become the append's sixth and fifth arguments; (2) byte-valued event ids are zero-extended
//   in the port, which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask
//   the event word for the phase index; (3) retail's 0x138-byte frame is reproduced as a single local slot whose address stands in for the frame
//   base, the same convention the accepted 0x004813E0, 0x00470FA0 and 0x0046DB40 files use - only the two addresses passed to 0x004F1A80
//   matter and the callee stores their difference, and nothing in this body reads the intervening bytes. Handlers 0x0046D5F0, 0x0046D740,
//   0x0046D760, 0x0046D7C0, 0x0046D7D0, 0x0046D8D0, 0x0046D930 and 0x0046D990 are raw code immediates in retail and are not transcribed yet
//   (native gaps in 0x0046xxxx-0x0048xxxx); 0x0046D760 is shared with 0x0046CB70, a later registrar in this lane. Domain: core/, because it is
//   one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4E0830(void);            // 0x004E0830: `mov ax,[0x548698]; ret` (parent type)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);            // 0x004F1210: `mov eax,[0x54DDA8]; ret` (event id)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);            // 0x004F1220: `mov eax,[0x54DDAC]; ret` (event id)
extern "C" std::uint8_t Sys_GetSettingByte_416850(void);          // 0x00416850: `mov al,[0x539548]; ret` (event id; ignores its push)
extern "C" std::uint8_t Sys_GetSettingByte_416880(void);          // 0x00416880: `mov al,[0x53954D]; ret` (event id; ignores its push)
extern "C" std::uint8_t Sys_GetSettingByte_4167E0(void);          // 0x004167E0: `mov al,[0x539541]; ret` (event id; ignores its push)
extern "C" std::uint32_t Sys_GetGlobalPointer_47CF10(void);       // 0x0047CF10: `mov eax,0x5C10F0; ret` (in-lane native gap)
extern "C" std::uint32_t Sys_GetGlobal32_4F1E20(void);            // 0x004F1E20: `mov eax,[0x54E158]; ret` (ignores both pushed arguments)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);            // 0x004F0F70: `mov eax,[0xA70C6C]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0FB0(void);            // 0x004F0FB0: `mov eax,[0xA70C78]; ret`
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script_state.hpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08); the two
// reads are both `[esp+0x20]` in retail at different push depths with a `pop edi` between them.
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x005BE300: the "AmmoPickup" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be300 = 0U;

// 0x005BE304: the four-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_5be304 = 0U;

// 0x0053FE28: the low byte of the event id allocated by QTask_AllocateEvent(0). Image value 0xFF, written at 0x0046D44C by a byte store, and
// the "AmmoPickup" name literal starts four bytes later at 0x0053FE2C.
std::uint8_t g_53fe28 = 0xFFU;

void Task_DefineAmmoPickup()
{
    // 0x0046D420: sub esp,0x138 - retail's 312-byte frame, which is the registered object size. The four 0x004F1A80 calls below pass frame+0x00
    // and frame+0xF0 / +0x70 / +0x110 / +0x130; the callee stores the differences. A single local slot stands in for the frame base, which is
    // the convention the accepted 0x004813E0, 0x00470FA0 and 0x0046DB40 files use.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0046D426/0x0046D428: push 0; push 0 (both inert arguments of the parent getter); 0x0046D42A: call 0x4E0830; 0x0046D42F: push eax;
    // 0x0046D430: push 0x138; 0x0046D435: push "AmmoPickup"; 0x0046D43A: call 0x401900.
    const std::uint16_t ammo_pickup_id = TaskType_Register("AmmoPickup", // 0x0046D435: push 0x0053FE2C
                                                           0x138U,       // 0x0046D430: push 0x138 (object size, equal to the frame)
                                                           Sys_GetGlobal16_4E0830(), // 0x0046D42A: call 0x4E0830 (parent type)
                                                           0U);          // 0x0046D428: push 0 (inert)

    // 0x0046D43F: push 0 (QTask_AllocateEvent's argument); 0x0046D441: mov word [0x5BE300],ax (the id store lands between the pushes);
    // 0x0046D447: call 0x4017C0; 0x0046D44C: mov byte [0x53FE28],al - only the low byte of the returned id is kept.
    ::g_5be300 = ammo_pickup_id;
    ::g_53fe28 = static_cast<std::uint8_t>(QTask_AllocateEvent(0));

    // 0x0046D451: mov ax,[0x5BE300]; 0x0046D457: push 0x0046D5F0 (pre-pushed handler); 0x0046D45C: push 0; 0x0046D45E: push eax;
    // 0x0046D45F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be300), // 0x0046D451: mov ax,[0x5BE300]
                                0U,                                     // 0x0046D45C: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046D5F0U))); // 0x0046D457: push handler

    // 0x0046D464: mov cx,[0x5BE300]; 0x0046D46B: push 0x0046D740; 0x0046D470: push 1; 0x0046D472: push ecx; 0x0046D473: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be300), // 0x0046D464: mov cx,[0x5BE300]
                                           1U,                                     // 0x0046D470: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046D740U))); // 0x0046D46B: push handler

    // 0x0046D478: mov dx,[0x5BE300]; 0x0046D47F: push 0x0046D760; 0x0046D484: push 2; 0x0046D486: push edx; 0x0046D487: call 0x401530;
    // 0x0046D48C: add esp,0x3C - the two inert arguments, the three registration arguments, the QTask_AllocateEvent argument and three
    // three-push links.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be300), // 0x0046D478: mov dx,[0x5BE300]
                                           2U,                                     // 0x0046D484: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046D760U))); // 0x0046D47F: push handler

    // 0x0046D48F: push 0x0046D7C0; 0x0046D494: call 0x4F1A60; 0x0046D499: push eax; 0x0046D49A: mov ax,[0x5BE300]; 0x0046D4A0: push eax;
    // 0x0046D4A1: call 0x401400; 0x0046D4A6: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be300), // 0x0046D49A: mov ax,[0x5BE300]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x0046D499: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046D7C0U))); // 0x0046D48F: push handler

    // 0x0046D4A9: push 0x0046D7D0; 0x0046D4AE: call 0x4F1A70; 0x0046D4B3: mov cx,[0x5BE300]; 0x0046D4BA: push eax; 0x0046D4BB: push ecx;
    // 0x0046D4BC: call 0x401400; 0x0046D4C1: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be300), // 0x0046D4B3: mov cx,[0x5BE300]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x0046D4BA: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046D7D0U))); // 0x0046D4A9: push handler

    // 0x0046D4C4: push 0x0046D8D0; 0x0046D4C9: call 0x4F1210; 0x0046D4CE: mov dx,[0x5BE300]; 0x0046D4D5: push eax; 0x0046D4D6: push edx;
    // 0x0046D4D7: call 0x401530; 0x0046D4DC: add esp,0xC.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be300), // 0x0046D4CE: mov dx,[0x5BE300]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // 0x0046D4D5: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046D8D0U))); // 0x0046D4C4: push handler

    // 0x0046D4DF: push 0x0046D930; 0x0046D4E4: call 0x4F1220; 0x0046D4E9: push eax; 0x0046D4EA: mov ax,[0x5BE300]; 0x0046D4F0: push eax;
    // 0x0046D4F1: call 0x401530; 0x0046D4F6: add esp,0xC.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be300), // 0x0046D4EA: mov ax,[0x5BE300]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // 0x0046D4E9: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046D930U))); // 0x0046D4DF: push handler

    // 0x0046D4F9: push 0x0045F920 (the handler, and the inert argument of the byte getter); 0x0046D4FE: call 0x416850;
    // 0x0046D503: mov cx,[0x5BE300]; 0x0046D50A: push eax; 0x0046D50B: push ecx; 0x0046D50C: call 0x401400; 0x0046D511: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be300), // 0x0046D503: mov cx,[0x5BE300]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416850()), // 0x0046D50A: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x0046D4F9: push handler (below 0x00460000)

    // 0x0046D514: push 0x0045F920 (the same handler again, for its second event); 0x0046D519: call 0x416880; 0x0046D51E: mov dx,[0x5BE300];
    // 0x0046D525: push eax; 0x0046D526: push edx; 0x0046D527: call 0x401400; 0x0046D52C: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be300), // 0x0046D51E: mov dx,[0x5BE300]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416880()), // 0x0046D525: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x0046D514: push handler (below 0x00460000)

    // 0x0046D52F: push 0x0046D990; 0x0046D534: call 0x4167E0; 0x0046D539: push eax; 0x0046D53A: mov ax,[0x5BE300]; 0x0046D540: push eax;
    // 0x0046D541: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be300), // 0x0046D53A: mov ax,[0x5BE300]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4167E0()), // 0x0046D539: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046D990U))); // 0x0046D52F: push handler

    // 0x0046D546: push 4 (Script_TaskContainer_Alloc's argument); 0x0046D548: call 0x4F1950;
    // 0x0046D54D: add esp,0x10 - the last link's three pushes plus this argument; 0x0046D550: mov [0x5BE304],eax. The count 4 matches the four
    // appends below.
    ::g_5be304 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(4));

    // Append 1: 0x0046D555/0x0046D557: push 0/7 (the append's sixth and fifth arguments); 0x0046D559: call 0x4F1010;
    // 0x0046D55E: lea ecx,[esp+0xF8] -> frame+0xF0; 0x0046D565: push eax (the record type); 0x0046D566: mov eax,[0x5BE304] (the container);
    // 0x0046D56B: lea edx,[esp+0xC] -> frame+0x00; 0x0046D56F/0x0046D570/0x0046D571: the three argument pushes;
    // 0x0046D572: call 0x4F1A80; 0x0046D577: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be304)),
                                append_frame, append_frame + 0xF0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // Append 2: 0x0046D57A/0x0046D57C: push 0/7; 0x0046D57E: call 0x4F0F70; 0x0046D583: lea ecx,[esp+0x78] -> frame+0x70;
    // 0x0046D58D: lea edx,[esp+0xC] -> frame+0x00; 0x0046D594: call 0x4F1A80; 0x0046D599: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be304)),
                                append_frame, append_frame + 0x70,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // Append 3: 0x0046D59C: call 0x47CF10 (eax = 0x005C10F0); 0x0046D5A1: push eax (inert for 0x4F1E20, and the append's sixth argument);
    // 0x0046D5A2: push 7 (inert, and the append's fifth argument); 0x0046D5A4: call 0x4F1E20 (eax = the global 0x0054E158, the record type);
    // 0x0046D5A9: lea ecx,[esp+0x118] -> frame+0x110; 0x0046D5B6: lea edx,[esp+0xC] -> frame+0x00; 0x0046D5BD: call 0x4F1A80;
    // 0x0046D5C2: add esp,0x18.
    const std::uint32_t ammo_pickup_type_address = Sys_GetGlobalPointer_47CF10(); // 0x0046D59C: call 0x47CF10
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be304)),
                                append_frame, append_frame + 0x110,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1E20()), 7,
                                static_cast<std::int32_t>(ammo_pickup_type_address));

    // Append 4: 0x0046D5C5/0x0046D5C7: push 0/7; 0x0046D5C9: call 0x4F0FB0; 0x0046D5CE: lea ecx,[esp+0x138] -> frame+0x130;
    // 0x0046D5DB: lea edx,[esp+0xC] -> frame+0x00; 0x0046D5E2: call 0x4F1A80. This append has no cleanup of its own.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be304)),
                                append_frame, append_frame + 0x130,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FB0()), 7, 0);

    // 0x0046D5E7: add esp,0x150 - the 0x138 frame plus the last append's six pushes; 0x0046D5ED: ret.
} // end of Task_DefineAmmoPickup

} // extern "C"
