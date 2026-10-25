// Native: Task_DefineRadarDish (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown);
//   retail VA 0x00448D30-0x00448E22.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 243 bytes (0xF3), 1 block,
//   64 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 1 block, 64 instructions, 0 edges (straight-line; the
//   `add esp,0x14` at 0x00448E1F followed by the ret is the only exit). The objdump listing build/retail-dis.txt was read for the same extent.
//   `ps` at 0x0053C848 (byte-identical to "RadarDish") and at 0x0053C834 (byte-identical to "TASKTYPE_RADARDISH"). `px 4` at 0x0057C20C gives zero.
//   **This body has NO FRAME AT ALL**: there is no `sub esp` anywhere in it, so no frame-relative arithmetic exists to reconcile and no `lea` is
//   resolved against a frame base. A push-by-push simulation closes exactly: 5 + 8*3 + 2 = 31 pushes = 124 bytes against `add esp,0x38` + four
//   `add esp,0xC` + `add esp,0x14` = 124 bytes; `esp` is 0 at the ret.
// Behavior/CFG: registers the "RadarDish" task type of size 0x180 with parent Sys_GetGlobal16_4D9FA0() into the word at 0x0057C20C, links EIGHT
//   handlers (six 0x00401400 and two 0x00401530), and hash-registers the type under "TASKTYPE_RADARDISH". **It allocates NO container and appends
//   NOTHING**, so it has no parameter block at all - the third body in this lane with no container.
//   Links: 0x00448B70/e0, 0x00448E30/e1 and 0x00448CF0/e2 through the literal event ids 0, 1 and 2; then 0x004F1A70->0x00449180,
//   0x004C56D0->0x00449220, 0x004C56E0->0x004EFCD0, 0x00416840->0x00449260 and 0x004E6610->0x00449270.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Census: 0x00401400 x6, 0x00401530 x2 (8 links), 0x00401900 x1, 0x00416840 x1, 0x004B8810 x1,
//   0x004C56D0 x1, 0x004C56E0 x1, 0x004D9FA0 x1, 0x004E6610 x1, 0x004F1A70 x1 = 16 sites / 10 distinct.
//   Cleanup reconciliation, each `add esp` matched to the pushes it removes: 0x00448D83 `add esp,0x38` = the five registration pushes plus links 1-3;
//   FOUR `add esp,0xC` at 0x00448D9E, 0x00448DB8, 0x00448DD3 and 0x00448DEE = links 4-7, one each; 0x00448E1F `add esp,0x14` = the EIGHTH link's
//   three pushes (which have no cleanup of their own) plus the hash registration's two. Totals as above; the arithmetic closes with no frame term.
//   **Three numbers: this body has NO container, so the append accounting is VACUOUS, not passed.** There are zero static append sites and no
//   allocation call, so nothing here corroborates or refutes a capacity relationship; it is recorded as inapplicable rather than as agreement.
//   **The "registered size equals frame size" shape is also inapplicable here, and for the same reason**: the registered size is 0x180 while the body
//   establishes no frame at all, so there is no frame to compare it against. A body with no frame cannot corroborate a size, and this file does not
//   claim that it does.
//   The hash registration again truncates the id explicitly: 0x00448E08 `mov ecx,[0x57C20C]`, 0x00448E0E `and $0xFFFF,%ecx`, 0x00448E14 `push ecx`,
//   0x00448E15 `push 0x0053C834`, 0x00448E1A `call 0x004B8810`.
//   Documented shape differences, all value-preserving: (1) argument-less getters ignore the arguments retail pushes at their call sites and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced; (2) the `add esp` cleanups are reproduced implicitly by the tree's
//   calling convention rather than by explicit arithmetic, with the retail sizes recorded above; (3) byte-valued getters are zero-extended into the
//   32-bit link arguments in the port while retail leaves the upper bits of eax undefined - no effect on the links, both of which mask the event word;
//   (4) the `and $0xFFFF` before the hash registration is reproduced explicitly as a mask on the id word, because retail executes it as a real
//   instruction rather than relying on the load's width.
//   Cross-references: handler 0x004EFCD0 IS transcribed (script/native_4efcd0.cpp); the other SEVEN handler immediates (0x00448B70, 0x00448E30,
//   0x00448CF0, 0x00449180, 0x00449220, 0x00449260, 0x00449270) have no transcription anywhere in the tree. Parent getter 0x004D9FA0 is the same
//   parent "Drawer" registers under in native_443ed0.cpp.
//   Unestablished: why this type needs no parameter container while its neighbours in the same lane do; the meaning of the registered size 0x180 in the
//   absence of a frame; and the seven untranscribed handler bodies.

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// Callees, each with the return type and linkage of its own transcribed file. 0x00401400, 0x00401530 and 0x00401900 come from
// igi/retail/core/task_types.hpp (core/task_types.cpp) and are not redeclared here.
extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);      // 0x004D9FA0: parent type getter (sys/native_4d9fa0.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);   // 0x004F1A70 (script/native_4f1a70.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56D0(void);        // 0x004C56D0 (dynobj/native_4c56d0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56E0(void);        // 0x004C56E0 (dynobj/native_4c56e0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);    // 0x00416840 (config/native_416840.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_4E6610(void); // 0x004E6610 (core/native_004e6610.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0057C20C: the "RadarDish" task type id (word). Image value 0 -> initialiser 0, symbol class `B`, writable `.data`: `.rdata` ends at 0x005358F6 and
// `.data` starts at 0x00536000, and this image has no `.bss`, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol
// classification rather than the section. No other file in the tree owns it. This body has no other global: it allocates no container.
std::uint16_t g_57c20c = 0U;

void Task_DefineRadarDish()
{
    // This body has NO FRAME: retail contains no `sub esp` at all, so there is no frame-relative arithmetic to reproduce and no local frame storage.
    // 0x00448D30: push 0; 0x00448D32: push 0; 0x00448D34: call 0x4D9FA0 (parent type); 0x00448D39: push eax; 0x00448D3A: push 0x180;
    // 0x00448D3F: push 0x0053C848; 0x00448D44: call 0x401900.
    const std::uint16_t radar_dish_id = TaskType_Register("RadarDish",               // 0x00448D3F: push 0x0053C848
                                                          0x180U,                    // 0x00448D3A: push 0x180 (object size)
                                                          Sys_GetGlobal16_4D9FA0(),  // 0x00448D34: call 0x4D9FA0 (parent type)
                                                          0U);                       // 0x00448D32: push 0
    ::g_57c20c = radar_dish_id; // 0x00448D51: mov word [0x57C20C],ax

    // 0x448D49: push 0x448B70 (handler, deferred argument 3);
    // 0x448D4E: push $0x0 (argument 2, literal event id);
    // 0x448D50: push %eax (argument 1, still the id TaskType_Register returned); 0x448D57: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(radar_dish_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x448B70U)));

    // 0x448D62: push 0x448E30 (handler, deferred argument 3);
    // 0x448D67: push $0x1 (argument 2, literal event id);
    // 0x448D5C: mov [0x57c20c] into a register (argument 1, the id RELOADED from the global); 0x448D6A: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c20c), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x448E30U)));

    // 0x448D76: push 0x448CF0 (handler, deferred argument 3);
    // 0x448D7B: push $0x2 (argument 2, literal event id);
    // 0x448D6F: mov [0x57c20c] into a register (argument 1, the id RELOADED from the global); 0x448D7E: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c20c), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x448CF0U)));

    // 0x448D86: push 0x449180 (handler, deferred argument 3);
    // 0x448D8B: call Script_PhaseIdGetE5_4F1A70 (supplies argument 2, the event id);
    // 0x448D90: mov [0x57c20c] into a register (argument 1, the id RELOADED from the global); 0x448D99: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c20c), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449180U)));

    // 0x448DA1: push 0x449220 (handler, deferred argument 3);
    // 0x448DA6: call DynObj_GetByte_4C56D0 (supplies argument 2, the event id);
    // 0x448DAC: mov [0x57c20c] into a register (argument 1, the id RELOADED from the global); 0x448DB3: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c20c), static_cast<std::uint32_t>(DynObj_GetByte_4C56D0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449220U)));

    // 0x448DBB: push 0x4EFCD0 (handler, deferred argument 3);
    // 0x448DC0: call DynObj_GetByte_4C56E0 (supplies argument 2, the event id);
    // 0x448DC5: mov [0x57c20c] into a register (argument 1, the id RELOADED from the global); 0x448DCE: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c20c), static_cast<std::uint32_t>(DynObj_GetByte_4C56E0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4EFCD0U)));

    // 0x448DD6: push 0x449260 (handler, deferred argument 3);
    // 0x448DDB: call Sys_GetSettingByte_416840 (supplies argument 2, the event id);
    // 0x448DE0: mov [0x57c20c] into a register (argument 1, the id RELOADED from the global); 0x448DE9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c20c), static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449260U)));

    // 0x448DF1: push 0x449270 (handler, deferred argument 3);
    // 0x448DF6: call Task_GetCachedEventId_4E6610 (supplies argument 2, the event id);
    // 0x448DFC: mov [0x57c20c] into a register (argument 1, the id RELOADED from the global); 0x448E03: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c20c), static_cast<std::uint32_t>(Task_GetCachedEventId_4E6610()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x449270U)));

    // 0x00448E08: mov ecx,[0x57C20C]; 0x00448E0E: and $0xFFFF,%ecx (an explicit 16-bit truncation of the id word, reproduced as a real mask);
    // 0x00448E14: push ecx; 0x00448E15: push 0x0053C834; 0x00448E1A: call 0x004B8810. The pushed order makes the NAME the first argument and the
    // masked id the second. The "TASKTYPE_RADARDISH" literal at 0x0053C834 is a DIFFERENT string from the type name at 0x0053C848.
    const std::uint32_t registered_id = static_cast<std::uint32_t>(::g_57c20c) & 0xFFFFU; // 0x00448E0E: and $0xffff,%ecx
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_RADARDISH"),                     // 0x00448E15: push 0x0053C834
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(registered_id))); // 0x00448E14: push ecx

    // 0x00448E1F: add esp,0x14 - the eighth link's three outstanding pushes plus this hash registration's two; 0x00448E22: ret.
} // end of Task_DefineRadarDish

} // extern "C"
