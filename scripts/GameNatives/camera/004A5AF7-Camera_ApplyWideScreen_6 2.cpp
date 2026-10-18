// IGI native provenance: Camera_ApplyWideScreen_6 - 0x004A5AF7 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A5AF7-Camera_ApplyWideScreen_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyWideScreen_6.h"
int Camera_ApplyWideScreen_6(unsigned int context, int arg2, int arg3)

{
    if (g_9360ac == 1)
    {
        TerminateProcess(GetCurrentProcess(), context);
        /* no return */
    }

    g_9360a8 = 1;
    g_9360a4 = arg3;

    if (!arg2)
    {
        int data_2 = g_c32550;

        if (data_2)
        {
            int* i = g_c3254c - 4;

            if (i >= data_2)
            {
                do
                {
                    int data_3 = *i;

                    if (data_3)
                        data_3();

                    i -= 4;
                } while (i >= g_c32550);
            }
        }

        Camera_ApplyPostProcess_6(0x536018, 0x536020);
    }

    int result = Camera_ApplyPostProcess_6(0x536024, 0x536028);

    if (arg3)
        return result;

    g_9360ac = 1;
    ExitProcess(context);
    /* no return */
}
