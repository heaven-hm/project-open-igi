// IGI native provenance: SubLevel_LoadParameters - 0x00488780 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00488780-SubLevel_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SubLevel_LoadParameters.h"
int* SubLevel_LoadParameters(void* context)

{
    int* result = QTask_CleanupTypes(context);
    result = *(context + 0x20);

    if (result)
    {
        result = QFile_ReadWrite(context + 0x20, "r");

        if (result)
        {
            Device_CloseStream(result);
            result = Qvm_Load(context + 0x20);

            if (result)
            {
                Script_SetSymbolContext("Task_New", context);
                Qvm_Execute(result);
                return Qvm_Cleanup(result);
            }
        }
    }

    return result;
}
