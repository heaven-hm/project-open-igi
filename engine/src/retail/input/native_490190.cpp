// Native: Keyboard_VkToStateIndex_490190 (placeholder until VERIFIED); retail VA 0x00490190-0x004901F2 (99 bytes, 10 blocks, jump table 0x004901F4, selector bytes 0x00490214).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP disassembly + radare2 6.2.0 full bytes + angr CFGFast; DINPUT-CALLBACKS packet §3.2 (jump table entry order and the 0x18 selector bytes quoted there).
// Behavior/CFG: if vk-0x11 > 0x17 -> (lParam>>16)&0x7F; else select via the 24-byte table at 0x00490214: selector 0 (VK_CONTROL/VK_MENU) -> ((lParam>>1)&0x800000 | lParam&0x7F0000)>>16, 1..6 -> the fixed VK_* scan codes 0xC9/0xD1/0xCB/0xC8/0xCD/0xD0, 7 -> the (lParam>>16)&0x7F path.
// Status/limits: TRANSCRIBED; no callees; the two image tables are transcribed byte-for-byte below with their retail VAs. Runtime UNOBSERVED.
#include <cstdint>

extern "C" {

namespace {

// Retail jump-target order 0x004901F4: 0x4901CF, 0x4901B1, 0x4901AB, 0x4901C3, 0x4901B7, 0x4901C9, 0x4901BD, 0x4901E8.
// Selector bytes at 0x00490214: 00 00 07 07 07 07 07 07 07 07 07 07 07 07 07 07 01 02 07 07 03 04 05 06.
std::uint8_t const kSelector[24] = {
    0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x01, 0x02, 0x07, 0x07, 0x03, 0x04, 0x05, 0x06};

} // namespace

std::int32_t Keyboard_VkToStateIndex_490190(std::int32_t vk, std::int32_t lParam)
{
    // 0x00490190-0x0049019A: eax = vk-0x11; cmp eax,0x17; ja 0x004901E8.
    const std::uint32_t offset = static_cast<std::uint32_t>(vk - 0x11);
    if (offset > 0x17U)
    {
        // 0x004901E8: sar eax,0x10; and eax,0x7F.
        return (lParam >> 16) & 0x7F;
    }
    // 0x0049019C-0x004901A4: selector byte -> jump table.
    switch (kSelector[offset])
    {
    // 0x004901CF: ((lParam>>1)&0x800000 | lParam&0x7F0000)>>16.
    case 0:
        return ((((lParam >> 1) & 0x800000) | (lParam & 0x7F0000)) >> 16);
    // 0x004901B1: VK_* fixed scan codes.
    case 1:
        return 0xC9; // VK_PRIOR -> extended 0xC9
    // 0x004901AB
    case 2:
        return 0xD1; // VK_NEXT -> extended 0xD1
    // 0x004901C3
    case 3:
        return 0xCB; // VK_LEFT -> extended 0xCB
    // 0x004901B7
    case 4:
        return 0xC8; // VK_UP -> extended 0xC8
    // 0x004901C9
    case 5:
        return 0xCD; // VK_RIGHT -> extended 0xCD
    // 0x004901BD
    case 6:
        return 0xD0; // VK_DOWN -> extended 0xD0
    // 0x004901E8 (selector 7): sar eax,0x10; and eax,0x7F.
    default:
        return (lParam >> 16) & 0x7F;
    }
}

} // extern "C"
