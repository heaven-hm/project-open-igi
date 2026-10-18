// IGI native provenance: Physics_CouldNotCreateDirect - 0x00509980 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00509980-Physics_CouldNotCreateDirect.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_CouldNotCreateDirect.hpp"
int Physics_CouldNotCreateDirect()

{
    int source_val = 0;
    g_a7b638 = 0;

    if (!DirectInputCreateA(FX_TrailUpdate_21(), 0x700))
    {
        int* data_2 = g_a7b4f0;
        int context_val;
        int item_10 = context_val;
        int dest_val;
        int item_14_1 = dest_val;
        (*(*data_2 + 0x10))(data_2, 4, Physics_SolveConstraints_9, 0, 1);
        int result = g_a7b638;

        if (result && result > 0)
        {
            int* context_1 = &g_a7b4e0;
            int* dest_1 = &g_a7b430;

            while (true)
            {
                if (!Physics_CalculateSplineLength_10(source_val))
                {
                    Error_Show("Could not acquire one or more joystick devices.");

                    while (true)
                        /* nop */
                }
                else
                {
                    int* data_4 = *context_1;
                    *dest_1 = 0x2c;

                    if ((*(*data_4 + 0xc))(data_4, dest_1))
                    {
                        Error_Show("Could not get capabilities for one or more joysticks .");

                        while (true)
                            /* nop */
                    }
                    else
                    {
                        Physics_SimulateRigidBody_10(source_val);
                        result = g_a7b638;
                        source_val += 1;
                        dest_1 = &dest_1[0xb];
                        context_1 = &context_1[1];

                        if (source_val >= result)
                            break;
                    }
                }
            }
        }

        return result;
    }

    Error_Show("Could not create DirectInput joypad object.");

    while (true)
        /* nop */
}
