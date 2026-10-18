// IGI native provenance: Script_ParseEnumString_8 - 0x004F1AF0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004F1AF0-Script_ParseEnumString_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumString_8.h"
int Script_ParseEnumString_8(int context, int arg2, char arg3, int arg4)

{
    return (*(((arg2 * 0x13 + arg3) << 2) + &g_a758e8))(context, arg4);
}
