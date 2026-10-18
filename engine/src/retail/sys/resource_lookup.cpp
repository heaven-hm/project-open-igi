#include "igi/retail/sys/resource_lookup.hpp"

namespace igi::retail::sys {

// Native: String_ToUpperChar; retail VA 0x004AD73D; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.
// Behavior/CFG: not restated here.
// Status/limits: STRUCTURALLY_VERIFIED; see verification record for scope and port limits.
// Previous catalog alias: Camera_ApplyWideScreen_28.
std::uint32_t String_ToUpperChar(std::uint32_t character)
{
    if (character >= 0x61U && character <= 0x7AU)
    {
        return character - 0x20U;
    }
    return character;
}

// Native: Resource_CanonicalizePath; retail VA 0x004B5AF0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.
// Behavior/CFG: not restated here.
// Status/limits: STRUCTURALLY_VERIFIED; see verification record for scope and port limits.
// Previous catalog alias: Sys_ResolveSymbol_15.
std::int32_t Resource_CanonicalizePath(char* destination, char* source)
{
    char character = *source;

    if (character == 0)
    {
        *destination = 0;
        return character;
    }

    char* out = destination;
    std::uint32_t uppercase_result = 0;

    do
    {
        // Retail MOVSX at 0x004B5B00 passes a signed byte to the CRT helper.
        uppercase_result = String_ToUpperChar(static_cast<std::uint32_t>(
            static_cast<std::int32_t>(static_cast<std::int8_t>(character))));
        character = static_cast<char>(uppercase_result);
        *out = character;
        character = source[1];
        ++out;
        ++source;
    } while (character != 0);

    *out = character;
    // MOV AL,[EDI+1] leaves EAX's upper bytes from the final uppercase call.
    return static_cast<std::int32_t>(uppercase_result & 0xFFFFFF00U);
}

// Native: Resource_CalculatePathHash; retail VA 0x004B5B30; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.
// Behavior/CFG: not restated here.
// Status/limits: STRUCTURALLY_VERIFIED; see verification record for scope and port limits.
// Previous catalog alias: Sys_CheckHeap_15.
std::uint32_t Resource_CalculatePathHash(const char* text)
{
    const char* cursor = text;
    std::uint32_t data = 1U;
    char character = *cursor;

    while (character != 0)
    {
        data = (data + 1U) * static_cast<std::uint32_t>(static_cast<std::int8_t>(character));
        character = cursor[1];
        ++cursor;
    }

    return data % 0x3037U;
}

} // namespace igi::retail::sys
