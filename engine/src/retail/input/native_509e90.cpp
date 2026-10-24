// Native: Joypad_ReleaseAll_509E90 (placeholder until VERIFIED); retail VA 0x00509E90-0x00509EFF (112 bytes, 9 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 disassembly/bytes; Binary Ninja MCP decompile; COM surface: Unacquire 0x20, Release 0x08.
// Behavior/CFG: if (!g_A7B4F0) { g_A7B638 = 0; return; } else for i < g_A7B638: Unacquire + Release each g_A7B4E0[i] and null it;
//           then Release g_A7B4F0 itself, set g_A7B4F0 = 0 and g_A7B638 = 0.
// Status/limits: TRANSCRIBED; only indirect COM calls on runtime data - no native dependencies. Runtime UNOBSERVED.
#include "igi/retail/input/dinput_globals.hpp"

#include <cstdint>

extern "C" {

void Joypad_ReleaseAll_509E90()
{
    // 0x00509E90-0x00509E9F: no DirectInput object -> count = 0 and return.
    if (g_a7b4f0 == nullptr)
    {
        g_a7b638 = 0;
        return;
    }
    // 0x00509EA0-0x00509EEF: unacquire/release each device, clear the slot pointer.
    for (std::int32_t i = 0; i < g_a7b638; ++i)
    {
        void* const device = g_a7b4e0[i];
        if (device != nullptr)
        {
            auto const* const* vtable = *reinterpret_cast<void* const* const*>(device);
            using UnacquireFn = std::int32_t(__stdcall*)(void*);
            using ReleaseFn = std::int32_t(__stdcall*)(void*);
            reinterpret_cast<UnacquireFn>(vtable[0x20 / 4])(device);
            reinterpret_cast<ReleaseFn>(vtable[0x08 / 4])(device);
            g_a7b4e0[i] = nullptr;
        }
    }
    // 0x00509EF0-0x00509EFF: release the DirectInput object and clear the globals.
    auto const* const* diVtable = *reinterpret_cast<void* const* const*>(g_a7b4f0);
    using ReleaseFn = std::int32_t(__stdcall*)(void*);
    reinterpret_cast<ReleaseFn>(diVtable[0x08 / 4])(g_a7b4f0);
    g_a7b4f0 = nullptr;
    g_a7b638 = 0;
}

} // extern "C"
