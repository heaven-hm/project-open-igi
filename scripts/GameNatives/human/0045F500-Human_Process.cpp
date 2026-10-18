// IGI native provenance: Human_Process - 0x0045F500 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045F500-Human_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Human_Process.hpp"
char* Human_Process(float context, int* arg2)

{
    char data_val = arg2[3];
    double* context_val = *arg2;
    double* flagP = arg2[1];
    char* result;

    if (data_val == 0xff)
    {
    label_45f54f:
        result = AI_MoveToNode_17(context, context_val, flagP, arg2[2]);

        if (result)
        {
            AI_FireWeapon_17(context);
            return (*((Render_DrawRigidMesh_12() << 2) + &g_a9a0e0))(context, arg2);
        }
    }
    else
    {
        int entity_1 = 0;
        entity_1 = *(context + 0x1c);
        int data_5 = *(((data_val * 0x180 + entity_1) << 2) + &g_a96ae0);

        if (!data_5)
            goto label_45f54f;

        data_5(context, arg2[4]);
        result = arg2[4];

        if (*result)
            goto label_45f54f;
    }

    return result;
}
