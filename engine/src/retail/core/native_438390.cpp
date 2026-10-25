// Native: Task_DefineGunShotEffect (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x00438390-0x00438432.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 163 bytes, 1 block, 42 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 42 instructions / 163 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent (0x00438390-0x00438432; the ret is at 0x00438432 and 0x00438440 is the next function,
//   which this body uses as its first handler); `ps` at 0x0053C1CC for the name literal (byte-identical to "GunShotEffect"), 0x0053C1B4 for the script
//   name ("TASKTYPE_GUNSHOTEFFECT"), 0x0053C1A4 for "concrete1.spr" and 0x0053C194 for "concrete2.spr"; `px 4` at 0x0057BE78/0x0057BE7C/0x0057BE80
//   (all image value 0). The three event-id getters 0x00408320, 0x00408330 and 0x00408340 were re-read at their own VAs: each is
//   `mov al,[0x53851x]; ret` with image value 255, so all three return a BYTE, not a dword. 0x004018F0 is the tree's existing
//   QTask_GetCurrentType() (core/task_types.cpp, declared in core/task_types.hpp, which this file includes) and was swept before use rather than
//   named locally. A push-by-push simulation of the whole body: this body has NO `sub esp` and allocates NO local frame, the registration's
//   `add esp,0x14` is five pushes, each of the first two links' `add esp,0xC` is its three pushes, and the final `add esp,0x1C` covers the third
//   link's three pushes plus the 0x004B8810 registration's two plus the two 0x004169D0 lookups' one push each (3 + 2 + 2 = 7 pushes = 0x1C).
// Behavior/CFG: one straight-line body with no local frame and no stack probe. It registers the "GunShotEffect" task type of size 0x20 with parent
//   QTask_GetCurrentType() (0x004018F0: `mov ax,[0x54F928]; ret`, image 0) -> id at 0x0057BE78, stores the id before linking anything, then links
//   three handlers through 0x00401400: e0->0x00438440 with the event id from Sys_GetGlobal8_408340(), e1->0x004387B0 from Sys_GetGlobal8_408330()
//   and e2->0x0046FB50 from Sys_GetGlobal8_408320(). It then registers "TASKTYPE_GUNSHOTEFFECT" against the masked id through 0x004B8810 and makes
//   two Sys_LookupOrAppendRecord_4169D0 calls whose returned pointers land in 0x57BE7C ("concrete1.spr") and 0x57BE80 ("concrete2.spr"). One basic
//   block, no branch, no loop, 11 call sites over 8 distinct callees. There is NO parameter container and NO 0x004F1A80 append in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. All three globals hold image value 0, so they are zero-initialised data (`B`); the id slot
//   0x0057BE78 is 16-bit and the two record slots are dwords. **The id store happens BEFORE the links here** (0x004383A9), unlike the
//   0x004351E0/0x00435870/0x00435D90 family where the store is interleaved between the first link's pushes - so the store's position is per-body
//   and was taken from the listing, not from the family. **The three links use three different byte getters that differ only by one byte of
//   address** (0x00538518, 0x00538519, 0x0053851A) and they are consumed in the reverse order of their addresses: e0 uses 0x00408340 (the highest
//   address), e1 uses 0x00408330 and e2 uses 0x00408320. **The three getters take an argument in retail** - each link pushes its handler immediate
//   and then calls the getter, which never reads it - so those pushes are the deferred handler arguments and not getter arguments; the getters are
//   declared argument-less here, matching the tree's transcriptions of every other getter of this shape. The two lookups are pipelined exactly as
//   in 0x00437510 (the next name is pushed before the previous result is stored), so the store order is 0x57BE7C then 0x57BE80 while the call order
//   is concrete1.spr then concrete2.spr; the transcription keeps both orders observable by writing them as two sequential store-from-call
//   statements. Deferred pushes: at all three link sites the handler immediate is pushed before the getter call and consumed as the link's third
//   argument. Handler immediates 0x00438440, 0x004387B0 and 0x0046FB50 are raw code immediates with no implementation in the tree yet; 0x0046FB50
//   is one of the shared handler addresses observed elsewhere in the image. Documented shape differences, all value-preserving: (1) the getters
//   are argument-less loads in retail that ignore the arguments the caller pushes, so those inert pushes are not reproduced; (2) the cleanups are
//   reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x14, 0xC (twice) and
//   0x1C are recorded so a future reviewer can check the arithmetic; (3) byte-valued event ids are zero-extended in the port, which cannot change
//   behaviour because every link fed by a byte getter goes through 0x00401400, which masks the event word for the phase index.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint8_t Sys_GetGlobal8_408320(void);              // 0x00408320: `mov al,[0x538518]; ret` (image 255)
extern "C" std::uint8_t Sys_GetGlobal8_408330(void);              // 0x00408330: `mov al,[0x538519]; ret` (image 255)
extern "C" std::uint8_t Sys_GetGlobal8_408340(void);              // 0x00408340: `mov al,[0x53851A]; ret` (image 255)
extern "C" void* Sys_LookupOrAppendRecord_4169D0(char const* name);   // 0x004169D0 (config/native_4169d0.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0057BE78: the "GunShotEffect" task type id. Image value 0, so it is zero-initialised data (`B`); all accesses are 16-bit.
std::uint16_t g_57be78 = 0U;

// 0x0057BE7C and 0x0057BE80: the pointers the two 0x004169D0 lookups return. Image values are 0, so both are zero-initialised data (`B`).
std::uint32_t g_57be7c = 0U;
std::uint32_t g_57be80 = 0U;

void Task_DefineGunShotEffect()
{
    // 0x00438390: push 0 (TaskType_Register's fourth argument, pushed first); 0x00438392: push 0 (the parent getter's inert argument);
    // 0x00438394: call 0x4018F0 (QTask_GetCurrentType); 0x00438399: push eax; 0x0043839A: push 0x20; 0x0043839C: push "GunShotEffect";
    // 0x004383A1: call 0x401900; 0x004383A6: add esp,0x14 - five pushes.
    const std::uint16_t gun_shot_effect_id = TaskType_Register("GunShotEffect",   // 0x0043839C: push 0x0053C1CC
                                                               0x20U,             // 0x0043839A: push 0x20 (object size)
                                                               QTask_GetCurrentType(), // 0x00438394: call 0x4018F0 (parent type)
                                                               0U);               // 0x00438392: push 0 (fourth argument)
    ::g_57be78 = gun_shot_effect_id; // 0x004383A9: mov word [0x57BE78],ax - stored before any link in this body

    // 0x004383AF: push 0x00438440 (handler, deferred argument 3); 0x004383B4: call 0x408340 (event id; the pushed handler is not read);
    // 0x004383B9: push eax (argument 2); 0x004383BA: mov ax,[0x57BE78]; 0x004383C0: push eax (argument 1); 0x004383C1: call 0x401400;
    // 0x004383C6: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be78),
                                static_cast<std::uint32_t>(Sys_GetGlobal8_408340()), // 0x004383B9: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00438440U)));

    // 0x004383C9: push 0x004387B0 (handler, deferred argument 3); 0x004383CE: call 0x408330; 0x004383D3: mov cx,[0x57BE78];
    // 0x004383DA: push eax (argument 2); 0x004383DB: push ecx (argument 1); 0x004383DC: call 0x401400; 0x004383E1: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be78),
                                static_cast<std::uint32_t>(Sys_GetGlobal8_408330()), // 0x004383DA: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004387B0U)));

    // 0x004383E4: push 0x0046FB50 (handler, deferred argument 3); 0x004383E9: call 0x408320; 0x004383EE: mov dx,[0x57BE78];
    // 0x004383F5: push eax (argument 2); 0x004383F6: push edx (argument 1); 0x004383F7: call 0x401400. This link has no cleanup of its own:
    // the tail's add esp,0x1C covers it together with the registration and the two lookups.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be78),
                                static_cast<std::uint32_t>(Sys_GetGlobal8_408320()), // 0x004383F5: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0046FB50U)));

    // 0x004383FC: mov eax,[0x57BE78]; 0x00438401: and eax,0xFFFF; 0x00438406: push eax (argument 2); 0x00438407: push 0x0053C1B4
    // (argument 1); 0x0043840C: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GUNSHOTEFFECT"), // 0x00438407: push 0x0053C1B4
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_57be78) & 0xFFFFU))); // 0x00438406: push eax

    // 0x00438411-0x0043842D: two record lookups. Retail pushes the second name before storing the first result, so the stores are 0x57BE7C then
    // 0x57BE80 while the lookups are concrete1.spr then concrete2.spr.
    ::g_57be7c = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        Sys_LookupOrAppendRecord_4169D0("concrete1.spr"))); // 0x00438411: push 0x0053C1A4; 0x00438416: call 0x4169D0
    ::g_57be80 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        Sys_LookupOrAppendRecord_4169D0("concrete2.spr"))); // 0x0043841B: push 0x0053C194; 0x00438425: call 0x4169D0

    // 0x0043842A: add esp,0x1C - the third link's three pushes plus this registration's two plus the two lookups' one push each.
} // end of Task_DefineGunShotEffect

} // extern "C"
