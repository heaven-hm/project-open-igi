// IGI native provenance: Script_SpawnNewTask_32 - 0x004FEAC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FEAC0-Script_SpawnNewTask_32.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004FEAC0-Script_SpawnNewTask_32.hpp"
void* Script_SpawnNewTask_32(void* context, int arg2)

{
    *(context + 0x120) = arg2;
    *(context + 0x20) = 0;
    return context;
}
