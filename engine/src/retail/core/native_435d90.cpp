// Native: Task_DefineHatch (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x00435D90-0x00435E4D.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 190 bytes, 1 block, 50 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 50 instructions / 190 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00435D90-0x00435E4D; the ret is at 0x00435E4D and 0x0x00435E50 is the next function, which this
//   body uses as its second handler); `ps` at 0xHatchVA for the name literal (byte-identical to "Hatch") and at 0xTASKTYPE_HATCHVA for the script name
//   ("TASKTYPE_HATCH"); `px 2 @ 0x0057BD6C` for the id slot; a push-by-push simulation of the whole body, which starts at the entry esp (this body has NO
//   `sub esp` and allocates NO local frame - the 0x140 it registers is never reserved on the stack) and ends with the final `add esp,0x14`, which
//   is exactly the last link's three pushes (0xC) plus the script tail's two pushes (0x8). Argument numbers were derived by counting the pushes
//   above each access. Every callee was re-read at its own VA. This body is instruction-for-instruction the shape of 0x004351E0 (CockpitShield)
//   apart from its name, its id slot and its handler immediates, which were compared site by site rather than assumed.
// Behavior/CFG: one straight-line body with no local frame. It registers the "Hatch" task type of size 0x140 with parent Sys_GetGlobal16_4D9FA0()
//   (0x004D9FA0: `mov ax,[0x548648]; ret`, image value 385 = 0x0181) -> id at 0x0057BD6C, links e0->0x00435C50 through 0x00401400, e1->0x00435E50 and
//   e2->0x0051B940 through 0x00401530, then links three more handlers through 0x00401400: 0x004D9FB0->0x00435F60, 0x004C56D0->0x00436090 and
//   0x004C56E0->0x00427D70. It then ends with the script-name tail Script_QHashNodeRegister("TASKTYPE_HATCH", id & 0xFFFF) at 0x004B8810. One basic block,
//   no branch, no loop, 12 call sites over 8 distinct callees. There is NO parameter container and NO 0x004F1A80 append in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x0057BD6C holds image value 0, so it is zero-initialised data (`B`) and its accesses
//   are 16-bit; it is the same 0x0057BDxx block the sibling registrars 0x004330F0, 0x004351E0 and 0x004351E0 use, each one task type further along.
//   **Deferred pushes**: at all six link sites the handler immediate is pushed before the event-id getter call and consumed as the link's third
//   argument, with the getter sitting in between and never reading it. Two of the three getters are not byte getters of the 0x004F4xxx family but
//   0x004D9FB0 (`mov al,[0x54864A]; ret`, image value 255) and the dynobj accessors 0x004C56D0/0x004C56E0, which is why the callee set of this
//   body is unusual for a registrar; they are transcribed as the argument-less calls retail makes. **0x00427D70 is already implemented in this tree as Script_SetConstant_427D70 (script/native_427d70.cpp)**, below 0x00430000 and therefore in another lane's scope; it is still passed here as the raw immediate retail pushes, so this file does not depend on that symbol's signature. The remaining handler immediates
//   0x00435C50, 0x00435E50, 0x00435F60, 0x00436090 are raw code immediates with no implementation in the tree yet. Documented shape differences, all value-preserving: (1) every
//   getter callee is an argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no
//   arguments, so those inert pushes are not reproduced; (2) byte-valued event ids are zero-extended in the port, which cannot change behaviour
//   because every link fed by a byte getter goes through 0x00401400 or 0x00401530, which mask the event word for the phase index; (3) the
//   cleanups are reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38 and
//   0x14 are recorded so a future reviewer can check the arithmetic. 0x0051B940 is registered as a handler by 0x004351E0, by 0x00435870 and by
//   0x00435D90, so one handler address serves three task types.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);            // 0x004D9FA0: `mov ax,[0x548648]; ret` (parent type; image 385 = 0x0181)
extern "C" std::uint8_t Sys_GetGlobal8_4D9FB0(void);              // 0x004D9FB0: `mov al,[0x54864A]; ret` (image 255)
extern "C" std::uint8_t DynObj_GetByte_4C56D0(void);              // 0x004C56D0: argument-less byte accessor (dynobj/native_4c56d0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56E0(void);              // 0x004C56E0: argument-less byte accessor (dynobj/native_4c56e0.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0057BD6C: the "Hatch" task type id. Image value 0, so it is zero-initialised data (`B`); all accesses are 16-bit.
std::uint16_t g_57bd6c = 0U;

void Task_DefineHatch()
{
    // 0x00435D90: push 0; 0x00435D902: push 0 (the parent getter's inert argument, then TaskType_Register's fourth argument);
    // 0x00435D903: call 0x4D9FA0; 0x00435D904: push eax; 0x00435D905: push 0x140; 0x00435D906: push "Hatch"; 0x00435D907: call 0x401900.
    const std::uint16_t hatch_id = TaskType_Register("Hatch",                  // 0x00435D906: push 0xHatchVA
                                                    0x140U,                   // 0x00435D905: push 0x140 (object size)
                                                    Sys_GetGlobal16_4D9FA0(), // 0x00435D903: call 0x4D9FA0 (parent type)
                                                    0U);                      // 0x00435D902: push 0 (fourth argument)
    ::g_57bd6c = hatch_id; // 0x00435DB1: mov word [0x0057BD6C],ax - the id store lands between the pushes of the first link

    // 0x00435DA9: push 0x00435C50 (handler, deferred argument 3); 0x00435DA9B: push 0x0 (argument 2); 0x00435DA9C: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd6c), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00435C50U)));

    // 0x00435DBC: push 0x00435E50 (handler, deferred argument 3); 0x00435DBCB: push 0x1 (argument 2); 0x00435DBCC: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bd6c), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00435E50U)));

    // 0x00435DCF: push 0x0051B940 (handler, deferred argument 3); 0x00435DCFB: push 0x2 (argument 2); 0x00435DCFC: call 0x401530;
    // 0x00435DCFD: add esp,0x38 - fourteen pushes: the two zeros, the parent-getter result, the size, the name and the three pushes of each of the
    // first three links.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bd6c), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0051B940U)));

    // 0x00435DE6: push 0x00435F60 (handler, deferred argument 3); 0x00435DE6B: call 0x4D9FB0 (event id; the pushed handler is not read);
    // 0x00435DE6C: mov dx,[0x0057BD6C]; 0x00435DE6D: push eax (argument 2); 0x00435DE6E: push edx (argument 1); 0x00435DE6F: call 0x401400; 0x00435DE6G: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd6c),
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4D9FB0()), // 0x00435DE6D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00435F60U)));

    // 0x00435E01: push 0x00436090 (handler, deferred argument 3); 0x00435E01B: call 0x4C56D0; 0x00435E01C: push eax (argument 2);
    // 0x00435E01D: mov ax,[0x0057BD6C]; 0x00435E01E: push eax (argument 1); 0x00435E01F: call 0x401400; 0x00435E01G: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd6c),
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56D0()), // 0x00435E01C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00436090U)));

    // 0x00435E1B: push 0x00427D70 (handler, deferred argument 3); 0x00435E1BB: call 0x4C56E0; 0x00435E1BC: mov cx,[0x0057BD6C]; 0x00435E1BD: push eax (argument 2);
    // 0x00435E1BE: push ecx (argument 1); 0x00435E1BF: call 0x401400. This link has no cleanup of its own: the tail's add esp,0x14 covers it together
    // with the script-name pushes.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd6c),
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56E0()), // 0x00435E1BD: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00427D70U)));

    // 0x00435E33: mov edx,[0x0057BD6C]; 0x00435E39: and edx,0xFFFF; 0x00435E3F: push edx (argument 2); 0x00435E40: push 0xTASKTYPE_HATCHVA
    // (argument 1); 0x00435E45: call 0x4B8810; 0x00435E4A: add esp,0x14 - the last link's three pushes (0xC) plus these two (0x8).
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_HATCH"), // 0x00435E40: push 0xTASKTYPE_HATCHVA
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_57bd6c) & 0xFFFFU))); // 0x00435E3F: push edx
} // end of Task_DefineHatch

} // extern "C"
