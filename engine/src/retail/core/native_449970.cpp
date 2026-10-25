// Native: Task_DefineGlass (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x00449970-0x00449A7D.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 270 bytes (0x10E), 1 block,
//   71 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 1 block, 71 instructions, 0 edges (straight-line; the
//   `add esp,0x14` at 0x00449A7A followed by the ret is the only exit). The objdump listing build/retail-dis.txt was read for the same extent.
//   `ps` at 0x0053C878 (byte-identical to "Glass") and at 0x0053C868 (byte-identical to "TASKTYPE_GLASS"). `px 4` at 0x0057C220 gives zero.
//   **This body has NO FRAME AT ALL**: there is no `sub esp` anywhere in it, so no frame-relative arithmetic exists and no `lea` is resolved against
//   a frame base. A push-by-push simulation closes exactly: 5 + 9*3 + 2 = 34 pushes = 136 bytes against `add esp,0x38` + five `add esp,0xC` +
//   `add esp,0x14` = 136 bytes; `esp` is 0 at the ret.
// Behavior/CFG: registers the "Glass" task type of size 0x148 with parent Sys_GetGlobal16_4D9FA0() into the word at 0x0057C220, links NINE handlers
//   (seven 0x00401400 and two 0x00401530), and hash-registers the type under "TASKTYPE_GLASS". It allocates NO container and appends NOTHING.
//   Links: 0x004497F0/e0, 0x00449A80/e1 and 0x00449950/e2 through the literal event ids 0, 1 and 2; then 0x004C56D0->0x004D3710 (the shared no-op
//   handler), 0x004C56E0->0x00427D70, 0x00416840->0x00449B00, 0x00416860->0x00449BA0, 0x004FC860->0x00449CB0 and 0x00489610->0x0045F920.
//   Note the two handlers 0x004D3710 and 0x00427D70 are SHARED code addresses rather than this family's own 0x00449xxx bodies, and 0x0045F920 is a
//   shared handler too (thirty-two link sites image-wide by instruction-start count).
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Census: 0x00401400 x7, 0x00401530 x2 (9 links), 0x00401900 x1, 0x00416840 x1, 0x00416860 x1,
//   0x00489610 x1, 0x004B8810 x1, 0x004C56D0 x1, 0x004C56E0 x1, 0x004D9FA0 x1, 0x004FC860 x1 = 18 sites / 11 distinct.
//   Cleanup reconciliation, each `add esp` matched to the pushes it removes: 0x004499C3 `add esp,0x38` = the five registration pushes plus links 1-3;
//   FIVE `add esp,0xC` at 0x004499DE, 0x004499F8, 0x00449A13, 0x00449A2E and 0x00449A48 = links 4-8, one each; 0x00449A7A `add esp,0x14` = the
//   NINTH link's three pushes (which have no cleanup of their own) plus the hash registration's two. Totals as above; the arithmetic closes with no
//   frame term.
//   **Two degenerate checks, recorded as inapplicable rather than as passes:** this body has NO container (zero static append sites and no allocation
//   call), so the three-number check is VACUOUS; and it has NO frame, so the "registered size equals frame size" shape cannot be evaluated - the
//   registered size 0x148 is compared against nothing here.
//   The hash registration truncates the id explicitly: 0x00449A63 `mov edx,[0x57C220]`, 0x00449A69 `and $0xFFFF,%edx`, 0x00449A6F `push edx`,
//   0x00449A70 `push 0x0053C868`, 0x00449A75 `call 0x004B8810`.
//   Documented shape differences, all value-preserving: (1) argument-less getters ignore the arguments retail pushes at their call sites and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced; (2) the `add esp` cleanups are reproduced implicitly by the tree's
//   calling convention rather than by explicit arithmetic, with the retail sizes recorded above; (3) byte-valued getters are zero-extended into the
//   32-bit link arguments in the port while retail leaves the upper bits of eax undefined - no effect on the links, both of which mask the event word;
//   (4) the `and $0xFFFF` before the hash registration is reproduced explicitly as a mask on the id word, because retail executes it as a real
//   instruction rather than relying on the load's width.
//   Cross-references: parent getter 0x004D9FA0 is the same parent "Drawer" (native_443ed0.cpp) and "RadarDish" (native_448d30.cpp) register under.
//   Handler 0x00427D70 IS transcribed (script/native_427d70.cpp); 0x004D3710 is the shared no-op; the remaining SEVEN handler immediates
//   (0x004497F0, 0x00449A80, 0x00449950, 0x00449B00, 0x00449BA0, 0x00449CB0, 0x0045F920) have no transcription anywhere in the tree.
//   Unestablished: why a shared handler address (0x0045F920, thirty-two sites) is used for one event while this family's own bodies serve the rest;
//   the meaning of the registered size 0x148 in the absence of a frame; and the seven untranscribed handler bodies.

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// Callees, each with the return type and linkage of its own transcribed file. 0x00401400, 0x00401530 and 0x00401900 come from
// igi/retail/core/task_types.hpp (core/task_types.cpp) and are not redeclared here.
extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);      // 0x004D9FA0: parent type getter (sys/native_4d9fa0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56D0(void);        // 0x004C56D0 (dynobj/native_4c56d0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56E0(void);        // 0x004C56E0 (dynobj/native_4c56e0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);    // 0x00416840 (config/native_416840.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416860(void);    // 0x00416860 (config/native_416860.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_4FC860(void);        // 0x004FC860 (sys/native_4fc860.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);        // 0x00489610 (sys/native_489610.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0057C220: the "Glass" task type id (word). Image value 0 -> initialiser 0, symbol class `B`, writable `.data`: `.rdata` ends at 0x005358F6 and
// `.data` starts at 0x00536000, and this image has no `.bss`, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol
// classification rather than the section. No other file in the tree owns it. This body has no other global: it allocates no container.
std::uint16_t g_57c220 = 0U;

void Task_DefineGlass()
{
    // This body has NO FRAME: retail contains no `sub esp` at all, so there is no frame-relative arithmetic to reproduce and no local frame storage.
    // 0x00449970: push 0; 0x00449972: push 0; 0x00449974: call 0x4D9FA0 (parent type); 0x00449979: push eax; 0x0044997A: push 0x148;
    // 0x0044997F: push 0x0053C878; 0x00449984: call 0x401900.
    const std::uint16_t glass_id = TaskType_Register("Glass",                     // 0x0044997F: push 0x0053C878
                                                     0x148U,                      // 0x0044997A: push 0x148 (object size)
                                                     Sys_GetGlobal16_4D9FA0(),    // 0x00449974: call 0x4D9FA0 (parent type)
                                                     0U);                         // 0x00449972: push 0
    ::g_57c220 = glass_id; // 0x00449991: mov word [0x57C220],ax

    // 0x449989: push 0x4497F0 (handler, deferred argument 3);
    // 0x44998E: push $0x0 (argument 2, literal event id);
    // 0x449990: push %eax (argument 1, still the id TaskType_Register returned); 0x449997: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(glass_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4497F0U)));

    // 0x4499A2: push 0x449A80 (handler, deferred argument 3);
    // 0x4499A7: push $0x1 (argument 2, literal event id);
    // 0x44999C: mov [0x57c220] into a register (argument 1, the id RELOADED from the global); 0x4499AA: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c220), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449A80U)));

    // 0x4499B6: push 0x449950 (handler, deferred argument 3);
    // 0x4499BB: push $0x2 (argument 2, literal event id);
    // 0x4499AF: mov [0x57c220] into a register (argument 1, the id RELOADED from the global); 0x4499BE: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c220), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449950U)));

    // 0x4499C6: push 0x4D3710 (handler, deferred argument 3);
    // 0x4499CB: call DynObj_GetByte_4C56D0 (supplies argument 2, the event id);
    // 0x4499D0: mov [0x57c220] into a register (argument 1, the id RELOADED from the global); 0x4499D9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c220), static_cast<std::uint32_t>(DynObj_GetByte_4C56D0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D3710U)));

    // 0x4499E1: push 0x427D70 (handler, deferred argument 3);
    // 0x4499E6: call DynObj_GetByte_4C56E0 (supplies argument 2, the event id);
    // 0x4499EC: mov [0x57c220] into a register (argument 1, the id RELOADED from the global); 0x4499F3: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c220), static_cast<std::uint32_t>(DynObj_GetByte_4C56E0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x427D70U)));

    // 0x4499FB: push 0x449B00 (handler, deferred argument 3);
    // 0x449A00: call Sys_GetSettingByte_416840 (supplies argument 2, the event id);
    // 0x449A05: mov [0x57c220] into a register (argument 1, the id RELOADED from the global); 0x449A0E: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c220), static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449B00U)));

    // 0x449A16: push 0x449BA0 (handler, deferred argument 3);
    // 0x449A1B: call Sys_GetSettingByte_416860 (supplies argument 2, the event id);
    // 0x449A20: mov [0x57c220] into a register (argument 1, the id RELOADED from the global); 0x449A29: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c220), static_cast<std::uint32_t>(Sys_GetSettingByte_416860()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449BA0U)));

    // 0x449A31: push 0x449CB0 (handler, deferred argument 3);
    // 0x449A36: call Sys_GetGlobal8_4FC860 (supplies argument 2, the event id);
    // 0x449A3C: mov [0x57c220] into a register (argument 1, the id RELOADED from the global); 0x449A43: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c220), static_cast<std::uint32_t>(Sys_GetGlobal8_4FC860()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449CB0U)));

    // 0x449A4B: push 0x45F920 (handler, deferred argument 3);
    // 0x449A50: call Sys_GetGlobal8_489610 (supplies argument 2, the event id);
    // 0x449A55: mov [0x57c220] into a register (argument 1, the id RELOADED from the global); 0x449A5E: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c220), static_cast<std::uint32_t>(Sys_GetGlobal8_489610()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F920U)));

    // 0x00449A63: mov edx,[0x57C220]; 0x00449A69: and $0xFFFF,%edx (an explicit 16-bit truncation of the id word, reproduced as a real mask);
    // 0x00449A6F: push edx; 0x00449A70: push 0x0053C868; 0x00449A75: call 0x004B8810. The pushed order makes the NAME the first argument and the
    // masked id the second. The "TASKTYPE_GLASS" literal at 0x0053C868 is a DIFFERENT string from the type name at 0x0053C878.
    const std::uint32_t registered_id = static_cast<std::uint32_t>(::g_57c220) & 0xFFFFU; // 0x00449A69: and $0xffff,%edx
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GLASS"),                         // 0x00449A70: push 0x0053C868
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(registered_id))); // 0x00449A6F: push edx

    // 0x00449A7A: add esp,0x14 - the ninth link's three outstanding pushes plus this hash registration's two; 0x00449A7D: ret.
} // end of Task_DefineGlass

} // extern "C"
