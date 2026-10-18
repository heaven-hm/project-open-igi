// IGI native provenance: Script_ParseEnumReal32_13 - 0x004F23D0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004F23D0-Script_ParseEnumReal32_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F23D0-Script_ParseEnumReal32_13.hpp"
int Script_ParseEnumReal32_13(void* context, int arg2)

{
    int temp_val = 0;
    temp_val = *(context + 0x1c);
    return (*(((*g_a76c10 * 0x180 + temp_val) << 2) + &g_a96ae0))(context, arg2);
}
