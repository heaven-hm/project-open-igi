// Native: Keyboard_Update_490230 (placeholder until VERIFIED); retail VA 0x00490230-0x004902E1 (177 bytes, 11 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 full bytes/disassembly; Binary Ninja MCP decompile; DINPUT-BODIES packet section 5.
// Behavior/CFG: if g_5C8E30 is clear, Acquire the keyboard and latch g_5C8E30 on success; snapshot C28C62 into C28D62; if g_C28C60 is set clear C28C62 and return 0; else if Input_GetInstance_48F270() != 0 copy g_C28E80 over C28C62 and return it; else GetDeviceState(0x100, C28C62), on failure re-Acquire then retry once, clearing C28C62 after either failed attempt; return the last GetDeviceState result (0 after a clear).
// Status/limits: TRANSCRIBED; deps 0x0048F270 and the COM interface only; the state blocks are declared in dinput_globals.hpp. Runtime UNOBSERVED.
#include "igi/retail/input/dinput_globals.hpp"
#include "igi/retail/input/native_48f270.hpp"
#include "igi/retail/render/display_state_flags.hpp"

#include <cstdint>

extern "C" {

std::int32_t Keyboard_Update_490230()
{
    using AcquireFn = std::int32_t(__stdcall*)(void*);
    using GetDeviceStateFn = std::int32_t(__stdcall*)(void*, std::int32_t, void*);
    // 0x00490230-0x0049024E: no Acquire attempt while the acquired flag is set.
    if (g_5c8e30 == 0)
    {
        void* const device = g_5c8e24;
        void* const* const vtable = *reinterpret_cast<void* const* const*>(device);
        if (reinterpret_cast<AcquireFn>(vtable[0x1C / 4])(device) == 0)
        {
            g_5c8e30 = 1;
        }
    }
    // 0x00490254-0x00490264: previous := current (0x100 bytes).
    for (std::int32_t i = 0; i < 0x40; ++i)
    {
        reinterpret_cast<std::uint32_t*>(g_c28d62)[i] = reinterpret_cast<std::uint32_t const*>(g_c28c62)[i];
    }
    // 0x00490267-0x00490279: disable byte forces an empty state and returns 0.
    if (g_c28c60 != 0)
    {
        for (std::int32_t i = 0; i < 0x40; ++i)
        {
            reinterpret_cast<std::uint32_t*>(g_c28c62)[i] = 0;
        }
        return 0;
    }
    // 0x0049027F-0x0049029A: non-zero instance handle uses the win-proc-written scan state.
    std::int32_t result = Input_GetInstance_48F270();
    if (result != 0)
    {
        for (std::int32_t i = 0; i < 0x40; ++i)
        {
            reinterpret_cast<std::uint32_t*>(g_c28c62)[i] = reinterpret_cast<std::uint32_t const*>(g_c28e80)[i];
        }
        return result;
    }
    // 0x0049029E-0x004902DB: DirectInput state; one retry after a re-Acquire.
    void* const device = g_5c8e24;
    void* const* const vtable = *reinterpret_cast<void* const* const*>(device);
    result = reinterpret_cast<GetDeviceStateFn>(vtable[0x24 / 4])(device, 0x100, g_c28c62);
    if (result != 0)
    {
        if (reinterpret_cast<AcquireFn>(vtable[0x1C / 4])(device) != 0)
        {
            for (std::int32_t i = 0; i < 0x40; ++i)
            {
                reinterpret_cast<std::uint32_t*>(g_c28c62)[i] = 0;
            }
            result = 0;
        }
        else
        {
            result = reinterpret_cast<GetDeviceStateFn>(vtable[0x24 / 4])(device, 0x100, g_c28c62);
            if (result != 0)
            {
                for (std::int32_t i = 0; i < 0x40; ++i)
                {
                    reinterpret_cast<std::uint32_t*>(g_c28c62)[i] = 0;
                }
                result = 0;
            }
        }
    }
    return result;
}

} // extern "C"
