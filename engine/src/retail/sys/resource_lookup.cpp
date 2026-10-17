#include "igi/retail/sys/resource_lookup.hpp"

namespace igi::retail::sys {

// Native: String_ToUpperChar() - VA 0x004AD73D - complete port.
// Previous catalog alias: Camera_ApplyWideScreen_28.
std::uint32_t String_ToUpperChar(std::uint32_t character)
{
    if (character >= 0x61U && character <= 0x7AU)
    {
        return character - 0x20U;
    }
    return character;
}

// Native: Resource_CanonicalizePath() - VA 0x004B5AF0 - complete port.
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

    do
    {
        character = static_cast<char>(String_ToUpperChar(static_cast<std::uint32_t>(
            static_cast<std::uint8_t>(character))));
        *out = character;
        character = source[1];
        ++out;
        ++source;
    } while (character != 0);

    *out = character;
    return character;
}

// Native: Resource_CalculatePathHash() - VA 0x004B5B30 - complete port.
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
