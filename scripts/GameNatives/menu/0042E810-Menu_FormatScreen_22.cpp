// IGI native provenance: Menu_FormatScreen_22 - 0x0042E810 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0042E810-Menu_FormatScreen_22.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_FormatScreen_22.hpp"
int* Menu_FormatScreen_22(long double context , void* arg2)

{
    int* result = Script_FormatString_6(arg2 + 0x928);

    if (result)
    {
        Script_SpawnNewTask_7(arg2 + 0x928, arg2);
        short data_val = Menu_DrawWidget_17();
        result = QTask_DeleteTask_2(*(arg2 + 0xe8), data_val);

        if (result)
            return Menu_HandleInput_17(result, context);
    }

    return result;
}
