// IGI native provenance: String_ToUpperChar - 0x004AD73D - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AD73D-String_ToUpperChar.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AD73D-String_ToUpperChar.hpp"
extern "C" {
extern int g_9361e4;
}
char String_ToUpperChar(char value)
{
    if (!g_9361e4) {
        if (value >= 'a' && value <= 'z')
            return (char)(value - 32);
        return value;
    }
    return (char)String_ToUpperCharLocale(value);
}
