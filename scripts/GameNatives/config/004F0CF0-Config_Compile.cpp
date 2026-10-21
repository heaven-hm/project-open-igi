// IGI native provenance: Config_Compile - 0x004F0CF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F0CF0-Config_Compile.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_Compile.h"
int Config_Compile(int context, void* arg2, char arg3)

// Sig: int Config_Compile(int context, char* cfgName, int flags)
// Note: Compiles a CONFIG script: validates context, then loads its compiled
// QVM via QvmLoad (0x4B80B0); references the Task_New name during task-table
// wiring. Params: context = config context id, cfgName = config path/name,
// flags = compile flags. Use when (re)loading game configuration
// programmatically.
{
    Script_SetSymbolContext("Task_New", context);
    int* data_val = Qvm_Load(arg2);

    if (!data_val)
        return 0;

    Script_Init(arg2, 1, 0, arg3);
    Qvm_Execute(data_val);
    Qvm_Cleanup(data_val);
    return g_a758a8;
}
