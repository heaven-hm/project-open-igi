// IGI native provenance: Script_ParseEnumReal32 - 0x004F0C90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F0C90-Script_ParseEnumReal32.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumReal32.h"
int Script_ParseEnumReal32(int* context)

{
    Qtask_UpdateList(context[2]);
    HeapList_Remove(context);
    return Sys_ExecuteBytecode_2(context);
}
