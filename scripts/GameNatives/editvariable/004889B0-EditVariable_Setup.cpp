// IGI native provenance: EditVariable_Setup - 0x004889B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004889B0-EditVariable_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004889B0-EditVariable_Setup.hpp"
int* EditVariable_Setup(long double context , void* arg2)

{
    if (Script_FormatString_6(arg2 + 0x38))
    {
        Script_SpawnNewTask_7(arg2 + 0x38, arg2);
        *(arg2 + 0xd8) += context;
    }

    int* result = Script_FormatString_6(arg2 + 0x88);

    if (result)
    {
        Script_SpawnNewTask_7(arg2 + 0x88, arg2);
🚫🚫🚫        int item_8_2 = /* int item_8_2 = (unimplemented  {fistp qword [stack_ptr+0x8], fp0}).d */;
        /* unimplemented  {fistp qword [stack_ptr+0x8], fp0} */
        result = *(arg2 + 0xd8) - item_8_2;
        *(arg2 + 0xd8) = result;
    }

    return result;
}
