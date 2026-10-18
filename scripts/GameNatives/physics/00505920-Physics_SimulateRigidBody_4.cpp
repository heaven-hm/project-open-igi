// IGI native provenance: Physics_SimulateRigidBody_4 - 0x00505920 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00505920-Physics_SimulateRigidBody_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_SimulateRigidBody_4.hpp"
void* const Physics_SimulateRigidBody_4(int context, int arg2, void* arg3, void* arg4, int arg5, int arg6)

{
    void* const result_1 = arg3;
    void* const result = arg4;
    result_1 = nullptr;
    int* i_1 = *(result + 8);

    if (*i_1 && i_1)
    {
        int* i;

        do
        {
            int* data_val = *i_1;

            if (!data_val)
                i = nullptr;
            else
            {
                int dest_1 = *data_val;
                int dest_2 = -(dest_1);
                i = (dest_2 - dest_2) & data_val;
            }

            unsigned int data_2 = Physics_CalculateSplineLength_5();
            int temp_val = 0;
            temp_val = i_1[7];
            result = *(((data_2 * 0x180 + temp_val) << 2) + &g_a96ae0);

            if (result)
            {
                unsigned int data_4 = Physics_CalculateSplineLength_5();
                int entity_3 = 0;
                entity_3 = i_1[7];
                (*(((data_4 * 0x180 + entity_3) << 2) + &g_a96ae0))(i_1, &result_1);
                result = result_1;

                if (result)
                {
                    result = arg5(result, arg6);
                    result_1 = nullptr;
                }
            }

            i_1 = i;
        } while (i);
    }

    return result;
}
