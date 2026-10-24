// Native: Input_MouseGetCaps_48FD90 (placeholder until VERIFIED); retail VA 0x0048FD90-0x0048FDD1 (66 bytes, 4 blocks, BN alias FX_CouldNotGetMouseCaps).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP disassembly/decompile + radare2 6.2.0 bytes; DINPUT-LAYER packet §3.4 (COM GetCapabilities = vtable +0x0C).
// Behavior/CFG: builds caps{dwSize=0x2C}, calls the mouse device's GetCapabilities; on failure Debug_Error("Could not get mouse caps!") and spin; otherwise stores 0x00C28F80=1 and 0x00C28F81=caps byte +0x10 and returns it.
// Status/limits: TRANSCRIBED; only callee Debug_Error is an engine native; the device object is runtime data (0x005C8E1C). Return leaves the caps byte in eax (callers ignore it) - documented limit. Runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/input/dinput_globals.hpp"

#include <cstdint>

extern "C" {

std::int32_t Input_MouseGetCaps_48FD90()
{
    // 0x0048FD90-0x0048FDA9: sub esp,0x2C; mov caps.dwSize,0x2C; load the mouse device vtable.
    std::uint8_t caps[0x2C] = {};
    *reinterpret_cast<std::uint32_t*>(&caps[0]) = 0x2CU;
    void* const device = g_5c8e1c;
    auto const* const* vtable = *reinterpret_cast<void* const* const*>(device);
    using GetCapabilitiesFn = std::int32_t(__stdcall*)(void*, void*);
    const GetCapabilitiesFn get_caps = reinterpret_cast<GetCapabilitiesFn>(vtable[0x0C / 4]);
    // 0x0048FDAA-0x0048FDBB: call [vtable+0x0C]; test eax.
    if (get_caps(device, caps) != 0)
    {
        // 0x0048FDBD-0x0048FDD0: push "Could not get mouse caps!"; call Debug_Error; jmp $.
        Debug_Error("Could not get mouse caps!");
        for (;;)
        {
        }
    }
    // 0x0048FDC2-0x0048FDCF: mov byte [0xC28F80],1; mov al,[caps+0x10]; mov [0xC28F81],al.
    g_c28f80 = 1U;
    g_c28f81 = caps[0x10];
    return static_cast<std::int32_t>(caps[0x10]);
}

} // extern "C"
