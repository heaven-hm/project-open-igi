// Native: Task_DefinePlaceExplosiveTBA (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x004388B0-0x00438999.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 234 bytes, 1 block, 66 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 66 instructions / 234 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent (0x004388B0-0x00438999; the ret is at 0x00438999 and 0x004389A0 is the next function,
//   which this body uses as its first handler); `ps` at 0x0053C1E0 for the name literal (byte-identical to "PlaceExplosiveTBA", terminated by the
//   dword at 0x0053C1F0 which is the next literal "RotatingObje..."); `px 4` at 0x0053C1DC (image value 0x00000181) and at 0x0057BE8C (image value
//   0). A push-by-push simulation of the whole body: `sub esp,0x2B0`, every push and every `add esp` in order, closing exactly with the final
//   `add esp,0x2C8` = 0x2B0 frame + the last append's six pushes (0x18); every `lea reg,[esp+N]` was resolved to frame+0x00 for the second argument
//   and to the recorded delta for the third. The two append deltas were generated mechanically with assertions and then re-checked against retail.
// Behavior/CFG: one straight-line body. It allocates a 0x2B0-byte frame with `sub esp,0x2B0` (no stack probe) and registers the
//   "PlaceExplosiveTBA" task type of size 0x2B0 with parent Sys_GetGlobal16_448550() -> id at 0x0053C1DC, which it stores before linking. It links
//   four handlers through 0x00401400: e0->0x004389A0 with the literal event id 0, e1->0x00438C00 with the event id from Script_PhaseIdGetE4_4F1A60(),
//   e2->0x004389E0 with the event id from Sys_GetSettingByte_4167F0() and e3->0x00438BF0 with the event id from Sys_GetSettingByte_416810(). It
//   then grows a parameter container by two entries - `Script_TaskContainer_Grow(Sys_GetGlobal32_448560(), 2)`, the container coming from the
//   getter and the count being the pushed 2 - stores the returned pointer at 0x0057BE8C and appends two records through 0x004F1A80 with recorded
//   deltas 0x2A8 and 0x2AC, using Sys_GetGlobal32_4F0F50 for the first and Core_GetRenderWordA_4F0FA0 for the second. One basic block, no branch,
//   no loop, 15 call sites over 11 distinct callees. There is NO 0x004B8810 script-name registration in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The id slot is 0x0053C1DC, which is inside the .data section (VA 0x00536000 + 0x6FC558) and
//   NOT in .rdata, even though its address is far above the 0x0053xxxx .rdata start**: parsing the PE section table gives .rdata as
//   VA 0x00533000 with virtual size 0x28F6, ending at 0x005358F6, so 0x0053C1DC is writable data - the address alone would have misclassified it.
//   Its image value is 0x00000181, which is non-zero, so it is INITIALISED data (`D`) with initialiser 0x0181U and not zero-initialised data; the
//   16-bit store at 0x004388D7 overwrites its low word at startup. The container slot 0x0057BE8C has image value 0 and is zero-initialised data
//   (`B`). The container is not allocated here: it is obtained from Sys_GetGlobal32_448560() and grown by 2, and the count 2 matches the two
//   appends below. Arg6 of 0x004F1A80 is 0 at both sites here (checked per site). **The first link's event id is the literal 0** (0x004388D4:
//   `push 0`), not a getter result, so this body mixes a constant event id with three getter-derived ones. The four handler immediates
//   0x004389A0, 0x00438C00, 0x004389E0 and 0x00438BF0 are raw code immediates with no implementation in the tree yet. Deferred pushes: at every
//   link site the handler immediate is pushed before the event-id getter call and consumed as the link's third argument, and at the two getter-fed
//   sites the getter never reads the pushed handler. Documented shape differences, all value-preserving: (1) every getter callee is an
//   argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; (2) the cleanups are reproduced implicitly by the tree's calling convention rather than by explicit `add esp`
//   arithmetic; the retail sizes 0x20, 0xC (three times), 0x8, 0x18 and 0x2C8 are recorded so a future reviewer can check the arithmetic;
//   (3) byte-valued event ids are zero-extended in the port, which cannot change behaviour because every link fed by a byte getter goes through
//   0x00401400, which masks the event word for the phase index.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_448550(void);            // 0x00448550: `mov ax,[0x53C7DC]; ret` (parent type)
extern "C" std::uint32_t Sys_GetGlobal32_448560(void);            // 0x00448560: `mov eax,[0x57C1FC]; ret` (the existing parameter container)
extern "C" std::uint8_t Sys_GetSettingByte_4167F0(void);          // 0x004167F0: argument-less byte getter (config/native_4167f0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416810(void);          // 0x00416810: argument-less byte getter (config/native_416810.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);            // 0x004F0F50: `mov eax,[0xA71870]; ret`
extern "C" std::uint32_t Core_GetRenderWordA_4F0FA0(void);        // 0x004F0FA0: `mov eax,[0xA70C7C]; ret` (core/native_4f0fa0.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
// 0x004F1980 (script/native_4f1980.cpp): takes the container and a count, returns the container pointer.
extern "C" void* Script_TaskContainer_Grow(std::int32_t* container, std::int32_t count);
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0053C1DC: the "PlaceExplosiveTBA" task type id. In .data (see the PE section table note above) with image value 0x0181, so it is
// INITIALISED data (`D`); only the low 16 bits are ever written and read.
std::uint16_t g_53c1dc = 0x0181U;

// 0x0057BE8C: the parameter container pointer Script_TaskContainer_Grow returns. Image value 0, so it is zero-initialised data (`B`).
std::uint32_t g_57be8c = 0U;

void Task_DefinePlaceExplosiveTBA()
{
    // 0x004388B0: sub esp,0x2B0 - a plain frame; this body has NO stack probe.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x004388B6: push 0 (TaskType_Register's fourth argument, pushed first); 0x004388B8: push 0 (the parent getter's inert argument);
    // 0x004388BA: call 0x448550; 0x004388BF: push eax; 0x004388C0: push 0x2B0; 0x004388C5: push "PlaceExplosiveTBA"; 0x004388CA: call 0x401900;
    // 0x004388CF: add esp,0x20 - eight pushes.
    const std::uint16_t place_explosive_id = TaskType_Register("PlaceExplosiveTBA", // 0x004388C5: push 0x0053C1E0
                                                               0x2B0U,                // 0x004388C0: push 0x2B0 (object size, equal to the frame)
                                                               Sys_GetGlobal16_448550(), // 0x004388BA: call 0x448550 (parent type)
                                                               0U);                  // 0x004388B8: push 0 (fourth argument)
    ::g_53c1dc = place_explosive_id; // 0x004388D7: mov word [0x53C1DC],ax - stored before any link in this body

    // 0x004388CF: push 0x004389A0 (handler, deferred argument 3); 0x004388D4: push 0x0 (argument 2, the literal event id);
    // 0x004388D6: push eax (argument 1); 0x004388DD: call 0x401400. The handler push and the event-id push are the link's two deferred
    // arguments; no getter is called for this link.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c1dc), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004389A0U)));

    // 0x004388E5: push 0x00438C00 (handler, deferred argument 3); 0x004388EA: call 0x4F1A60 (event id; the pushed handler is not read);
    // 0x004388EF: push eax (argument 2); 0x004388F0: mov ax,[0x53C1DC]; 0x004388F6: push eax (argument 1); 0x004388F7: call 0x401400;
    // 0x004388FC: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c1dc),
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x004388EF: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00438C00U)));

    // 0x004388FF: push 0x004389E0 (handler, deferred argument 3); 0x00438904: call 0x4167F0 (event id; the pushed handler is not read);
    // 0x00438909: mov cx,[0x53C1DC]; 0x00438910: push eax (argument 2); 0x00438911: push ecx (argument 1); 0x00438912: call 0x401400;
    // 0x00438917: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c1dc),
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4167F0()), // 0x00438910: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004389E0U)));

    // 0x0043891A: push 0x00438BF0 (handler, deferred argument 3); 0x0043891F: call 0x416810; 0x00438924: mov dx,[0x53C1DC];
    // 0x0043892B: push eax (argument 2); 0x0043892C: push edx (argument 1); 0x0043892D: call 0x401400; 0x00438932: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c1dc),
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416810()), // 0x0043892B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00438BF0U)));

    // 0x00438935: push 0x2 (argument 2 = the number of entries to grow by, equal to the two appends below); 0x00438937: call 0x448560
    // (argument 1 = the existing container); 0x0043893C: push eax (argument 1); 0x0043893D: call 0x4F1980; 0x00438942: add esp,0x8;
    // 0x00438945: mov [0x57BE8C],eax.
    ::g_57be8c = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        Script_TaskContainer_Grow(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(Sys_GetGlobal32_448560())), 2))); // 0x00438935: push 0x2

    // 0x0043894A: push 0x0 (arg6); 0x0043894C: push 0x7 (arg5); 0x0043894E: call 0x4F0F50 (arg4); 0x00438953: mov edx,[0x57BE8C] (arg1);
    // 0x00438959: push eax (arg4); 0x0043895A: lea eax,[esp+0x2B4] -> frame+0x2A8 (arg3); 0x00438961: lea ecx,[esp+0xC] -> frame+0x00 (arg2);
    // 0x00438968: call 0x4F1A80; 0x0043896D: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be8c)),
                                append_frame, append_frame + 0x2A8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);

    // 0x00438970: push 0x0 (arg6); 0x00438972: push 0x7 (arg5); 0x00438974: call 0x4F0FA0 (arg4); 0x00438979: mov edx,[0x57BE8C] (arg1);
    // 0x0043897F: push eax (arg4); 0x00438980: lea eax,[esp+0x2B8] -> frame+0x2AC (arg3); 0x00438987: lea ecx,[esp+0xC] -> frame+0x00 (arg2);
    // 0x0043898E: call 0x4F1A80; 0x00438993: add esp,0x2C8 - the 0x2B0 frame plus this append's six pushes.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be8c)),
                                append_frame, append_frame + 0x2AC,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
} // end of Task_DefinePlaceExplosiveTBA

} // extern "C"
