// Native: Material_DefineScriptNatives (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00407D60-0x00407DD4.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 117 bytes, 3 blocks, 31 instructions.
// Tools/evidence: radare2 6.2.0 per-instruction listing over the exact image (loop 0x00407D60-0x00407D72, linear 0x00407D74-0x00407DD1, epilogue 0x00407DD4); the three name literals read as raw .rdata bytes at 0x00538544/0x00538534/0x00538520 and the three event-id bytes at 0x00538518/19/1A; cleanup constant cross-checked against the push count.
// Behavior/CFG: clears the first byte of each of the 32 records of the table at 0x00569474 (stride 0x268, bound 0x0056E174 = 0x00569474 + 0x4D00), stores the low byte of three QTask_AllocateEvent(0) ids at 0x00538518/19/1A, then registers three script-callable material entry points through 0x004B8890 with the argument tuple (name, handler, 0, -1): "SetActiveMaterial" -> 0x00407DE0, "DefineQMaterial" -> 0x00407E30, "DefineGameMaterial" -> 0x00407F50. The epilogue's `add esp,0x3C` is 3*4 bytes (allocate pushes) + 3*16 bytes (registration pushes), independently confirming that these seven calls are the whole call set.
// Status/limits: BUILD_VERIFIED; retail leaves EAX holding the last registration call's (void) result and no caller consumes it, so this native returns nothing. The three handlers are raw code immediates in retail and are not transcribed yet, so they are passed as address constants exactly as retail pushes them (0x00407DE0 / 0x00407E30 / 0x00407F50 are still native gaps). Runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" void Script_QHashNodeRegisterEx(void* arg0, void* arg1, char const* arg2, void* arg3); // 0x004B8890

// Retail BSS/data the routine touches. The material table is 32 records of 0x268 bytes at 0x00569474..0x0056E174; this routine only
// clears the byte at each record's start, so the region is declared as raw bytes with the retail stride rather than as an invented type.
extern "C" std::uint8_t g_569474[0x4D00] = {}; // retail 0x00569474-0x0056E174
extern "C" std::uint8_t g_538518 = 0xFFU;      // retail 0x00538518; image value 0xFF, not 0
extern "C" std::uint8_t g_538519 = 0xFFU;      // retail 0x00538519; image value 0xFF, not 0
extern "C" std::uint8_t g_53851a = 0xFFU;      // retail 0x0053851A; image value 0xFF, not 0

extern "C" {

void Material_DefineScriptNatives()
{
    // 0x00407D60: mov eax,0x569474; 0x00407D65: mov byte [eax],0; 0x00407D68: add eax,0x268; 0x00407D6D: cmp eax,0x56E174; jl.
    for (std::uint32_t offset = 0U; offset < 0x4D00U; offset += 0x268U)
    {
        g_569474[offset] = 0U;
    }

    g_538518 = static_cast<std::uint8_t>(QTask_AllocateEvent(0)); // 0x00407D74/0x00407D76/0x00407D7D: mov byte [0x538518],al
    g_538519 = static_cast<std::uint8_t>(QTask_AllocateEvent(0)); // 0x00407D7B/0x00407D82/0x00407D89: mov byte [0x538519],al
    g_53851a = static_cast<std::uint8_t>(QTask_AllocateEvent(0)); // 0x00407D87/0x00407D8E/0x00407DA1: mov byte [0x53851A],al

    Script_QHashNodeRegisterEx( // 0x00407DA6: call 0x4B8890
        const_cast<char*>("SetActiveMaterial"), // 0x00407D9C: push 0x00538544
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00407DE0U)), // 0x00407D97: push 0x00407DE0
        nullptr, // 0x00407D95: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x00407D93: push -1
    Script_QHashNodeRegisterEx( // 0x00407DB9: call 0x4B8890
        const_cast<char*>("DefineQMaterial"), // 0x00407DB4: push 0x00538534
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00407E30U)), // 0x00407DAF: push 0x00407E30
        nullptr, // 0x00407DAD: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x00407DAB: push -1
    Script_QHashNodeRegisterEx( // 0x00407DCC: call 0x4B8890
        const_cast<char*>("DefineGameMaterial"), // 0x00407DC7: push 0x00538520
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00407F50U)), // 0x00407DC2: push 0x00407F50
        nullptr, // 0x00407DC0: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x00407DBE: push -1
    // 0x00407DD1: add esp,0x3C - cdecl cleanup of the seven pushes above; 0x00407DD4: ret.
}

} // extern "C"
