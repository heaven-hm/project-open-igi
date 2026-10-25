// Native: Task_DefineExplodeMagicObj (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0043BFD0-0x0043C0A6.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 215 bytes, 1 block, 57 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 57 instructions / 215 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent, instruction by instruction (the ret is at 0x0043C0A6 and 0x0043BE80 is the preceding
//   function, which this body uses as its first handler); `ps` at 0x0053C354 for the name literal (byte-identical to "ExplodeMagicObj") and at
//   0x0053C338 for the hash-registration key (byte-identical to "TASKTYPE_EXPLODEMAGICOBJ"); `px 4` at 0x0057BF7C (image value 0). **This body has
//   NO prologue at all - it contains no `sub esp` and no saved-register push, so it has no local frame and therefore no frame-relative `lea` to
//   resolve.** The frame-relative shift question that governs 0x004397C0 (8), 0x0043AD80 (12) and 0x0043B2F0 (0) does not arise here, because
//   there is no frame to be relative to; that is a third distinct prologue shape in the same family. A full push-by-push simulation closes exactly:
//   the registration's `add esp,0x38` is fourteen pushes, the fourth, fifth and sixth links' `add esp,0xC` is three each, and the FINAL
//   `add esp,0x14` is the seventh link's three pushes plus the two pushes of the hash registration - the seventh link's cleanup is deliberately
//   folded into the final one, which is why no `add esp,0xC` appears between the last `call 0x401400` and the `call 0x4B8810`. Every callee was
//   swept against the tree's `// Native:` provenance index with no lane exemption and every one resolves to an existing file under the same name,
//   return type and linkage.
// Behavior/CFG: one straight-line body with no local frame. It registers the "ExplodeMagicObj" task type of size 0x150 with parent
//   Sys_GetGlobal16_4D9FA0() -> id at 0x0057BF7C, stores the id, then links SEVEN handlers: e0->0x0043BE80, e1->0x0043C0B0 and e2->0x00449950
//   with the literal event ids 0, 1 and 2, then 0x004C56D0->0x0043C1F0, 0x004C56E0->0x0043C2A0, 0x00416840->0x0043C2B0 and
//   0x00416860->0x0043C2D0. It then registers the task type in the script hash: `Script_QHashNodeRegister("TASKTYPE_EXPLODEMAGICOBJ",
//   (uint32)id & 0xFFFF)` - the id is reloaded as a DWORD and masked with 0xFFFF before being pushed, which is why the port reproduces both the
//   dword reload and the mask rather than a plain 16-bit read. One basic block, no branch, no loop, 14 call sites over 9 distinct callees.
//   There is NO 0x004F1A80 container append, NO 0x004F1950 allocation and NO 0x004F1980 growth in this body: it is a pure registrar.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The id slot 0x0057BF7C is in writable `.data`** (the section table puts `.rdata` at VA
//   0x00533000 with virtual size 0x28F6, ending at 0x005358F6) and its image value is 0, so its initialiser is 0 and its symbol class is `B`; this
//   image has no `.bss` section, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol classification rather than the
//   section. **Note that this body's id slot is at 0x0057BF7C, NOT in the 0x0053Cxxx run that the neighbouring registrars use** - the 0x0053C354
//   and 0x0053C338 addresses here are string literals, while the writable id lives in the 0x0057Bxxx block, so an id read from 0x0057BF7C and a
//   name read from 0x0053C354 are in different sections and must not be conflated. The only store this body makes to the id slot is a WORD store
//   (`mov word [0x57BF7C],ax`), so the port declares it as a 16-bit object; the later dword reload with the 0xFFFF mask is preserved at the
//   registration site, where it is what retail actually executes. The id store lands BETWEEN the pushes of the first link, exactly as in the
//   neighbouring registrars. Deferred pushes: at every getter-fed link site the handler immediate is pushed before the event-id getter call and
//   consumed as the link's third argument. Handler immediates 0x0043BE80, 0x0043C0B0, 0x00449950, 0x0043C1F0, 0x0043C2A0, 0x0043C2B0 and
//   0x0043C2D0 are raw code immediates with no implementation in the tree yet; 0x00449950 is shared with other bodies in this family. Documented
//   shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the caller
//   pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) the cleanups are reproduced implicitly by
//   the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38, 0xC (three times) and 0x14 are recorded so a
//   future reviewer can check the arithmetic, and the 0x14 is the one worth checking, since it spans two calls; (3) byte-valued event ids are
//   zero-extended in the port, which cannot change behaviour because every link fed by a byte getter goes through 0x00401400 or 0x00401530, which
//   mask the event word for the phase index.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);            // 0x004D9FA0: parent type getter (sys/native_4d9fa0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56D0(void);              // 0x004C56D0: argument-less byte getter (dynobj/native_4c56d0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56E0(void);              // 0x004C56E0: argument-less byte getter (dynobj/native_4c56e0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);          // 0x00416840: argument-less byte getter (config/native_416840.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416860(void);          // 0x00416860: argument-less byte getter (config/native_416860.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0057BF7C: the "ExplodeMagicObj" task type id, in writable `.data` (NOT in the 0x0053Cxxx string run). Image value 0, so the initialiser is 0 and
// the symbol class is `B`. Only the low 16 bits are ever stored.
std::uint16_t g_57bf7c = 0U;

void Task_DefineExplodeMagicObj()
{
    // 0x0043BFD0: push 0 (TaskType_Register's fourth argument, pushed first); 0x0043BFD2: push 0 (the parent getter's inert argument);
    // 0x0043BFD4: call 0x4D9FA0; 0x0043BFD9: push eax; 0x0043BFDA: push 0x150; 0x0043BFDF: push "ExplodeMagicObj"; 0x0043BFE4: call 0x401900.
    // This body has no `sub esp` and no saved-register pushes, so there is no local frame and no frame-relative address to resolve.
    const std::uint16_t explode_magic_obj_id = TaskType_Register("ExplodeMagicObj",     // 0x0043BFDF: push 0x0053C354
                                                                 0x150U,                 // 0x0043BFDA: push 0x150 (object size)
                                                                 Sys_GetGlobal16_4D9FA0(), // 0x0043BFD4: call 0x4D9FA0 (parent type)
                                                                 0U);                   // 0x0043BFD2: push 0 (fourth argument)
    ::g_57bf7c = explode_magic_obj_id; // 0x0043BFF1: mov word [0x57BF7C],ax - between the pushes of the first link

    // 0x0043BFE9: push 0x0043BE80 (handler, deferred argument 3); 0x0043BFEE: push 0x0 (argument 2, literal event id);
    // 0x0043BFF0: push eax (argument 1); 0x0043BFF7: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bf7c), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043BE80U)));

    // 0x0043BFFC: mov ax,[0x57BF7C]; 0x0043C002: push 0x0043C0B0 (handler); 0x0043C007: push 0x1 (event id); 0x0043C009: push eax (argument 1);
    // 0x0043C00A: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bf7c), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043C0B0U)));

    // 0x0043C00F: mov cx,[0x57BF7C]; 0x0043C016: push 0x00449950 (handler); 0x0043C01B: push 0x2 (event id); 0x0043C01D: push ecx (argument 1);
    // 0x0043C01E: call 0x401530; 0x0043C023: add esp,0x38 - fourteen pushes.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bf7c), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00449950U)));

    // 0x0043C026: push 0x0043C1F0 (handler, deferred argument 3); 0x0043C02B: call 0x4C56D0 (event id; the pushed handler is not read);
    // 0x0043C030: mov dx,[0x57BF7C]; 0x0043C037: push eax (argument 2); 0x0043C038: push edx (argument 1); 0x0043C039: call 0x401400;
    // 0x0043C03E: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bf7c),
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56D0()), // 0x0043C037: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043C1F0U)));

    // 0x0043C041: push 0x0043C2A0 (handler, deferred argument 3); 0x0043C046: call 0x4C56E0; 0x0043C04B: push eax (argument 2);
    // 0x0043C04C: mov ax,[0x57BF7C]; 0x0043C052: push eax (argument 1); 0x0043C053: call 0x401400; 0x0043C058: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bf7c),
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56E0()), // 0x0043C04B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043C2A0U)));

    // 0x0043C05B: push 0x0043C2B0 (handler, deferred argument 3); 0x0043C060: call 0x416840; 0x0043C065: mov cx,[0x57BF7C];
    // 0x0043C06C: push eax (argument 2); 0x0043C06D: push ecx (argument 1); 0x0043C06E: call 0x401400; 0x0043C073: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bf7c),
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416840()), // 0x0043C06C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043C2B0U)));

    // 0x0043C076: push 0x0043C2D0 (handler, deferred argument 3); 0x0043C07B: call 0x416860; 0x0043C080: mov dx,[0x57BF7C];
    // 0x0043C087: push eax (argument 2); 0x0043C088: push edx (argument 1); 0x0043C089: call 0x401400. NOTE: no `add esp` follows this call -
    // its three pushes are cleaned up together with the two below by the final `add esp,0x14`.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bf7c),
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416860()), // 0x0043C087: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043C2D0U)));

    // 0x0043C08E: mov eax,[0x57BF7C]; 0x0043C093: and eax,0xFFFF (the id is reloaded as a DWORD and masked, which the port reproduces);
    // 0x0043C098: push eax (argument 2); 0x0043C099: push 0x0053C338 (argument 1, "TASKTYPE_EXPLODEMAGICOBJ"); 0x0043C09E: call 0x4B8810;
    // 0x0043C0A3: add esp,0x14 - this seventh link's three pushes plus these two.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_EXPLODEMAGICOBJ"), // 0x0043C099: push 0x0053C338
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_57bf7c) & 0xFFFFU))); // 0x0043C098: push eax

    // 0x0043C0A6: ret - no epilogue, because this body has no frame and no saved registers to restore.
} // end of Task_DefineExplodeMagicObj

} // extern "C"
