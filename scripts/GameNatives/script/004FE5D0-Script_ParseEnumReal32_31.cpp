// IGI native provenance: Script_ParseEnumReal32_31 - 0x004FE5D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FE5D0-Script_ParseEnumReal32_31.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004FE5D0-Script_ParseEnumReal32_31.hpp"
int Script_ParseEnumReal32_31(void* context, char arg2)

{
    *(context + 0xfd) = arg2;
    return arg2;
}
