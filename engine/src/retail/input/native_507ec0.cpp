// Native: Input_Run; retail VA 0x00507EC0-0x00507F0E (78 bytes, 8 blocks, 0 direct callees).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 disassembly/bytes and Binary Ninja MCP decompile; no absolute data references.
// Behavior/CFG: slot[6]=slot[7]; if slot[0] call it as callback(slot, arg2); slot[5]=~slot[6] & slot[7]; for i in 0..31: if slot[7] bit i set then ++word[slot+0x20+2i] else 0; return slot bytes + 0x60.
// Status/limits: TRANSCRIBED; no native dependencies (the callback pointer is runtime data). Slot layout is the retail's: dwords +0x14/+0x18/+0x1C, counters at +0x20, return past the 32 int16 counters. Runtime UNOBSERVED.
#include "igi/retail/input/native_507ec0.hpp"

#include <cstdint>

extern "C" {

std::int16_t* Input_Run(std::int32_t* slot, std::int32_t arg2)
{
    // 0x00507EC5-0x00507EC8: mov eax,[esi+0x1C]; mov [esi+0x18],eax (slot[6] = slot[7]).
    slot[6] = slot[7];
    // 0x00507ECB-0x00507ED9: indirect callback slot[0](slot, arg2) when non-null.
    if (slot[0] != 0)
    {
        using SlotCallback = void (*)(std::int32_t*, std::int32_t);
        reinterpret_cast<SlotCallback>(slot[0])(slot, arg2);
    }
    // 0x00507EDC-0x00507EE8: eax=~slot[6]; eax&=slot[7]; slot[5]=eax.
    const std::uint32_t previous = static_cast<std::uint32_t>(slot[6]);
    const std::uint32_t current = static_cast<std::uint32_t>(slot[7]);
    slot[5] = static_cast<std::int32_t>((~previous) & current);
    // 0x00507EEB-0x00507F0A: 32 int16 counters at slot+0x20, incremented when the bit is held, cleared otherwise.
    std::int16_t* counter = reinterpret_cast<std::int16_t*>(reinterpret_cast<std::uint8_t*>(slot) + 0x20);
    for (std::int32_t i = 0; i < 0x20; ++i)
    {
        const std::uint32_t bit = 1U << (i & 31);
        if ((current & bit) != 0U)
        {
            ++(*counter);
        }
        else
        {
            *counter = 0;
        }
        counter += 1;
    }
    // 0x00507F0C-0x00507F0D: pop esi; ret - eax points past the 32 counters (slot bytes + 0x60).
    return reinterpret_cast<std::int16_t*>(reinterpret_cast<std::uint8_t*>(slot) + 0x60);
}

} // extern "C"
