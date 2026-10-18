// IGI native provenance: Display_SwapBuffers_11 - 0x00496770 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00496770-Display_SwapBuffers_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SwapBuffers_11.hpp"
int Display_SwapBuffers_11(int context, int arg2, int arg3, int* arg4)

{
    if (!g_6840e9)
        return 0;

    int* saved_frame_4 = arg4;

    if (!saved_frame_4)
    {
        void item_58;
        Display_SetGammaRamp_12(&item_58);
        saved_frame_4 = &item_58;
        arg4 = &item_58;
    }

    int* saved_frame_2 = saved_frame_4;
    int result = Display_ClearSurfaces_11(context, arg2, arg3);

    if (!result)
        return result;

    Display_SyncVSync_12(result, saved_frame_4);
    void* entity_1 = *(result + 0xc);
    short data_2 = *(entity_1 + 0x14);

    if (data_2 == 2 || !data_2)
    {
        int* data_12 = *(result + 0x10);
        (*(*data_12 + 0x30))(data_12, 0, 0, 1);
        return result;
    }

    int data_6;
    int temp_2;
    temp_2 = HIGHD(*(entity_1 + 0x18) * *(entity_1 + 0x16) * *(entity_1 + 0x1c));
    data_6 = LOWD(*(entity_1 + 0x18) * *(entity_1 + 0x16) * *(entity_1 + 0x1c));
    int* context_val = *(result + 0x10);
    (*(*context_val + 0x34))(context_val, ((data_6 + (temp_2 & 7)) >> 3) * arg4[0x14]);
    int* data_10 = *(result + 0x10);
    (*(*data_10 + 0x30))(data_10, 0, 0, arg3 >> 4 & 1);
    return result;
}
