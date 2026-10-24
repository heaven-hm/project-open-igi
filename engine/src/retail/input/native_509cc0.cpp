// Native: Joypad_IsAcquired_509CC0 (placeholder until VERIFIED); retail VA 0x00509CC0-0x00509CE8 (41 bytes, 3 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 disassembly/bytes; Binary Ninja MCP decompile; DINPUT-LAYER packet (COM surface: Acquire = vtable 0x1C).
// Behavior/CFG: if (!g_A7B4E0[index]) return 0; device = g_A7B2E0[index]; return device->Acquire() >= 0.
// Status/limits: TRANSCRIBED; only an indirect COM call on runtime device data - no native dependencies. Runtime UNOBSERVED.
#include "igi/retail/input/dinput_globals.hpp"

#include <cstdint>

extern "C" {

std::int32_t Joypad_IsAcquired_509CC0(std::int32_t index)
{
    // 0x00509CC0: test dword [0xA7B4E0 + index*4]; je return 0.
    if (g_a7b4e0[index] == nullptr)
    {
        return 0;
    }
    // 0x00509CCE: mov eax,[0xA7B2E0 + index*4]; mov ecx,[eax]; call [ecx+0x1C].
    void* device = g_a7b2e0[index];
    auto const* const* vtable = *reinterpret_cast<void* const* const*>(device);
    using AcquireFn = std::int32_t(__stdcall*)(void*);
    const std::int32_t hr = reinterpret_cast<AcquireFn>(vtable[0x1C / 4])(device);
    return hr >= 0 ? 1 : 0;
}

} // extern "C"
