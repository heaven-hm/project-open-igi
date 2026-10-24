// IGI native provenance: Table_GetAt - 0x004F2060 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F2060-Script_ParseEnumString_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F2060-Table_GetAt.hpp"
int Table_GetAt(int context, int arg2)

{
    return *(context + (arg2 << 2));
}
